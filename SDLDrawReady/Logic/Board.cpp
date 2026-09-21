#include "Board.h" // trae fila, columna, colores, bombas

// Board.cpp = solo numeros. Aqui NO hay dibujos ni SDL.

// Crea el tablero con una semilla para el azar.
Board::Board(unsigned int semillaInicial)
{
    _coloresTablero = new Grid<int>(ROWS, COLS); // rejilla 8x8 de colores
    _bombasTablero = new Grid<bool>(ROWS, COLS); // rejilla 8x8 de bombas
    _semillaAleatoria = semillaInicial ? semillaInicial : 12345u; // guarda semilla (si es 0 usa 12345)
    RandomFillNoMatch(); // llena todo sin trios iniciales
}

// Destructor: borra las dos rejillas para no fugar memoria.
Board::~Board()
{
    delete _coloresTablero; // libera colores
    delete _bombasTablero; // libera bombas
    _coloresTablero = nullptr; // apunta a nada (seguro)
    _bombasTablero = nullptr; // apunta a nada (seguro)
}

// Da un color al azar entre 0 y 5 usando nuestra semilla.
int Board::RandType()
{
    return (int)(MatchLogic::LcgNext(_semillaAleatoria) % (unsigned int)TYPES); // avanza semilla y recorta a 0..5
}

// Que color hay en esta casilla. Si esta fuera, dice VACIO.
int Board::Get(int fila, int columna)
{
    if (!InBounds(fila, columna)) return EMPTY; // fuera = vacio
    return _coloresTablero->Get(fila, columna); // lee la rejilla
}

// Hay bomba en esta casilla? Fuera = no.
bool Board::IsBomb(int fila, int columna)
{
    if (!InBounds(fila, columna)) return false; // fuera = no hay
    return _bombasTablero->Get(fila, columna); // lee la rejilla
}

// Esta vacia? Fuera se cuenta como vacia. Si no, revisa si es -1.
bool Board::IsEmptyCell(int fila, int columna)
{
    if (!InBounds(fila, columna)) return true; // fuera = vacia
    return _coloresTablero->Get(fila, columna) == EMPTY; // -1 = vacia
}

// Pone un color y dice si es bomba en una casilla.
void Board::SetCell(int fila, int columna, int color, bool esBomba)
{
    if (!InBounds(fila, columna)) return; // fuera = no hace nada
    _coloresTablero->Set(fila, columna, color); // guarda color
    _bombasTablero->Set(fila, columna, esBomba); // guarda si es bomba
}

// Cambia dos casillas vecinas (color y bomba juntos).
void Board::SwapCells(int filaOrigen, int columnaOrigen,
                      int filaDestino, int columnaDestino)
{
    if (!InBounds(filaOrigen, columnaOrigen)) return; // origen fuera = nada
    if (!InBounds(filaDestino, columnaDestino)) return; // destino fuera = nada
    int colorOrigen = _coloresTablero->Get(filaOrigen, columnaOrigen); // guarda color 1
    int colorDestino = _coloresTablero->Get(filaDestino, columnaDestino); // guarda color 2
    bool bombaOrigen = _bombasTablero->Get(filaOrigen, columnaOrigen); // guarda bomba 1
    bool bombaDestino = _bombasTablero->Get(filaDestino, columnaDestino); // guarda bomba 2
    _coloresTablero->Set(filaOrigen, columnaOrigen, colorDestino); // origen recibe color 2
    _coloresTablero->Set(filaDestino, columnaDestino, colorOrigen); // destino recibe color 1
    _bombasTablero->Set(filaOrigen, columnaOrigen, bombaDestino); // origen recibe bomba 2
    _bombasTablero->Set(filaDestino, columnaDestino, bombaOrigen); // destino recibe bomba 1
}

