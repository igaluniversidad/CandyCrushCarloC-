#include "Board.h"

// ---------------------------------------------------------------------
// Board.cpp - Logica pura. Prohibido incluir SDL aqui (regla de oro).
// Todo son numeros en dos Grid: colores (int) y bombas (bool).
// fila = renglon 0..7 (vertical). columna = 0..7 (horizontal).
// ---------------------------------------------------------------------

Board::Board(unsigned int semillaInicial)
{
    _coloresTablero = new Grid<int>(ROWS, COLS);
    _bombasTablero = new Grid<bool>(ROWS, COLS);
    _semillaAleatoria = semillaInicial ? semillaInicial : 12345u;
    RandomFillNoMatch();
}

Board::~Board()
{
    delete _coloresTablero;
    delete _bombasTablero;
    _coloresTablero = nullptr;
    _bombasTablero = nullptr;
}

// Da un color aleatorio 0..TYPES-1 usando nuestra semilla LCG propia.
int Board::RandType()
{
    return (int)(MatchLogic::LcgNext(_semillaAleatoria) % (unsigned int)TYPES);
}

int Board::Get(int fila, int columna)
{
    if (!InBounds(fila, columna)) return EMPTY;
    return _coloresTablero->Get(fila, columna);
}

bool Board::IsBomb(int fila, int columna)
{
    if (!InBounds(fila, columna)) return false;
    return _bombasTablero->Get(fila, columna);
}

bool Board::IsEmptyCell(int fila, int columna)
{
    if (!InBounds(fila, columna)) return true;
    return _coloresTablero->Get(fila, columna) == EMPTY;
}

void Board::SetCell(int fila, int columna, int color, bool esBomba)
{
    if (!InBounds(fila, columna)) return;
    _coloresTablero->Set(fila, columna, color);
    _bombasTablero->Set(fila, columna, esBomba);
}

void Board::SwapCells(int filaOrigen, int columnaOrigen,
                      int filaDestino, int columnaDestino)
{
    if (!InBounds(filaOrigen, columnaOrigen)) return;
    if (!InBounds(filaDestino, columnaDestino)) return;
    int colorOrigen = _coloresTablero->Get(filaOrigen, columnaOrigen);
    int colorDestino = _coloresTablero->Get(filaDestino, columnaDestino);
    bool bombaOrigen = _bombasTablero->Get(filaOrigen, columnaOrigen);
    bool bombaDestino = _bombasTablero->Get(filaDestino, columnaDestino);
    _coloresTablero->Set(filaOrigen, columnaOrigen, colorDestino);
    _coloresTablero->Set(filaDestino, columnaDestino, colorOrigen);
    _bombasTablero->Set(filaOrigen, columnaOrigen, bombaDestino);
    _bombasTablero->Set(filaDestino, columnaDestino, bombaOrigen);
}

void Board::RandomFillNoMatch()
{
    for (int fila = 0; fila < ROWS; ++fila)
    {
        for (int columna = 0; columna < COLS; ++columna)
        {
            int colorCandidato = 0;
            // Elige un color que NO forme 3 con los 2 de la izquierda
            // ni con los 2 de arriba. Asi el tablero nace sin matches.
            for (int intento = 0; intento < 50; ++intento)
            {
                colorCandidato = RandType();
                bool formaTrioHorizontal = (columna >= 2 &&
                    _coloresTablero->Get(fila, columna - 1) == colorCandidato &&
                    _coloresTablero->Get(fila, columna - 2) == colorCandidato);
                bool formaTrioVertical = (fila >= 2 &&
                    _coloresTablero->Get(fila - 1, columna) == colorCandidato &&
                    _coloresTablero->Get(fila - 2, columna) == colorCandidato);
                if (!formaTrioHorizontal && !formaTrioVertical) break;
            }
            _coloresTablero->Set(fila, columna, colorCandidato);
            _bombasTablero->Set(fila, columna, false);
        }
    }
}

void Board::ShuffleNoMatch()
{
    // Fisher-Yates sobre arreglo plano + reintentos hasta condicion valida.
    int coloresPlanos[ROWS * COLS];
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
            coloresPlanos[fila * COLS + columna] =
                _coloresTablero->Get(fila, columna);

    bool marcasTemporales[ROWS][COLS];
    for (int intentoMezcla = 0; intentoMezcla < 200; ++intentoMezcla)
    {
        // Barajar el arreglo plano
        for (int indice = ROWS * COLS - 1; indice > 0; --indice)
        {
            unsigned int indiceAleatorio =
                MatchLogic::LcgNext(_semillaAleatoria) % (unsigned int)(indice + 1);
            int temporal = coloresPlanos[indice];
            coloresPlanos[indice] = coloresPlanos[indiceAleatorio];
            coloresPlanos[indiceAleatorio] = temporal;
        }
        for (int fila = 0; fila < ROWS; ++fila)
            for (int columna = 0; columna < COLS; ++columna)
            {
                _coloresTablero->Set(fila, columna,
                    coloresPlanos[fila * COLS + columna]);
                _bombasTablero->Set(fila, columna, false); // el shuffle limpia bombas
            }
        if (FindMatches(marcasTemporales) == 0 && HasPossibleMove())
            return;
    }
    // Si tras 200 intentos no se logro (casi imposible en 8x8/6 tipos),
    // regenera desde cero que por construccion cumple ambas.
    RandomFillNoMatch();
}

int Board::FindMatches(bool marcaDestruccion[ROWS][COLS])
{
    bool marcasPlanas[ROWS * COLS];
    int totalMarcadas = MatchLogic::ScanMatches(
        _coloresTablero, ROWS, COLS, marcasPlanas);
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
            marcaDestruccion[fila][columna] = marcasPlanas[fila * COLS + columna];
    return totalMarcadas;
}

