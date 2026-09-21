#pragma once // este archivo se incluye una sola vez
#include "../MyLib/Grid.h" // rejilla 2D para guardar numeros
#include "../MyLib/LinkedQueue.h" // cola (se usa en Gameplay, no aqui directo)
#include "MatchLogic.h" // escaneo de lineas y aleatorio LCG

// Board.h = tablero LOGICO. Solo numeros, nada de dibujos ni SDL.

// Una casilla del tablero. fila 0..7 de arriba a abajo.
// columna 0..7 de izquierda a derecha.
struct CellPos
{
    int fila; // renglon
    int columna; // columna
    CellPos() : fila(-1), columna(-1) {} // casilla vacia = -1,-1
    // Crea una casilla con su fila y columna iniciales.
    CellPos(int filaInicial, int columnaInicial)
        : fila(filaInicial), columna(columnaInicial) {}
    // Dos casillas son iguales si tienen misma fila y misma columna.
    bool operator==(const CellPos& otraPosicion) const
    {
        return fila == otraPosicion.fila && columna == otraPosicion.columna;
    }
};

// Foto del tablero para el Undo. El RingBuffer guarda 4 fotos.
struct BoardSnapshot
{
    static const int MAX = 8; // el tablero siempre es 8x8
    int colores[MAX][MAX]; // color de cada casilla (-1 = vacia)
    bool bombas[MAX][MAX]; // true = aqui hay bomba
    int puntaje; // puntos que tenias en la foto
    int movimientos; // movimientos que te quedaban en la foto
    int filas; // siempre 8
    int columnas; // siempre 8
    BoardSnapshot() // foto nueva: todo vacio y ceros
    {
        for (int fila = 0; fila < MAX; ++fila) // recorre filas
            for (int columna = 0; columna < MAX; ++columna) // recorre columnas
            {
                colores[fila][columna] = -1; // vacia
                bombas[fila][columna] = false; // sin bomba
            }
        puntaje = 0; // empieza en 0
        movimientos = 0; // se llena al guardar
        filas = 8; // fijo
        columnas = 8; // fijo
    }
};

class Board // el tablero y todas sus reglas
{
public:
    static const int ROWS = 8; // filas del tablero
    static const int COLS = 8; // columnas del tablero
    static const int TYPES = 6; // 6 colores de dulce (0..5)
    static const int EMPTY = -1; // -1 significa casilla vacia

private:
    Grid<int>* _coloresTablero; // rejilla con el color de cada casilla
    Grid<bool>* _bombasTablero; // rejilla que dice donde hay bomba
    unsigned int _semillaAleatoria; // semilla para dar dulces al azar

    int RandType(); // da un color al azar 0..5
    // Busca toda la mancha del mismo color (arriba/abajo/izq/der).
    // fila/columna = por donde empieza. colorObjetivo = color a buscar.
    // visitadosPlano = ya revisadas. marcasPlano = las que se van a romper.
    int CollectRec(int fila, int columna, int colorObjetivo,
                   bool* visitadosPlano, bool* marcasPlano);

public:
    explicit Board(unsigned int semillaInicial); // crea el tablero y lo llena
    ~Board(); // borra las dos rejillas

    int Rows() const { return ROWS; } // cuantas filas (8)
    int Cols() const { return COLS; } // cuantas columnas (8)
    int Types() const { return TYPES; } // cuantos colores (6)

    // Cambia la semilla. Si es 0 usa 12345.
    void SetSeed(unsigned int nuevaSemilla)
    {
        _semillaAleatoria = nuevaSemilla ? nuevaSemilla : 12345u;
    }
    // true si la casilla esta dentro del tablero 8x8.
    bool InBounds(int fila, int columna) const
    {
        return fila >= 0 && fila < ROWS && columna >= 0 && columna < COLS;
    }

    int Get(int fila, int columna); // que color hay aqui
    bool IsBomb(int fila, int columna); // hay bomba aqui?
    bool IsEmptyCell(int fila, int columna); // esta vacia?
    // Pone color y bomba en una casilla.
    void SetCell(int fila, int columna, int color, bool esBomba);
    // Cambia dos casillas vecinas (color + bomba).
    void SwapCells(int filaOrigen, int columnaOrigen,
                   int filaDestino, int columnaDestino);

    void RandomFillNoMatch(); // llena todo al azar pero sin trios
    void ShuffleNoMatch(); // mezcla si ya no hay jugadas (max 200 intentos)

    // Busca trios en lineas rectas. Marca true donde hay match. Da el total.
    int FindMatches(bool marcaDestruccion[ROWS][COLS]);
    bool HasPossibleMove(); // true si existe al menos una jugada
    // Te dice dos vecinas que si las cambias forman trio. false = bloqueado.
    bool FindHint(int& filaOrigen, int& columnaOrigen,
                  int& filaDestino, int& columnaDestino);

    // Bomba: cuenta la mancha con FloodFill y la marca para romperla.
    // No rompe nada aqui, solo avisa cuantas serian.
    int DetonatePreview(int filaBomba, int columnaBomba,
                        bool marcaDestruccion[ROWS][COLS]);

    void ApplyGravity(); // baja todo lo de arriba y rellena con nuevos

    // Guarda foto (para Undo) y carga foto (para deshacer).
    void SaveTo(BoardSnapshot& snapshotSalida, int puntajeActual,
                int movimientosActuales) const;
    void LoadFrom(const BoardSnapshot& snapshotOrigen);

    Grid<int>* Colors() { return _coloresTablero; } // rejilla para MatchLogic

    Board(const Board&) = delete; // no se puede copiar (evita doble-free)
    Board& operator=(const Board&) = delete; // no se puede asignar
};