// Llena todo al azar pero sin que nazcan trios.
void Board::RandomFillNoMatch()
{
    for (int fila = 0; fila < ROWS; ++fila) // recorre cada fila
    {
        for (int columna = 0; columna < COLS; ++columna) // recorre cada columna
        {
            int colorCandidato = 0; // color que vamos a probar
            // Prueba hasta 50 colores hasta hallar uno que no forme trio.
            for (int intento = 0; intento < 50; ++intento)
            {
                colorCandidato = RandType(); // saca color al azar
                // Trio horizontal? los 2 de la izquierda son iguales al candidato.
                bool formaTrioHorizontal = (columna >= 2 &&
                    _coloresTablero->Get(fila, columna - 1) == colorCandidato &&
                    _coloresTablero->Get(fila, columna - 2) == colorCandidato);
                // Trio vertical? los 2 de arriba son iguales al candidato.
                bool formaTrioVertical = (fila >= 2 &&
                    _coloresTablero->Get(fila - 1, columna) == colorCandidato &&
                    _coloresTablero->Get(fila - 2, columna) == colorCandidato);
                if (!formaTrioHorizontal && !formaTrioVertical) break; // bueno, lo usamos
            }
            _coloresTablero->Set(fila, columna, colorCandidato); // guarda el color bueno
            _bombasTablero->Set(fila, columna, false); // al inicio no hay bombas
        }
    }
}

// Mezcla el tablero si ya no hay jugadas. Max 200 intentos.
void Board::ShuffleNoMatch()
{
    // Copia todos los colores a una lista plana de 64.
    int coloresPlanos[ROWS * COLS]; // lista de 64 colores
    for (int fila = 0; fila < ROWS; ++fila) // recorre filas
        for (int columna = 0; columna < COLS; ++columna) // recorre columnas
            coloresPlanos[fila * COLS + columna] =
                _coloresTablero->Get(fila, columna); // copia el color

    bool marcasTemporales[ROWS][COLS]; // para revisar si hay trios
    for (int intentoMezcla = 0; intentoMezcla < 200; ++intentoMezcla) // 200 mezclas max
    {
        // Baraja la lista (Fisher-Yates: cambia cada posicion con una al azar).
        for (int indice = ROWS * COLS - 1; indice > 0; --indice) // del ultimo al primero
        {
            unsigned int indiceAleatorio =
                MatchLogic::LcgNext(_semillaAleatoria) % (unsigned int)(indice + 1); // azar 0..indice
            int temporal = coloresPlanos[indice]; // guarda uno
            coloresPlanos[indice] = coloresPlanos[indiceAleatorio]; // cambia
            coloresPlanos[indiceAleatorio] = temporal; // completa el cambio
        }
        // Vacía la lista mezclada de vuelta al tablero. Limpia bombas.
        for (int fila = 0; fila < ROWS; ++fila) // filas
            for (int columna = 0; columna < COLS; ++columna) // columnas
            {
                _coloresTablero->Set(fila, columna,
                    coloresPlanos[fila * COLS + columna]); // pega color mezclado
                _bombasTablero->Set(fila, columna, false); // sin bombas (regla simple)
            }
        // Si quedo sin trios Y con jugada posible, listo.
        if (FindMatches(marcasTemporales) == 0 && HasPossibleMove())
            return; // mezcla buena, salimos
    }
    RandomFillNoMatch(); // si 200 fallaron, crea uno nuevo desde cero
}

// Busca trios con escaneo de lineas. Marca true donde hay match.
int Board::FindMatches(bool marcaDestruccion[ROWS][COLS])
{
    bool marcasPlanas[ROWS * COLS]; // lista plana de marcas
    int totalMarcadas = MatchLogic::ScanMatches(
        _coloresTablero, ROWS, COLS, marcasPlanas); // escanea y cuenta
    for (int fila = 0; fila < ROWS; ++fila) // copia lista plana a matriz
        for (int columna = 0; columna < COLS; ++columna) // copia cada una
            marcaDestruccion[fila][columna] = marcasPlanas[fila * COLS + columna]; // pega marca
    return totalMarcadas; // cuantas casillas son match
}

