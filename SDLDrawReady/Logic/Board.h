#pragma once
#include "../MyLib/Grid.h"
#include "../MyLib/LinkedQueue.h"
#include "MatchLogic.h"

// =====================================================================
//  Board.h - Tablero LOGICO del Match-3 (CERO SDL, solo numeros).
//  Guarda dos rejillas del mismo tamano:
//    _coloresTablero : Grid<int>  (0..TYPES-1 = color, EMPTY(-1) = vacio)
//    _bombasTablero  : Grid<bool> (true = esa celda es gema explosiva)
//  La aleatoriedad usa semilla LCG propia (_semillaAleatoria), sin rand()
//  global, para que la logica sea determinista y testeable en defensa.
// =====================================================================

// Posicion de una celda: fila + columna (0..7 en tablero 8x8).
struct CellPos
{
    int fila;
    int columna;
    CellPos() : fila(-1), columna(-1) {}
    CellPos(int filaInicial, int columnaInicial)
        : fila(filaInicial), columna(columnaInicial) {}
    bool operator==(const CellPos& otraPosicion) const
    {
        return fila == otraPosicion.fila && columna == otraPosicion.columna;
    }
};

// Foto completa del tablero para el Undo (RingBuffer guarda 4 de estas).
struct BoardSnapshot
{
    static const int MAX = 8;
    int colores[MAX][MAX];      // color por celda (-1 = vacia)
    bool bombas[MAX][MAX];      // true = bomba en esa celda
    int puntaje;                // score al momento de la foto
    int movimientos;            // movimientos restantes al momento de la foto
    int filas;
    int columnas;
    BoardSnapshot()
    {
        for (int fila = 0; fila < MAX; ++fila)
            for (int columna = 0; columna < MAX; ++columna)
            {
                colores[fila][columna] = -1;
                bombas[fila][columna] = false;
            }
        puntaje = 0;
        movimientos = 0;
        filas = 8;
        columnas = 8;
    }
};

class Board
{
public:
    static const int ROWS = 8;
    static const int COLS = 8;
    static const int TYPES = 6;
    static const int EMPTY = -1;

private:
    Grid<int>* _coloresTablero;
    Grid<bool>* _bombasTablero;
    unsigned int _semillaAleatoria;

    int RandType();
    // Recolector recursivo gemelo de Grid::FloodRecursivo, pero que ADEMAS
    // junta las celdas en el arreglo de marcas (para destruir la mancha).
    // fila/columna = celda actual. colorObjetivo = color de la bomba.
    // visitadosPlano = celdas ya revisadas. marcasPlano = celdas a destruir.
    int CollectRec(int fila, int columna, int colorObjetivo,
                   bool* visitadosPlano, bool* marcasPlano);

public:
    explicit Board(unsigned int semillaInicial);
    ~Board();

    int Rows() const { return ROWS; }
    int Cols() const { return COLS; }
    int Types() const { return TYPES; }

    void SetSeed(unsigned int nuevaSemilla)
    {
        _semillaAleatoria = nuevaSemilla ? nuevaSemilla : 12345u;
    }
    bool InBounds(int fila, int columna) const
    {
        return fila >= 0 && fila < ROWS && columna >= 0 && columna < COLS;
    }

    int Get(int fila, int columna);
    bool IsBomb(int fila, int columna);
    bool IsEmptyCell(int fila, int columna);
    void SetCell(int fila, int columna, int color, bool esBomba);
    void SwapCells(int filaOrigen, int columnaOrigen,
                   int filaDestino, int columnaDestino);

    // Llena sin matches iniciales (para inicio y shuffle garantizado).
    void RandomFillNoMatch();
    // Mezcla hasta lograr: sin matches directos Y con al menos 1 movimiento.
    // (Reto Shuffle Inteligente. Max 200 intentos; determinista por semilla.)
    void ShuffleNoMatch();

    // Escaneo de lineas -> marca celdas matched. Regresa conteo (sin doble).
    // marcaDestruccion[fila][columna] = true si esa celda es parte de un match.
    int FindMatches(bool marcaDestruccion[ROWS][COLS]);
    // Hay al menos un movimiento valido? (para anti-bloqueo)
    bool HasPossibleMove();
    // Primer movimiento valido (para Hint). false si bloqueado.
    // Devuelve dos celdas vecinas que al intercambiarse forman match.
    bool FindHint(int& filaOrigen, int& columnaOrigen,
                  int& filaDestino, int& columnaDestino);

    // GEMA EXPLOSIVA (usa tu Flood Fill):
    // 1) _coloresTablero->FloodFill cuenta la mancha (obligatorio).
    // 2) CollectRec junta las celdas de la mancha en marcaDestruccion.
    // Regresa cuantas celdas se destruirian. No destruye aqui.
    int DetonatePreview(int filaBomba, int columnaBomba,
                        bool marcaDestruccion[ROWS][COLS]);

    // Gravedad logica: compacta cada columna hacia abajo y rellena arriba
    // con aleatorios. Las celdas EMPTY (-1) desaparecen. O(filas*columnas).
    void ApplyGravity();

    void SaveTo(BoardSnapshot& snapshotSalida, int puntajeActual,
                int movimientosActuales) const;
    void LoadFrom(const BoardSnapshot& snapshotOrigen);

    // Acceso crudo para MatchLogic/tests (solo lectura/escritura puntual).
    Grid<int>* Colors() { return _coloresTablero; }

    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
};