bool Board::HasPossibleMove()
{
    SwapHint pista;
    return MatchLogic::FindAnyHint(_coloresTablero, ROWS, COLS, pista);
}

bool Board::FindHint(int& filaOrigen, int& columnaOrigen,
                     int& filaDestino, int& columnaDestino)
{
    // Si hay bomba en el tablero, sugerir usarla es valido: busca cualquier
    // bomba y propone intercambiarla con su vecino derecho/abajo.
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
            if (_bombasTablero->Get(fila, columna))
            {
                if (columna + 1 < COLS)
                {
                    filaOrigen = fila; columnaOrigen = columna;
                    filaDestino = fila; columnaDestino = columna + 1;
                    return true;
                }
                if (fila + 1 < ROWS)
                {
                    filaOrigen = fila; columnaOrigen = columna;
                    filaDestino = fila + 1; columnaDestino = columna;
                    return true;
                }
            }
    SwapHint pista;
    if (!MatchLogic::FindAnyHint(_coloresTablero, ROWS, COLS, pista)) return false;
    filaOrigen = pista.r1; columnaOrigen = pista.c1;
    filaDestino = pista.r2; columnaDestino = pista.c2;
    return true;
}

int Board::CollectRec(int fila, int columna, int colorObjetivo,
                      bool* visitadosPlano, bool* marcasPlano)
{
    if (fila < 0 || fila >= ROWS || columna < 0 || columna >= COLS) return 0;
    int indicePlano = fila * COLS + columna;
    if (visitadosPlano[indicePlano]) return 0;
    if (_coloresTablero->Get(fila, columna) != colorObjetivo) return 0;
    visitadosPlano[indicePlano] = true;
    marcasPlano[indicePlano] = true;
    int totalConectadas = 1;
    totalConectadas += CollectRec(fila + 1, columna, colorObjetivo,
                                 visitadosPlano, marcasPlano);
    totalConectadas += CollectRec(fila - 1, columna, colorObjetivo,
                                 visitadosPlano, marcasPlano);
    totalConectadas += CollectRec(fila, columna + 1, colorObjetivo,
                                 visitadosPlano, marcasPlano);
    totalConectadas += CollectRec(fila, columna - 1, colorObjetivo,
                                 visitadosPlano, marcasPlano);
    return totalConectadas;
}

int Board::DetonatePreview(int filaBomba, int columnaBomba,
                           bool marcaDestruccion[ROWS][COLS])
{
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
            marcaDestruccion[fila][columna] = false;
    if (!InBounds(filaBomba, columnaBomba)) return 0;
    if (_coloresTablero->Get(filaBomba, columnaBomba) == EMPTY) return 0;

    // PASO 1 (obligatorio): tu FloodFill cuenta la mancha del mismo color.
    int celdasContadas = _coloresTablero->FloodFill(filaBomba, columnaBomba);
    (void)celdasContadas;

    // PASO 2: recolector gemelo que junta las celdas (para encolarlas).
    bool visitados[ROWS * COLS];
    bool marcadas[ROWS * COLS];
    for (int i = 0; i < ROWS * COLS; ++i)
    {
        visitados[i] = false;
        marcadas[i] = false;
    }
    int colorBomba = _coloresTablero->Get(filaBomba, columnaBomba);
    int totalADestruir = CollectRec(filaBomba, columnaBomba, colorBomba,
                                   visitados, marcadas);
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
            marcaDestruccion[fila][columna] = marcadas[fila * COLS + columna];
    return totalADestruir;
}

void Board::ApplyGravity()
{
    for (int columna = 0; columna < COLS; ++columna)
    {
        // escritor = fila mas baja donde debe caer la siguiente gema viva.
        int filaEscritura = ROWS - 1;
        for (int filaLectura = ROWS - 1; filaLectura >= 0; --filaLectura)
        {
            int colorActual = _coloresTablero->Get(filaLectura, columna);
            if (colorActual != EMPTY)
            {
                if (filaEscritura != filaLectura)
                {
                    _coloresTablero->Set(filaEscritura, columna, colorActual);
                    _bombasTablero->Set(filaEscritura, columna,
                        _bombasTablero->Get(filaLectura, columna));
                    _coloresTablero->Set(filaLectura, columna, EMPTY);
                    _bombasTablero->Set(filaLectura, columna, false);
                }
                filaEscritura--;
            }
        }
        // Rellena los huecos de arriba con dulces nuevos aleatorios.
        for (int filaNueva = filaEscritura; filaNueva >= 0; --filaNueva)
        {
            _coloresTablero->Set(filaNueva, columna, RandType());
            _bombasTablero->Set(filaNueva, columna, false);
        }
    }
}

void Board::SaveTo(BoardSnapshot& snapshotSalida, int puntajeActual,
                   int movimientosActuales) const
{
    snapshotSalida.filas = ROWS;
    snapshotSalida.columnas = COLS;
    snapshotSalida.puntaje = puntajeActual;
    snapshotSalida.movimientos = movimientosActuales;
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
        {
            snapshotSalida.colores[fila][columna] =
                _coloresTablero->Get(fila, columna);
            snapshotSalida.bombas[fila][columna] =
                _bombasTablero->Get(fila, columna);
        }
}

void Board::LoadFrom(const BoardSnapshot& snapshotOrigen)
{
    for (int fila = 0; fila < ROWS; ++fila)
        for (int columna = 0; columna < COLS; ++columna)
        {
            _coloresTablero->Set(fila, columna,
                snapshotOrigen.colores[fila][columna]);
            _bombasTablero->Set(fila, columna,
                snapshotOrigen.bombas[fila][columna]);
        }
}