// True si existe al menos una jugada valida en todo el tablero.
bool Board::HasPossibleMove()
{
    SwapHint pista; // aqui se guardaria la pista
    return MatchLogic::FindAnyHint(_coloresTablero, ROWS, COLS, pista); // busca cualquiera
}

// Te dice dos vecinas que al cambiarlas forman trio. false = bloqueado.
bool Board::FindHint(int& filaOrigen, int& columnaOrigen,
                     int& filaDestino, int& columnaDestino)
{
    // Si hay bomba, conviene usarla: propone cambiarla con el vecino.
    for (int fila = 0; fila < ROWS; ++fila) // busca en filas
        for (int columna = 0; columna < COLS; ++columna) // busca en columnas
            if (_bombasTablero->Get(fila, columna)) // aqui hay bomba?
            {
                if (columna + 1 < COLS) // hay vecino a la derecha?
                {
                    filaOrigen = fila; columnaOrigen = columna; // origen = bomba
                    filaDestino = fila; columnaDestino = columna + 1; // destino = derecha
                    return true; // listo
                }
                if (fila + 1 < ROWS) // hay vecino abajo?
                {
                    filaOrigen = fila; columnaOrigen = columna; // origen = bomba
                    filaDestino = fila + 1; columnaDestino = columna; // destino = abajo
                    return true; // listo
                }
            }
    SwapHint pista; // pista generica
    if (!MatchLogic::FindAnyHint(_coloresTablero, ROWS, COLS, pista)) return false; // no hay = bloqueado
    filaOrigen = pista.r1; columnaOrigen = pista.c1; // primera casilla
    filaDestino = pista.r2; columnaDestino = pista.c2; // segunda casilla
    return true; // si hay pista
}

// Busca la mancha del mismo color (arriba/abajo/izq/der). La marca para romper.
int Board::CollectRec(int fila, int columna, int colorObjetivo,
                      bool* visitadosPlano, bool* marcasPlano)
{
    if (fila < 0 || fila >= ROWS || columna < 0 || columna >= COLS) return 0; // fuera = 0
    int indicePlano = fila * COLS + columna; // convierte 2D a indice 0..63
    if (visitadosPlano[indicePlano]) return 0; // ya revisada = 0
    if (_coloresTablero->Get(fila, columna) != colorObjetivo) return 0; // otro color = 0
    visitadosPlano[indicePlano] = true; // marca como revisada
    marcasPlano[indicePlano] = true; // marca para destruir
    int totalConectadas = 1; // esta casilla cuenta 1
    totalConectadas += CollectRec(fila + 1, columna, colorObjetivo,
                                 visitadosPlano, marcasPlano); // suma la de abajo
    totalConectadas += CollectRec(fila - 1, columna, colorObjetivo,
                                 visitadosPlano, marcasPlano); // suma la de arriba
    totalConectadas += CollectRec(fila, columna + 1, colorObjetivo,
                                 visitadosPlano, marcasPlano); // suma la derecha
    totalConectadas += CollectRec(fila, columna - 1, colorObjetivo,
                                 visitadosPlano, marcasPlano); // suma la izquierda
    return totalConectadas; // total de la mancha
}

