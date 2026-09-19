#pragma once
#include <iostream>

// =====================================================================
//  Grid<T> - Rejilla 2D con memoria dinamica encapsulada.
//  De tu libreria EstructurasDeDatos26-3, vendorizado sin logica cambiada
//  salvo: ConsoleUI -> std::cerr (cero dependencias extra).
//  Es literalmente el tablero del Match-3: _celdas[fila][columna].
//  FLOOD FILL recursivo (4 vecinos, sin diagonales) con 3 casos base:
//    1) fuera de limites, 2) valor distinto, 3) ya visitado.
//  La gema explosiva USA este FloodFill (cuenta la mancha de su color).
//  La deteccion de matches NO usa flood: usa escaneo de lineas rectas
//  (ver Logic/MatchLogic). Esa distincion es la leccion del proyecto.
//  PROHIBIDO #include <SDL.h> aqui: logica pura.
// =====================================================================

template <class T>
class Grid
{
private:
    T** _celdas;
    bool** _visitadas;
    int _filas;
    int _columnas;

    int FloodRecursivo(int fila, int columna, T objetivo);

public:
    Grid(int filas, int columnas);
    ~Grid();

    void Set(int fila, int columna, T valor);
    T Get(int fila, int columna);
    int GetFilas();
    int GetColumnas();
    int FloodFill(int fila, int columna);
    void Print();

    // Sin copia: el tablero es unico por partida (evita doble-free).
    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;
};

template <class T>
Grid<T>::Grid(int filas, int columnas)
{
    _celdas = nullptr;
    _visitadas = nullptr;
    _filas = filas;
    _columnas = columnas;

    _celdas = new T*[_filas];
    _visitadas = new bool*[_filas];
    for (int i = 0; i < _filas; ++i)
    {
        _celdas[i] = new T[_columnas];
        _visitadas[i] = new bool[_columnas];
        for (int j = 0; j < _columnas; ++j)
        {
            _celdas[i][j] = T();
            _visitadas[i][j] = false;
        }
    }
}

template <class T>
Grid<T>::~Grid()
{
    if (_celdas != nullptr)
    {
        for (int i = 0; i < _filas; ++i)
            delete[] _celdas[i];
        delete[] _celdas;
        _celdas = nullptr;
    }
    if (_visitadas != nullptr)
    {
        for (int i = 0; i < _filas; ++i)
            delete[] _visitadas[i];
        delete[] _visitadas;
        _visitadas = nullptr;
    }
}

template <class T>
void Grid<T>::Set(int fila, int columna, T valor)
{
    if (fila < 0 || fila >= _filas || columna < 0 || columna >= _columnas)
    {
        std::cerr << "Grid::Set: coordenada fuera de rango" << std::endl;
        return;
    }
    _celdas[fila][columna] = valor;
}

template <class T>
T Grid<T>::Get(int fila, int columna)
{
    if (fila < 0 || fila >= _filas || columna < 0 || columna >= _columnas)
    {
        std::cerr << "Grid::Get: coordenada fuera de rango" << std::endl;
        return T();
    }
    return _celdas[fila][columna];
}

template <class T>
int Grid<T>::GetFilas()
{
    return _filas;
}

template <class T>
int Grid<T>::GetColumnas()
{
    return _columnas;
}

template <class T>
int Grid<T>::FloodFill(int fila, int columna)
{
    if (fila < 0 || fila >= _filas || columna < 0 || columna >= _columnas)
    {
        std::cerr << "Grid::FloodFill: coordenada fuera de rango" << std::endl;
        return 0;
    }
    for (int i = 0; i < _filas; ++i)
        for (int j = 0; j < _columnas; ++j)
            _visitadas[i][j] = false;

    T objetivo = _celdas[fila][columna];
    return FloodRecursivo(fila, columna, objetivo);
}

template <class T>
int Grid<T>::FloodRecursivo(int fila, int columna, T objetivo)
{
    if (fila < 0 || fila >= _filas || columna < 0 || columna >= _columnas)
        return 0;
    if (_visitadas[fila][columna])
        return 0;
    if (_celdas[fila][columna] != objetivo)
        return 0;

    _visitadas[fila][columna] = true;

    int count = 1;
    count += FloodRecursivo(fila + 1, columna, objetivo);
    count += FloodRecursivo(fila - 1, columna, objetivo);
    count += FloodRecursivo(fila, columna + 1, objetivo);
    count += FloodRecursivo(fila, columna - 1, objetivo);
    return count;
}

template <class T>
void Grid<T>::Print()
{
    std::cout << "    ";
    for (int c = 0; c < _columnas; ++c)
        std::cout << c << "\t";
    std::cout << std::endl;
    for (int f = 0; f < _filas; ++f)
    {
        std::cout << f << " | ";
        for (int c = 0; c < _columnas; ++c)
        {
            std::cout << _celdas[f][c] << "\t";
        }
        std::cout << std::endl;
    }
}
