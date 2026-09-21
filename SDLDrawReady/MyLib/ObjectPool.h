#pragma once // incluir una sola vez

// ObjectPool = ESTRUCTURA PROPIA #1 (no vista en clase).
// Problema: a 60 FPS, hacer new/delete por cada chispa rompe el juego
// (fragmenta la memoria y baja los frames).
// Solucion: aparta TODOS los espacios UNA sola vez y los recicla.
// Como charola de vasos: pides uno (Alloc), lo usas, lo lavas (Free).
// _espaciosMemoria = los vasos. _espacioOcupado = cual esta en uso.
// _indicesLibres = pila de vasos limpios. Alloc = saca, Free = regresa.
// Todo O(1), cero new/delete durante el juego. Memoria acotada.
// T debe poder crearse vacio (ej. Particle). Sin SDL. Sin std::vector.

template <class T> // T = Particle (chispa de explosion)
class ObjectPool
{
private:
    T* _espaciosMemoria; // arreglo fijo con todos los objetos (un solo new[])
    bool* _espacioOcupado; // true = este espacio esta en uso
    int* _indicesLibres; // pila de indices limpios para regalar
    int _capacidadTotal; // cuantos espacios hay en total
    int _topeLibres; // cuantos limpios quedan (tope de la pila)
    int _cantidadActivos; // cuantos estan en uso ahora

public:
    explicit ObjectPool(int capacidadDeseada); // aparta todo de una vez
    ~ObjectPool(); // libera los tres arreglos

    T* Alloc(); // pide un espacio limpio (nullptr si esta lleno)
    void Free(T* punteroAReciclar); // regresa un espacio con su puntero
    void FreeAt(int indiceEspacio); // regresa un espacio con su numero

    bool IsAlive(int indiceEspacio) const; // esta en uso?
    T& Get(int indiceEspacio); // lee/escribe el objeto
    const T& Get(int indiceEspacio) const; // lee el objeto (const)
    int Capacity() const; // total de espacios
    int ActiveCount() const; // cuantos en uso
    int FreeCount() const; // cuantos limpios
    bool IsFull() const; // no quedan limpios?
    bool IsEmpty() const; // nadie en uso?
    void Clear(); // marca todo como limpio

    ObjectPool(const ObjectPool&) = delete; // no copiar
    ObjectPool& operator=(const ObjectPool&) = delete; // no asignar
};

// Aparta los tres arreglos UNA sola vez. Llena la pila de limpios.
template <class T>
ObjectPool<T>::ObjectPool(int capacidadDeseada)
{
    if (capacidadDeseada <= 0) capacidadDeseada = 1; // minimo 1 espacio
    _capacidadTotal = capacidadDeseada; // guarda el total
    _espaciosMemoria = new T[_capacidadTotal]; // cajon de objetos
    _espacioOcupado = new bool[_capacidadTotal]; // marcas de uso
    _indicesLibres = new int[_capacidadTotal]; // pila de limpios
    for (int indice = 0; indice < _capacidadTotal; ++indice) // por cada espacio...
    {
        _espacioOcupado[indice] = false; // ...empieza limpio
        _indicesLibres[indice] = _capacidadTotal - 1 - indice; // ...guarda su numero (para dar 0,1,2...)
    }
    _topeLibres = _capacidadTotal; // todos limpios al inicio
    _cantidadActivos = 0; // nadie en uso
}

// Libera los tres arreglos (un delete[] por cada new[]).
template <class T>
ObjectPool<T>::~ObjectPool()
{
    delete[] _espaciosMemoria; // libera objetos
    delete[] _espacioOcupado; // libera marcas
    delete[] _indicesLibres; // libera pila
    _espaciosMemoria = nullptr; // queda en nada
    _espacioOcupado = nullptr; // queda en nada
    _indicesLibres = nullptr; // queda en nada
}

// Pide un espacio limpio. Marca en uso y regresa su puntero. O(1).
template <class T>
T* ObjectPool<T>::Alloc()
{
    if (_topeLibres <= 0) return nullptr; // lleno = no hay (se ignora, no se traba)
    _topeLibres--; // saca uno de la pila de limpios
    int indiceLibre = _indicesLibres[_topeLibres]; // lee cual toco
    _espacioOcupado[indiceLibre] = true; // marca en uso
    _cantidadActivos++; // uno mas activo
    return &_espaciosMemoria[indiceLibre]; // regresa su direccion
}

// Regresa un espacio usando su puntero. Convierte puntero a numero. O(1).
template <class T>
void ObjectPool<T>::Free(T* punteroAReciclar)
{
    if (punteroAReciclar == nullptr) return; // nada = nada
    long indiceCalculado = (long)(punteroAReciclar - _espaciosMemoria); // resta punteros = indice
    if (indiceCalculado < 0 || indiceCalculado >= _capacidadTotal) return; // fuera = ignora
    FreeAt((int)indiceCalculado); // libera por numero
}

// Regresa un espacio usando su numero. O(1).
template <class T>
void ObjectPool<T>::FreeAt(int indiceEspacio)
{
    if (indiceEspacio < 0 || indiceEspacio >= _capacidadTotal) return; // fuera = nada
    if (!_espacioOcupado[indiceEspacio]) return; // ya limpio = nada
    _espacioOcupado[indiceEspacio] = false; // marca limpio
    _indicesLibres[_topeLibres] = indiceEspacio; // mete su numero a la pila
    _topeLibres++; // un limpio mas
    _cantidadActivos--; // un activo menos
}

// True si este numero esta en uso.
template <class T>
bool ObjectPool<T>::IsAlive(int indiceEspacio) const
{
    if (indiceEspacio < 0 || indiceEspacio >= _capacidadTotal) return false; // fuera = no
    return _espacioOcupado[indiceEspacio]; // lee su marca
}

// Da el objeto para leerlo o llenarlo.
template <class T>
T& ObjectPool<T>::Get(int indiceEspacio)
{
    return _espaciosMemoria[indiceEspacio]; // regresa el objeto
}

// Da el objeto solo para leerlo (version const).
template <class T>
const T& ObjectPool<T>::Get(int indiceEspacio) const
{
    return _espaciosMemoria[indiceEspacio]; // regresa el objeto
}

// Total de espacios del pool.
template <class T>
int ObjectPool<T>::Capacity() const { return _capacidadTotal; } // regresa total

// Cuantos estan en uso ahora.
template <class T>
int ObjectPool<T>::ActiveCount() const { return _cantidadActivos; } // regresa activos

// Cuantos limpios quedan.
template <class T>
int ObjectPool<T>::FreeCount() const { return _topeLibres; } // regresa libres

// True si ya no quedan limpios.
template <class T>
bool ObjectPool<T>::IsFull() const { return _topeLibres <= 0; } // lleno = tope en 0

// True si nadie esta en uso.
template <class T>
bool ObjectPool<T>::IsEmpty() const { return _cantidadActivos <= 0; } // vacio = 0 activos

// Marca todo como limpio (como recien creado).
template <class T>
void ObjectPool<T>::Clear()
{
    for (int indice = 0; indice < _capacidadTotal; ++indice) // por cada espacio...
    {
        _espacioOcupado[indice] = false; // ...marca limpio
        _indicesLibres[indice] = _capacidadTotal - 1 - indice; // ...mete su numero
    }
    _topeLibres = _capacidadTotal; // todos limpios
    _cantidadActivos = 0; // nadie activo
}
