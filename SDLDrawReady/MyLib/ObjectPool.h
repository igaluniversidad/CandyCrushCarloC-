#pragma once

// =====================================================================
//  ObjectPool<T> - ESTRUCTURA PROPIA #1 (no vista en clase).
//  Problema que resuelve: a 60 FPS, hacer new/delete por cada particula
//  de explosion fragmenta el heap y tira el framerate. El pool reserva
//  UNA sola vez un arreglo fijo y recicla slots via free-list O(1).
//
//  Diseno:
//    - _slots: arreglo fijo de T (una sola new[] en el ctor, un delete[]).
//    - _alive: bool por slot.
//    - _free: pila de indices libres (arreglo crudo + tope). Alloc = pop,
//      Free = push. Todo O(1), cero asignaciones en el loop.
//  Por que no LinkedList con new por particula? Cada explosion (20-40
//  particulas x varias por segundo) haria cientos de new/delete -> picos
//  de GC del heap, leaks si se olvida un delete en transiciones.
//  Por que no std::vector con erase? erase() es O(n) por corrimiento y
//  puede reubicar (realloc) justo en el frame critico.
//  El pool garantiza memoria acotada y 60 FPS estables. Ideal para VFX.
//
//  T debe ser default-constructible y copiable (ej. Particle).
//  Cero SDL, cero std::vector. Solo new[]/delete[] fijos.
// =====================================================================

template <class T>
class ObjectPool
{
private:
    T* _slots;
    bool* _alive;
    int* _free;
    int _capacity;
    int _freeTop;   // indice del siguiente libre en _free (conteo de libres)
    int _active;

public:
    explicit ObjectPool(int capacity);
    ~ObjectPool();

    // Pide un slot libre. Regresa nullptr si el pool esta lleno.
    // El objeto queda marcado vivo; el llamador lo inicializa.
    T* Alloc();
    // Libera un slot previamente dado por Alloc. O(1).
    void Free(T* ptr);
    // Libera por indice (util para iterar y matar particulas muertas).
    void FreeAt(int index);

    bool IsAlive(int index) const;
    T& Get(int index);
    const T& Get(int index) const;
    int Capacity() const;
    int ActiveCount() const;
    int FreeCount() const;
    bool IsFull() const;
    bool IsEmpty() const;
    void Clear();

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
};

template <class T>
ObjectPool<T>::ObjectPool(int capacity)
{
    if (capacity <= 0) capacity = 1;
    _capacity = capacity;
    _slots = new T[_capacity];
    _alive = new bool[_capacity];
    _free = new int[_capacity];
    for (int i = 0; i < _capacity; ++i)
    {
        _alive[i] = false;
        _free[i] = _capacity - 1 - i; // para que Alloc entregue 0,1,2...
    }
    _freeTop = _capacity;
    _active = 0;
}

template <class T>
ObjectPool<T>::~ObjectPool()
{
    delete[] _slots;
    delete[] _alive;
    delete[] _free;
    _slots = nullptr;
    _alive = nullptr;
    _free = nullptr;
}

template <class T>
T* ObjectPool<T>::Alloc()
{
    if (_freeTop <= 0) return nullptr;
    _freeTop--;
    int idx = _free[_freeTop];
    _alive[idx] = true;
    _active++;
    return &_slots[idx];
}

template <class T>
void ObjectPool<T>::Free(T* ptr)
{
    if (ptr == nullptr) return;
    // Convertir puntero a indice con aritmetica (mismo arreglo).
    long idx = (long)(ptr - _slots);
    if (idx < 0 || idx >= _capacity) return;
    FreeAt((int)idx);
}

template <class T>
void ObjectPool<T>::FreeAt(int index)
{
    if (index < 0 || index >= _capacity) return;
    if (!_alive[index]) return;
    _alive[index] = false;
    _free[_freeTop] = index;
    _freeTop++;
    _active--;
}

template <class T>
bool ObjectPool<T>::IsAlive(int index) const
{
    if (index < 0 || index >= _capacity) return false;
    return _alive[index];
}

template <class T>
T& ObjectPool<T>::Get(int index)
{
    return _slots[index];
}

template <class T>
const T& ObjectPool<T>::Get(int index) const
{
    return _slots[index];
}

template <class T>
int ObjectPool<T>::Capacity() const { return _capacity; }

template <class T>
int ObjectPool<T>::ActiveCount() const { return _active; }

template <class T>
int ObjectPool<T>::FreeCount() const { return _freeTop; }

template <class T>
bool ObjectPool<T>::IsFull() const { return _freeTop <= 0; }

template <class T>
bool ObjectPool<T>::IsEmpty() const { return _active <= 0; }

template <class T>
void ObjectPool<T>::Clear()
{
    for (int i = 0; i < _capacity; ++i)
    {
        _alive[i] = false;
        _free[i] = _capacity - 1 - i;
    }
    _freeTop = _capacity;
    _active = 0;
}
