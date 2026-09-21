#pragma once // incluir una sola vez
#include <iostream> // solo avisos en consola

// Grid = rejilla 2D de numeros (el tablero del juego).
// Guarda _celdasTablero[fila][columna] con memoria dinamica.
// FloodFill cuenta la mancha del mismo color (4 vecinos, sin diagonal).
// Tiene 3 frenos: 1) fuera del tablero, 2) otro color, 3) ya visitada.
// La bomba USA este FloodFill. Los matches NO (usan escaneo de lineas).
// Sin SDL aqui: logica pura.

template <class T> // T = int (colores) o bool (bombas)
class Grid
{
private:
    T** _celdasTablero; // matriz de valores [fila][columna]
    bool** _casillasVisitadas; // matriz que marca ya contadas en Flood
    int _totalFilas; // cuantas filas
    int _totalColumnas; // cuantas columnas

    int FloodRecursivo(int fila, int columna, T colorObjetivo); // cuenta vecinos iguales

public:
    Grid(int totalFilas, int totalColumnas); // crea las dos matrices
    ~Grid(); // libera las dos matrices

    void Set(int fila, int columna, T valorNuevo); // escribe un valor
    T Get(int fila, int columna); // lee un valor
    int GetFilas(); // cuantas filas
    int GetColumnas(); // cuantas columnas
    int FloodFill(int fila, int columna); // cuenta la mancha desde aqui
    void Print(); // imprime la rejilla (debug)

    Grid(const Grid&) = delete; // no copiar (evita doble-free)
    Grid& operator=(const Grid&) = delete; // no asignar
};

// Crea matriz de valores + matriz de visitadas, todo en ceros.
template <class T>
Grid<T>::Grid(int totalFilas, int totalColumnas)
{
    _celdasTablero = nullptr; // aun nada
    _casillasVisitadas = nullptr; // aun nada
    _totalFilas = totalFilas; // guarda filas
    _totalColumnas = totalColumnas; // guarda columnas

    _celdasTablero = new T*[_totalFilas]; // arreglo de filas (punteros)
    _casillasVisitadas = new bool*[_totalFilas]; // arreglo de filas (punteros)
    for (int fila = 0; fila < _totalFilas; ++fila) // por cada fila...
    {
        _celdasTablero[fila] = new T[_totalColumnas]; // ...crea sus columnas
        _casillasVisitadas[fila] = new bool[_totalColumnas]; // ...crea sus marcas
        for (int columna = 0; columna < _totalColumnas; ++columna) // por cada columna...
        {
            _celdasTablero[fila][columna] = T(); // valor vacio (0 o false)
            _casillasVisitadas[fila][columna] = false; // no visitada
        }
    }
}

// Libera primero cada fila y luego el arreglo de filas (orden inverso).
template <class T>
Grid<T>::~Grid()
{
    if (_celdasTablero != nullptr) // si hay matriz de valores...
    {
        for (int fila = 0; fila < _totalFilas; ++fila) // por cada fila...
            delete[] _celdasTablero[fila]; // ...libera sus columnas
        delete[] _celdasTablero; // libera el arreglo de filas
        _celdasTablero = nullptr; // queda en nada
    }
    if (_casillasVisitadas != nullptr) // si hay matriz de marcas...
    {
        for (int fila = 0; fila < _totalFilas; ++fila) // por cada fila...
            delete[] _casillasVisitadas[fila]; // ...libera sus marcas
        delete[] _casillasVisitadas; // libera el arreglo de filas
        _casillasVisitadas = nullptr; // queda en nada
    }
}

// Escribe un valor. Si esta fuera, avisa y no hace nada.
template <class T>
void Grid<T>::Set(int fila, int columna, T valorNuevo)
{
    if (fila < 0 || fila >= _totalFilas || columna < 0 || columna >= _totalColumnas) // fuera?
    {
        std::cerr << "Grid::Set: coordenada fuera de rango" << std::endl; // avisa
        return; // no escribe
    }
    _celdasTablero[fila][columna] = valorNuevo; // guarda el valor
}

// Lee un valor. Si esta fuera, avisa y regresa vacio.
template <class T>
T Grid<T>::Get(int fila, int columna)
{
    if (fila < 0 || fila >= _totalFilas || columna < 0 || columna >= _totalColumnas) // fuera?
    {
        std::cerr << "Grid::Get: coordenada fuera de rango" << std::endl; // avisa
        return T(); // valor vacio
    }
    return _celdasTablero[fila][columna]; // regresa el valor
}

// Cuantas filas tiene.
template <class T>
int Grid<T>::GetFilas()
{
    return _totalFilas; // regresa filas
}

// Cuantas columnas tiene.
template <class T>
int Grid<T>::GetColumnas()
{
    return _totalColumnas; // regresa columnas
}

// Cuenta la mancha del mismo color desde (fila,columna). Limpia marcas antes.
template <class T>
int Grid<T>::FloodFill(int fila, int columna)
{
    if (fila < 0 || fila >= _totalFilas || columna < 0 || columna >= _totalColumnas) // fuera?
    {
        std::cerr << "Grid::FloodFill: coordenada fuera de rango" << std::endl; // avisa
        return 0; // cero
    }
    for (int filaActual = 0; filaActual < _totalFilas; ++filaActual) // limpia marcas...
        for (int columnaActual = 0; columnaActual < _totalColumnas; ++columnaActual) // ...una por una
            _casillasVisitadas[filaActual][columnaActual] = false; // todas en no visitada

    T colorObjetivo = _celdasTablero[fila][columna]; // color a buscar (el de inicio)
    return FloodRecursivo(fila, columna, colorObjetivo); // cuenta desde aqui
}

// Cuenta vecinos iguales con recursion. 3 frenos: fuera, visitada, otro color.
template <class T>
int Grid<T>::FloodRecursivo(int fila, int columna, T colorObjetivo)
{
    if (fila < 0 || fila >= _totalFilas || columna < 0 || columna >= _totalColumnas) // fuera?
        return 0; // freno 1: no cuenta
    if (_casillasVisitadas[fila][columna]) // ya contada?
        return 0; // freno 2: no cuenta doble
    if (_celdasTablero[fila][columna] != colorObjetivo) // otro color?
        return 0; // freno 3: no cuenta

    _casillasVisitadas[fila][columna] = true; // marca como contada

    int totalConectadas = 1; // esta casilla vale 1
    totalConectadas += FloodRecursivo(fila + 1, columna, colorObjetivo); // suma la de abajo
    totalConectadas += FloodRecursivo(fila - 1, columna, colorObjetivo); // suma la de arriba
    totalConectadas += FloodRecursivo(fila, columna + 1, colorObjetivo); // suma la derecha
    totalConectadas += FloodRecursivo(fila, columna - 1, colorObjetivo); // suma la izquierda
    return totalConectadas; // total de la mancha
}

// Imprime la rejilla con numeros de fila y columna (debug).
template <class T>
void Grid<T>::Print()
{
    std::cout << "    "; // esquina vacia
    for (int columna = 0; columna < _totalColumnas; ++columna) // titulos de columna
        std::cout << columna << "\t"; // numero + tab
    std::cout << std::endl; // salto de linea
    for (int fila = 0; fila < _totalFilas; ++fila) // por cada fila...
    {
        std::cout << fila << " | "; // numero de fila
        for (int columna = 0; columna < _totalColumnas; ++columna) // por cada columna...
        {
            std::cout << _celdasTablero[fila][columna] << "\t"; // valor + tab
        }
        std::cout << std::endl; // salto de linea
    }
}