// Bomba: cuenta la mancha con FloodFill y la marca. NO destruye aqui.
int Board::DetonatePreview(int filaBomba, int columnaBomba,
                           bool marcaDestruccion[ROWS][COLS])
{
    for (int fila = 0; fila < ROWS; ++fila) // limpia marcas
        for (int columna = 0; columna < COLS; ++columna) // limpia cada una
            marcaDestruccion[fila][columna] = false; // todo en false
    if (!InBounds(filaBomba, columnaBomba)) return 0; // fuera = 0
    if (_coloresTablero->Get(filaBomba, columnaBomba) == EMPTY) return 0; // vacia = 0

    // PASO 1 (obligatorio en la tarea): FloodFill cuenta la mancha.
    int celdasContadas = _coloresTablero->FloodFill(filaBomba, columnaBomba); // cuenta
    (void)celdasContadas; // la cuenta ya quedo, seguimos a recolectar

    // PASO 2: recolecta las casillas de la mancha para encolarlas.
    bool visitados[ROWS * COLS]; // cuales ya vimos
    bool marcadas[ROWS * COLS]; // cuales se van a romper
    for (int i = 0; i < ROWS * COLS; ++i) // limpia las dos listas
    {
        visitados[i] = false; // no visitada
        marcadas[i] = false; // no marcada
    }
    int colorBomba = _coloresTablero->Get(filaBomba, columnaBomba); // color de la bomba
    int totalADestruir = CollectRec(filaBomba, columnaBomba, colorBomba,
                                   visitados, marcadas); // junta la mancha
    for (int fila = 0; fila < ROWS; ++fila) // copia a la matriz de salida
        for (int columna = 0; columna < COLS; ++columna) // copia cada una
            marcaDestruccion[fila][columna] = marcadas[fila * COLS + columna]; // pega marca
    return totalADestruir; // cuantas se romperian
}

// Gravedad: baja todo y rellena arriba con dulces nuevos.
void Board::ApplyGravity()
{
    for (int columna = 0; columna < COLS; ++columna) // columna por columna
    {
        int filaEscritura = ROWS - 1; // fila mas baja donde cae lo vivo
        for (int filaLectura = ROWS - 1; filaLectura >= 0; --filaLectura) // sube desde abajo
        {
            int colorActual = _coloresTablero->Get(filaLectura, columna); // color de esta fila
            if (colorActual != EMPTY) // si no esta vacia, debe bajar
            {
                if (filaEscritura != filaLectura) // si no ya esta en su lugar
                {
                    _coloresTablero->Set(filaEscritura, columna, colorActual); // baja el color
                    _bombasTablero->Set(filaEscritura, columna,
                        _bombasTablero->Get(filaLectura, columna)); // baja su bomba
                    _coloresTablero->Set(filaLectura, columna, EMPTY); // deja vacio arriba
                    _bombasTablero->Set(filaLectura, columna, false); // sin bomba arriba
                }
                filaEscritura--; // siguiente hueco sube uno
            }
        }
        // Rellena los huecos que quedaron arriba con dulces nuevos.
        for (int filaNueva = filaEscritura; filaNueva >= 0; --filaNueva) // de donde quedo hasta arriba
        {
            _coloresTablero->Set(filaNueva, columna, RandType()); // dulce nuevo al azar
            _bombasTablero->Set(filaNueva, columna, false); // nuevo = sin bomba
        }
    }
}

// Guarda foto del tablero (para Undo).
void Board::SaveTo(BoardSnapshot& snapshotSalida, int puntajeActual,
                   int movimientosActuales) const
{
    snapshotSalida.filas = ROWS; // guarda filas (8)
    snapshotSalida.columnas = COLS; // guarda columnas (8)
    snapshotSalida.puntaje = puntajeActual; // guarda puntos
    snapshotSalida.movimientos = movimientosActuales; // guarda movimientos
    for (int fila = 0; fila < ROWS; ++fila) // recorre filas
        for (int columna = 0; columna < COLS; ++columna) // recorre columnas
        {
            snapshotSalida.colores[fila][columna] =
                _coloresTablero->Get(fila, columna); // copia color
            snapshotSalida.bombas[fila][columna] =
                _bombasTablero->Get(fila, columna); // copia bomba
        }
}

// Carga una foto (deshacer movimiento).
void Board::LoadFrom(const BoardSnapshot& snapshotOrigen)
{
    for (int fila = 0; fila < ROWS; ++fila) // recorre filas
        for (int columna = 0; columna < COLS; ++columna) // recorre columnas
        {
            _coloresTablero->Set(fila, columna,
                snapshotOrigen.colores[fila][columna]); // pega color guardado
            _bombasTablero->Set(fila, columna,
                snapshotOrigen.bombas[fila][columna]); // pega bomba guardada
        }
}
