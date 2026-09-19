#pragma once
// =====================================================================
//  MatchLogic.h - Lógica pura del Match-3 (CERO SDL, CERO std::vector).
//  Aqui vive el algoritmo CORRECTO de deteccion: escaneo de lineas rectas.
//  Flood Fill NO se usa aqui (ver nota en Board/MatchLogic.cpp).
//  Opera sobre Grid<int> de colores para no acoplar Board <-> MatchLogic.
// =====================================================================

class GridIntFwd;
struct SwapHint
{
    int r1, c1, r2, c2;
    bool valid;
};

namespace MatchLogic
{
    // Avanza la semilla LCG (determinista, sin rand() global).
    unsigned int LcgNext(unsigned int& seed);

    // Escanea lineas horizontales + verticales buscando corridas >= 3.
    // Marca outMark (tamanio rows*cols, index = r*cols+c) y regresa
    // cuantas celdas distintas quedaron marcadas (L/T no se cuentan doble).
    // Complejidad: O(rows*cols). Itera cada celda 2 veces como maximo.
    // outMark debe venir en false; EMPTY (-1) nunca matchea.
    template <class GridT>
    int ScanMatches(GridT* colors, int rows, int cols, bool* outMark);

    // Prueba un swap temporal y dice si generaria match. O(rows*cols).
    template <class GridT>
    bool WouldMatchAfterSwap(GridT* colors, int rows, int cols,
                             int r1, int c1, int r2, int c2);

    // Busca el primer swap adyacente que genere match (pista / anti-bloqueo).
    // Solo prueba derecha y abajo por par para no duplicar. O((rows*cols)^2)
    // en el peor caso, con tablero 8x8 = ~4k ops: despreciable a 60 FPS
    // porque solo se llama tras cada turno o cada 5s idle, no por frame.
    template <class GridT>
    bool FindAnyHint(GridT* colors, int rows, int cols, SwapHint& out);
}

// ---- Implementacion template (header-only para no complicar el vcxproj) ----

template <class GridT>
int MatchLogic::ScanMatches(GridT* colors, int rows, int cols, bool* outMark)
{
    for (int i = 0; i < rows * cols; ++i) outMark[i] = false;

    // Horizontales: corridas por renglon.
    for (int r = 0; r < rows; ++r)
    {
        int runStart = 0;
        for (int c = 1; c <= cols; ++c)
        {
            int cur = (c < cols) ? colors->Get(r, c) : -9999;
            int startVal = colors->Get(r, runStart);
            bool same = (c < cols) && (cur == startVal) && (startVal != -1);
            if (same) continue;
            int runLen = c - runStart;
            if (startVal != -1 && runLen >= 3)
            {
                for (int k = runStart; k < c; ++k)
                    outMark[r * cols + k] = true;
            }
            runStart = c;
        }
    }
    // Verticales: corridas por columna.
    for (int c = 0; c < cols; ++c)
    {
        int runStart = 0;
        for (int r = 1; r <= rows; ++r)
        {
            int cur = (r < rows) ? colors->Get(r, c) : -9999;
            int startVal = colors->Get(runStart, c);
            bool same = (r < rows) && (cur == startVal) && (startVal != -1);
            if (same) continue;
            int runLen = r - runStart;
            if (startVal != -1 && runLen >= 3)
            {
                for (int k = runStart; k < r; ++k)
                    outMark[k * cols + c] = true;
            }
            runStart = r;
        }
    }
    int count = 0;
    for (int i = 0; i < rows * cols; ++i)
        if (outMark[i]) ++count;
    return count;
}

template <class GridT>
bool MatchLogic::WouldMatchAfterSwap(GridT* colors, int rows, int cols,
                                     int r1, int c1, int r2, int c2)
{
    if (r1 < 0 || r1 >= rows || c1 < 0 || c1 >= cols) return false;
    if (r2 < 0 || r2 >= rows || c2 < 0 || c2 >= cols) return false;
    int dr = r1 - r2; if (dr < 0) dr = -dr;
    int dc = c1 - c2; if (dc < 0) dc = -dc;
    if (dr + dc != 1) return false;

    int a = colors->Get(r1, c1);
    int b = colors->Get(r2, c2);
    if (a == -1 || b == -1) return false;

    colors->Set(r1, c1, b);
    colors->Set(r2, c2, a);

    // Escaneo rapido con salida temprana: basta saber si hay >= 1 corrida.
    bool found = false;
    // Checar solo filas/columnas afectadas (r1,r2,c1,c2) en vez de todo.
    // Filas r1 y r2:
    int rowsChk[2] = { r1, r2 };
    for (int i = 0; i < 2 && !found; ++i)
    {
        int r = rowsChk[i];
        int run = 1;
        for (int c = 1; c < cols; ++c)
        {
            int v0 = colors->Get(r, c);
            int v1 = colors->Get(r, c - 1);
            if (v0 != -1 && v0 == v1) { run++; if (run >= 3) { found = true; break; } }
            else run = 1;
        }
    }
    // Columnas c1 y c2:
    int colsChk[2] = { c1, c2 };
    for (int i = 0; i < 2 && !found; ++i)
    {
        int c = colsChk[i];
        int run = 1;
        for (int r = 1; r < rows; ++r)
        {
            int v0 = colors->Get(r, c);
            int v1 = colors->Get(r - 1, c);
            if (v0 != -1 && v0 == v1) { run++; if (run >= 3) { found = true; break; } }
            else run = 1;
        }
    }

    colors->Set(r1, c1, a);
    colors->Set(r2, c2, b);
    return found;
}

template <class GridT>
bool MatchLogic::FindAnyHint(GridT* colors, int rows, int cols, SwapHint& out)
{
    out.valid = false;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (colors->Get(r, c) == -1) continue;
            // Solo derecha y abajo (cada par una vez).
            const int dr[2] = { 0, 1 };
            const int dc[2] = { 1, 0 };
            for (int k = 0; k < 2; ++k)
            {
                int r2 = r + dr[k], c2 = c + dc[k];
                if (r2 < 0 || r2 >= rows || c2 < 0 || c2 >= cols) continue;
                if (colors->Get(r2, c2) == -1) continue;
                if (WouldMatchAfterSwap(colors, rows, cols, r, c, r2, c2))
                {
                    out.r1 = r; out.c1 = c; out.r2 = r2; out.c2 = c2;
                    out.valid = true;
                    return true;
                }
            }
        }
    }
    return false;
}

inline unsigned int MatchLogic::LcgNext(unsigned int& seed)
{
    seed = seed * 1664525u + 1013904223u;
    return (seed >> 16) & 0x7FFFu;
}
