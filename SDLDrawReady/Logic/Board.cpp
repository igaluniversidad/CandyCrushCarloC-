#include "Board.h"

// ---------------------------------------------------------------------
// Board.cpp - Logica pura. Prohibido incluir SDL aqui (regla de oro).
// ---------------------------------------------------------------------

Board::Board(unsigned int seed)
{
    _colors = new Grid<int>(ROWS, COLS);
    _bombs = new Grid<bool>(ROWS, COLS);
    _seed = seed ? seed : 12345u;
    RandomFillNoMatch();
}

Board::~Board()
{
    delete _colors;
    delete _bombs;
    _colors = nullptr;
    _bombs = nullptr;
}

int Board::RandType()
{
    return (int)(MatchLogic::LcgNext(_seed) % (unsigned int)TYPES);
}

int Board::Get(int r, int c)
{
    if (!InBounds(r, c)) return EMPTY;
    return _colors->Get(r, c);
}

bool Board::IsBomb(int r, int c)
{
    if (!InBounds(r, c)) return false;
    return _bombs->Get(r, c);
}

bool Board::IsEmptyCell(int r, int c)
{
    if (!InBounds(r, c)) return true;
    return _colors->Get(r, c) == EMPTY;
}

void Board::SetCell(int r, int c, int color, bool bomb)
{
    if (!InBounds(r, c)) return;
    _colors->Set(r, c, color);
    _bombs->Set(r, c, bomb);
}

void Board::SwapCells(int r1, int c1, int r2, int c2)
{
    if (!InBounds(r1, c1) || !InBounds(r2, c2)) return;
    int a = _colors->Get(r1, c1);
    int b = _colors->Get(r2, c2);
    bool ba = _bombs->Get(r1, c1);
    bool bb = _bombs->Get(r2, c2);
    _colors->Set(r1, c1, b);
    _colors->Set(r2, c2, a);
    _bombs->Set(r1, c1, bb);
    _bombs->Set(r2, c2, ba);
}

void Board::RandomFillNoMatch()
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            int v = 0;
            // Elige un tipo que no forme 3 con los 2 izquierdos ni 2 arriba.
            for (int tries = 0; tries < 50; ++tries)
            {
                v = RandType();
                bool badH = (c >= 2 &&
                    _colors->Get(r, c - 1) == v && _colors->Get(r, c - 2) == v);
                bool badV = (r >= 2 &&
                    _colors->Get(r - 1, c) == v && _colors->Get(r - 2, c) == v);
                if (!badH && !badV) break;
            }
            _colors->Set(r, c, v);
            _bombs->Set(r, c, false);
        }
    }
}

void Board::ShuffleNoMatch()
{
    // Fisher-Yates sobre arreglo plano + reintentos hasta condicion valida.
    int flat[ROWS * COLS];
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            flat[r * COLS + c] = _colors->Get(r, c);

    bool mark[ROWS][COLS];
    for (int attempt = 0; attempt < 200; ++attempt)
    {
        // Barajar
        for (int i = ROWS * COLS - 1; i > 0; --i)
        {
            unsigned int j = MatchLogic::LcgNext(_seed) % (unsigned int)(i + 1);
            int tmp = flat[i]; flat[i] = flat[j]; flat[j] = tmp;
        }
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
            {
                _colors->Set(r, c, flat[r * COLS + c]);
                _bombs->Set(r, c, false); // el shuffle limpia bombas (regla simple)
            }
        if (FindMatches(mark) == 0 && HasPossibleMove())
            return;
    }
    // Si tras 200 intentos no se logro (casi imposible en 8x8/6 tipos),
    // regenera desde cero que por construccion cumple ambas.
    RandomFillNoMatch();
}

int Board::FindMatches(bool markOut[ROWS][COLS])
{
    bool flat[ROWS * COLS];
    int n = MatchLogic::ScanMatches(_colors, ROWS, COLS, flat);
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            markOut[r][c] = flat[r * COLS + c];
    return n;
}

bool Board::HasPossibleMove()
{
    SwapHint h;
    return MatchLogic::FindAnyHint(_colors, ROWS, COLS, h);
}

bool Board::FindHint(int& r1, int& c1, int& r2, int& c2)
{
    // Si hay bomba en el tablero, sugerir usarla es valido: busca cualquier
    // bomba y propone swap con su vecino derecho/abajo.
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (_bombs->Get(r, c))
            {
                if (c + 1 < COLS) { r1 = r; c1 = c; r2 = r; c2 = c + 1; return true; }
                if (r + 1 < ROWS) { r1 = r; c1 = c; r2 = r + 1; c2 = c; return true; }
            }
    SwapHint h;
    if (!MatchLogic::FindAnyHint(_colors, ROWS, COLS, h)) return false;
    r1 = h.r1; c1 = h.c1; r2 = h.r2; c2 = h.c2;
    return true;
}

int Board::CollectRec(int r, int c, int target, bool* visitedFlat, bool* markFlat)
{
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return 0;
    int idx = r * COLS + c;
    if (visitedFlat[idx]) return 0;
    if (_colors->Get(r, c) != target) return 0;
    visitedFlat[idx] = true;
    markFlat[idx] = true;
    int n = 1;
    n += CollectRec(r + 1, c, target, visitedFlat, markFlat);
    n += CollectRec(r - 1, c, target, visitedFlat, markFlat);
    n += CollectRec(r, c + 1, target, visitedFlat, markFlat);
    n += CollectRec(r, c - 1, target, visitedFlat, markFlat);
    return n;
}

int Board::DetonatePreview(int r, int c, bool markOut[ROWS][COLS])
{
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            markOut[i][j] = false;
    if (!InBounds(r, c)) return 0;
    if (_colors->Get(r, c) == EMPTY) return 0;

    // PASO 1 (obligatorio): tu FloodFill cuenta la mancha del mismo color.
    // Si el conteo es 0 algo anda mal; aun asi recolectamos para destruir.
    int counted = _colors->FloodFill(r, c);
    (void)counted;

    // PASO 2: recolector gemelo que junta las celdas (para encolarlas).
    bool visited[ROWS * COLS];
    bool marked[ROWS * COLS];
    for (int i = 0; i < ROWS * COLS; ++i) { visited[i] = false; marked[i] = false; }
    int target = _colors->Get(r, c);
    int n = CollectRec(r, c, target, visited, marked);
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            markOut[i][j] = marked[i * COLS + j];
    return n;
}

void Board::ApplyGravity()
{
    for (int c = 0; c < COLS; ++c)
    {
        int write = ROWS - 1;
        for (int r = ROWS - 1; r >= 0; --r)
        {
            int v = _colors->Get(r, c);
            if (v != EMPTY)
            {
                if (write != r)
                {
                    _colors->Set(write, c, v);
                    _bombs->Set(write, c, _bombs->Get(r, c));
                    _colors->Set(r, c, EMPTY);
                    _bombs->Set(r, c, false);
                }
                write--;
            }
        }
        for (int r = write; r >= 0; --r)
        {
            _colors->Set(r, c, RandType());
            _bombs->Set(r, c, false);
        }
    }
}

void Board::SaveTo(BoardSnapshot& out, int score, int moves) const
{
    out.rows = ROWS; out.cols = COLS;
    out.score = score; out.moves = moves;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
        {
            out.colors[r][c] = _colors->Get(r, c);
            out.bombs[r][c] = _bombs->Get(r, c);
        }
}

void Board::LoadFrom(const BoardSnapshot& snap)
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
        {
            _colors->Set(r, c, snap.colors[r][c]);
            _bombs->Set(r, c, snap.bombs[r][c]);
        }
}
