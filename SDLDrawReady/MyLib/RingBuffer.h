#pragma once

// =====================================================================
//  RingBuffer<T> - ESTRUCTURA PROPIA #2 (no vista en clase).
//  Problema que resuelve: Sistema de Deshacer con limite de 3 usos.
//  Guardar snapshots infinitos del tablero (8x8) en un Stack sin cota
//  crece sin control; guardarlos en lista con new por turno fragmenta.
//  El RingBuffer acota la memoria a N snapshots (aqui N=4 para 3 undos
//  + estado base) y sobrescribe el mas viejo al llenarse. Todo O(1).
//
//  Diseno circular:
//    - _buf: arreglo fijo new T[_cap] (una alloc, un free).
//    - _head: indice del mas VIEJO. _count: cuantos hay (0.._cap).
//    - Push: si lleno, sobrescribe en _head y avanza _head (descarta
//      el mas viejo); si no, escribe en (_head+_count)%_cap.
//    - PopRecent: saca el mas NUEVO (LIFO acotado, perfecto para Undo).
//    - PeekRecent/Past: lee sin sacar para mostrar "Undos: X".
//  Por que no un Stack puro? El Stack crece infinito si el jugador hace
//  200 movimientos (200 snapshots de 8x8 en heap). El anillo garantiza
//  cota dura de memoria, que es lo que pide el reto (limite de 3).
//  Alternativas descartadas: Trie/SparseSet (sobre-ingenieria para esto),
//  PriorityQueue con heap (no hay prioridad aqui, solo recencia).
//  Cero SDL, cero std::vector.
// =====================================================================

template <class T>
class RingBuffer
{
private:
    T* _buf;
    int _cap;
    int _head;   // indice del elemento mas viejo
    int _count;  // cuantos elementos validos hay

    int PhysIndex(int logical) const; // logical 0 = mas viejo

public:
    explicit RingBuffer(int capacity);
    ~RingBuffer();

    void Push(const T& value); // O(1), sobrescribe al mas viejo si llena
    bool PopRecent(T& out);    // saca el mas nuevo. false si vacio.
    bool PeekRecent(T& out) const;
    bool PeekOldest(T& out) const;
    bool GetFromRecent(int back, T& out) const; // back=0 mas nuevo
    int Size() const;
    int Capacity() const;
    bool IsEmpty() const;
    bool IsFull() const;
    void Clear();

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
};

template <class T>
int RingBuffer<T>::PhysIndex(int logical) const
{
    return (_head + logical) % _cap;
}

template <class T>
RingBuffer<T>::RingBuffer(int capacity)
{
    if (capacity <= 0) capacity = 1;
    _cap = capacity;
    _buf = new T[_cap];
    _head = 0;
    _count = 0;
}

template <class T>
RingBuffer<T>::~RingBuffer()
{
    delete[] _buf;
    _buf = nullptr;
}

template <class T>
void RingBuffer<T>::Push(const T& value)
{
    if (_count < _cap)
    {
        _buf[PhysIndex(_count)] = value;
        _count++;
    }
    else
    {
        // Lleno: sobrescribe al mas viejo y avanza la ventana.
        _buf[_head] = value;
        _head = (_head + 1) % _cap;
    }
}

template <class T>
bool RingBuffer<T>::PopRecent(T& out)
{
    if (_count <= 0) return false;
    int idx = PhysIndex(_count - 1);
    out = _buf[idx];
    _count--;
    return true;
}

template <class T>
bool RingBuffer<T>::PeekRecent(T& out) const
{
    if (_count <= 0) return false;
    out = _buf[PhysIndex(_count - 1)];
    return true;
}

template <class T>
bool RingBuffer<T>::PeekOldest(T& out) const
{
    if (_count <= 0) return false;
    out = _buf[_head];
    return true;
}

template <class T>
bool RingBuffer<T>::GetFromRecent(int back, T& out) const
{
    if (back < 0 || back >= _count) return false;
    out = _buf[PhysIndex(_count - 1 - back)];
    return true;
}

template <class T>
int RingBuffer<T>::Size() const { return _count; }

template <class T>
int RingBuffer<T>::Capacity() const { return _cap; }

template <class T>
bool RingBuffer<T>::IsEmpty() const { return _count <= 0; }

template <class T>
bool RingBuffer<T>::IsFull() const { return _count >= _cap; }

template <class T>
void RingBuffer<T>::Clear()
{
    _head = 0;
    _count = 0;
}
