#pragma once // incluir una sola vez

// RingBuffer = ESTRUCTURA PROPIA #2 (no vista en clase).
// Problema: el Undo guarda fotos del tablero. Un Stack sin tope crece
// sin control (200 turnos = 200 fotos en memoria).
// Solucion: anillo de tamano fijo (aqui 4 = base + 3 undos).
// Lleno = borra la mas VIEJA y guarda la nueva (siempre cabe).
// Como cinta circular: _indiceMasViejo da vueltas, _cantidadGuardada dice cuantas hay.
// Push/PopRecent son O(1). Perfecto para Undo con limite de 3.
// Se descarto Stack puro (sin cota), Trie/SparseSet (muy grandes para esto)
// y PriorityQueue (aqui no hay prioridad, solo lo mas reciente).
// Sin SDL. Sin std::vector.

template <class T> // T = BoardSnapshot (foto del tablero)
class RingBuffer
{
private:
    T* _arregloCircular; // cajon fijo donde viven las fotos
    int _capacidadMaxima; // cuantas fotos caben (4)
    int _indiceMasViejo; // donde esta la foto mas vieja
    int _cantidadGuardada; // cuantas fotos validas hay (0..capacidad)

    int PhysIndex(int posicionLogica) const; // logica 0=mas vieja -> fisica real

public:
    explicit RingBuffer(int capacidadDeseada); // aparta el cajon
    ~RingBuffer(); // libera el cajon

    void Push(const T& valorNuevo); // guarda foto (si llena, borra la mas vieja)
    bool PopRecent(T& valorSalida); // saca la mas NUEVA (para Undo). false si vacio.
    bool PeekRecent(T& valorSalida) const; // mira la mas nueva sin sacarla
    bool PeekOldest(T& valorSalida) const; // mira la mas vieja sin sacarla
    bool GetFromRecent(int pasosAtras, T& valorSalida) const; // 0=nueva, 1=anterior...
    int Size() const; // cuantas hay
    int Capacity() const; // cuantas caben
    bool IsEmpty() const; // hay cero?
    bool IsFull() const; // esta lleno?
    void Clear(); // tira todo, queda vacio

    RingBuffer(const RingBuffer&) = delete; // no copiar
    RingBuffer& operator=(const RingBuffer&) = delete; // no asignar
};

// Convierte posicion logica (0 = mas vieja) a posicion fisica real en el cajon.
template <class T>
int RingBuffer<T>::PhysIndex(int posicionLogica) const
{
    return (_indiceMasViejo + posicionLogica) % _capacidadMaxima; // da la vuelta con modulo
}

// Crea el cajon fijo. Empieza vacio: vieja en 0, cero guardadas.
template <class T>
RingBuffer<T>::RingBuffer(int capacidadDeseada)
{
    if (capacidadDeseada <= 0) capacidadDeseada = 1; // minimo 1
    _capacidadMaxima = capacidadDeseada; // guarda el tope
    _arregloCircular = new T[_capacidadMaxima]; // aparta el cajon (un solo new[])
    _indiceMasViejo = 0; // la mas vieja empieza en 0
    _cantidadGuardada = 0; // cero fotos
}

// Libera el cajon.
template <class T>
RingBuffer<T>::~RingBuffer()
{
    delete[] _arregloCircular; // libera todo
    _arregloCircular = nullptr; // queda en nada
}

// Guarda una foto. O(1). Si llena, borra la mas vieja.
template <class T>
void RingBuffer<T>::Push(const T& valorNuevo)
{
    if (_cantidadGuardada < _capacidadMaxima) // aun cabe?
    {
        _arregloCircular[PhysIndex(_cantidadGuardada)] = valorNuevo; // guarda al final logico
        _cantidadGuardada++; // una mas
    }
    else // lleno?
    {
        _arregloCircular[_indiceMasViejo] = valorNuevo; // pisa a la mas vieja
        _indiceMasViejo = (_indiceMasViejo + 1) % _capacidadMaxima; // la siguiente ahora es la vieja
    }
}

// Saca la foto mas NUEVA (la del ultimo turno). Para el Undo. O(1).
template <class T>
bool RingBuffer<T>::PopRecent(T& valorSalida)
{
    if (_cantidadGuardada <= 0) return false; // vacio = no hay
    int indiceFisico = PhysIndex(_cantidadGuardada - 1); // la ultima logica
    valorSalida = _arregloCircular[indiceFisico]; // copia la foto
    _cantidadGuardada--; // una menos
    return true; // si saco
}

// Mira la mas nueva sin sacarla.
template <class T>
bool RingBuffer<T>::PeekRecent(T& valorSalida) const
{
    if (_cantidadGuardada <= 0) return false; // vacio = no hay
    valorSalida = _arregloCircular[PhysIndex(_cantidadGuardada - 1)]; // copia la ultima
    return true; // si hay
}

// Mira la mas vieja sin sacarla.
template <class T>
bool RingBuffer<T>::PeekOldest(T& valorSalida) const
{
    if (_cantidadGuardada <= 0) return false; // vacio = no hay
    valorSalida = _arregloCircular[_indiceMasViejo]; // copia la vieja
    return true; // si hay
}

// Lee mirando hacia atras: 0 = la nueva, 1 = la anterior, etc.
template <class T>
bool RingBuffer<T>::GetFromRecent(int pasosAtras, T& valorSalida) const
{
    if (pasosAtras < 0 || pasosAtras >= _cantidadGuardada) return false; // fuera = no
    valorSalida = _arregloCircular[PhysIndex(_cantidadGuardada - 1 - pasosAtras)]; // copia esa
    return true; // si hay
}

// Cuantas fotos hay guardadas.
template <class T>
int RingBuffer<T>::Size() const { return _cantidadGuardada; } // regresa conteo

// Cuantas fotos caben maximo.
template <class T>
int RingBuffer<T>::Capacity() const { return _capacidadMaxima; } // regresa tope

// True si no hay ninguna foto.
template <class T>
bool RingBuffer<T>::IsEmpty() const { return _cantidadGuardada <= 0; } // vacio = 0

// True si ya no cabe ni una mas.
template <class T>
bool RingBuffer<T>::IsFull() const { return _cantidadGuardada >= _capacidadMaxima; } // lleno = al tope

// Tira todo: vieja en 0, cero guardadas (no libera memoria, la reusa).
template <class T>
void RingBuffer<T>::Clear()
{
    _indiceMasViejo = 0; // reinicia vieja
    _cantidadGuardada = 0; // reinicia conteo
}
