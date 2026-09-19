#pragma once
#include "../MyLib/Grid.h"
#include "../MyLib/LinkedQueue.h"
#include "MatchLogic.h"

// =====================================================================
//  Board.h - Tablero logico del Match-3 (CERO SDL).
//  Guarda numeros, no sprites: Grid<int> colores (0..TYPES-1, -1 = vacio)
//  y Grid<bool> bomba (true = gema explosiva de ese color).
//  Toda la aleatoriedad usa semilla LCG propia (sin rand global) para que
//  la logica sea testeable y determinista en la defensa.
// =====================================================================

struct CellPos
{
    int r, c;
    CellPos() : r(-1), c(-1) {}
    CellPos(int rr, int cc) : r(rr), c(cc) {}
    bool operator==(const CellPos& o) const { return r == o.r && c == o.c; }
};

struct BoardSnapshot
{
    static const int MAX = 8;
    int colors[MAX][MAX];
    bool bombs[MAX][MAX];
    int score;
    int moves;
    int rows, cols;
    BoardSnapshot()
    {
        for (int r = 0; r < MAX; ++r)
            for (int c = 0; c < MAX; ++c) { colors[r][c] = -1; bombs[r][c] = false; }
        score = 0; moves = 0; rows = 8; cols = 8;
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
    Grid<int>* _colors;
    Grid<bool>* _bombs;
    unsigned int _seed;

    int RandType();
    // Recolector recursivo gemelo de Grid::FloodRecursivo, pero que ADEMAS
    // junta las celdas en mark[][] (para destruir la mancha de la bomba).
    int CollectRec(int r, int c, int target, bool* visitedFlat, bool* markFlat);

public:
    explicit Board(unsigned int seed);
    ~Board();

    int Rows() const { return ROWS; }
    int Cols() const { return COLS; }
    int Types() const { return TYPES; }

    void SetSeed(unsigned int s) { _seed = s ? s : 12345u; }
    bool InBounds(int r, int c) const { return r >= 0 && r < ROWS && c >= 0 && c < COLS; }

    int Get(int r, int c);
    bool IsBomb(int r, int c);
    bool IsEmptyCell(int r, int c);
    void SetCell(int r, int c, int color, bool bomb);
    void SwapCells(int r1, int c1, int r2, int c2);

    // Llena sin matches iniciales (para inicio y shuffle garantizado).
    void RandomFillNoMatch();
    // Mezcla hasta lograr: sin matches directos Y con al menos 1 movimiento.
    // (Reto Shuffle Inteligente. Max 200 intentos; determinista por semilla.)
    void ShuffleNoMatch();

    // Escaneo de lineas -> marca celdas matched. Regresa conteo (sin doble).
    int FindMatches(bool markOut[ROWS][COLS]);
    // Hay al menos un movimiento valido? (para anti-bloqueo)
    bool HasPossibleMove();
    // Primer movimiento valido (para Hint). false si bloqueado.
    bool FindHint(int& r1, int& c1, int& r2, int& c2);

    // GEMA EXPLOSIVA (usa tu Flood Fill):
    // 1) _colors->FloodFill(r,c) cuenta la mancha (tu algoritmo, obligatorio).
    // 2) CollectRec junta las celdas de la mancha en markOut.
    // Regresa cuantas celdas se destruirian. No destruye aqui.
    int DetonatePreview(int r, int c, bool markOut[ROWS][COLS]);

    // Gravedad logica: compacta cada columna hacia abajo y rellena arriba
    // con aleatorios. Las celdas EMPTY (-1) desaparecen. O(rows*cols).
    void ApplyGravity();

    void SaveTo(BoardSnapshot& out, int score, int moves) const;
    void LoadFrom(const BoardSnapshot& snap);

    // Acceso crudo para MatchLogic/tests (solo lectura/escritura puntual).
    Grid<int>* Colors() { return _colors; }

    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
};
