#pragma once // incluir una sola vez
#include "TNode.h" // usa eslabones
#include <iostream> // solo para avisos en consola

// Stack = pila LIFO (ultimo en entrar, primero en salir).
// Como una torre de platos: solo tocas el de arriba.
// El tope es la CABEZA de la lista: Push/Pop son O(1), sin recorrer nada.
// En el juego guarda escenas: Push = ir a escena, Pop = volver.
// Copia profunda con arreglo crudo new T[] (prohibido std::vector).
// Sin SDL aqui: logica pura.

template <class T> // T = cualquier tipo (aqui: punteros a escenas)
class Stack
{
private:
    TNode<T>* _nodoTope; // el plato de hasta arriba (cabeza de la lista)
    int _cantidadElementos; // cuantos platos hay

    void Push(TNode<T>* nodoNuevo); // mete un nodo ya creado al tope

public:
    Stack(); // pila vacia
    Stack(const Stack<T>& otraPila); // copia profunda (duplica todo)
    Stack<T>& operator=(const Stack<T>& otraPila); // asigna duplicando todo
    ~Stack(); // borra todos los nodos

    void Push(T valorNuevo); // crea nodo y lo mete arriba
    T Pop(); // saca y borra el de arriba, regresa su dato
    T Top(); // mira el de arriba sin sacarlo
    bool IsEmpty(); // true si no hay nada
    int GetSize(); // cuantos hay
    void Clear(); // tira todos los platos
    void Print(); // imprime de arriba a abajo
};

// Pila nueva: tope vacio, cero elementos.
template <class T>
Stack<T>::Stack()
{
    _nodoTope = nullptr; // nadie arriba
    _cantidadElementos = 0; // cero platos
}

// Copia profunda: duplica cada dato en el MISMO orden.
// Usa arreglo temporal crudo (sin std::vector, regla de oro).
template <class T>
Stack<T>::Stack(const Stack<T>& otraPila)
{
    _nodoTope = nullptr; // empieza vacia
    _cantidadElementos = 0; // empieza en cero
    int totalElementos = otraPila._cantidadElementos; // cuantos copiar
    if (totalElementos <= 0) return; // vacia = nada que copiar
    T* valoresTemporales = new T[totalElementos]; // cajon temporal
    TNode<T>* nodoActual = otraPila._nodoTope; // empieza desde el tope ajeno
    for (int indice = 0; indice < totalElementos && nodoActual != nullptr; ++indice) // recorre la otra pila
    {
        valoresTemporales[indice] = nodoActual->getDato(); // copia el dato (indice 0 = tope)
        nodoActual = nodoActual->getNext(); // avanza al siguiente
    }
    for (int indice = totalElementos - 1; indice >= 0; --indice) // mete al reves...
        Push(valoresTemporales[indice]); // ...para que quede en el mismo orden
    delete[] valoresTemporales; // libera el cajon temporal
}

// Asigna duplicando: tira lo mio y copia lo ajeno.
template <class T>
Stack<T>& Stack<T>::operator=(const Stack<T>& otraPila)
{
    if (this == &otraPila) // si es la misma pila...
        return *this; // ...no hace nada
    Clear(); // tira todos mis platos
    int totalElementos = otraPila._cantidadElementos; // cuantos copiar
    if (totalElementos <= 0) return *this; // vacia = listo
    T* valoresTemporales = new T[totalElementos]; // cajon temporal
    TNode<T>* nodoActual = otraPila._nodoTope; // desde el tope ajeno
    for (int indice = 0; indice < totalElementos && nodoActual != nullptr; ++indice) // recorre
    {
        valoresTemporales[indice] = nodoActual->getDato(); // copia dato
        nodoActual = nodoActual->getNext(); // avanza
    }
    for (int indice = totalElementos - 1; indice >= 0; --indice) // mete al reves
        Push(valoresTemporales[indice]); // conserva el orden
    delete[] valoresTemporales; // libera cajon
    return *this; // permite a = b = c
}

// Al morir la pila, tira todos los platos (cero fugas).
template <class T>
Stack<T>::~Stack()
{
    Clear(); // borra nodo por nodo
}

// Mete un nodo YA creado arriba del tope. O(1).
template <class T>
void Stack<T>::Push(TNode<T>* nodoNuevo)
{
    if (nodoNuevo == nullptr) return; // nada = nada
    nodoNuevo->setNext(_nodoTope); // el nuevo apunta al tope viejo
    _nodoTope = nodoNuevo; // el nuevo ahora es el tope
    _cantidadElementos++; // un plato mas
}

// Crea un nodo con el valor y lo mete arriba. O(1).
template <class T>
void Stack<T>::Push(T valorNuevo)
{
    TNode<T>* nodoNuevo = new TNode<T>(valorNuevo, _cantidadElementos); // crea eslabon
    Push(nodoNuevo); // lo mete arriba
}

// Saca el de arriba, lo borra y regresa su dato. O(1).
template <class T>
T Stack<T>::Pop()
{
    if (IsEmpty()) // pila vacia...
    {
        std::cerr << "Stack::Pop: pila vacia" << std::endl; // avisa
        return T(); // regresa dato vacio
    }
    TNode<T>* nodoTope = _nodoTope; // guarda el tope
    T datoGuardado = nodoTope->getDato(); // copia su dato
    _nodoTope = nodoTope->getNext(); // el tope ahora es el siguiente
    delete nodoTope; // borra el viejo tope
    _cantidadElementos--; // un plato menos
    return datoGuardado; // regresa el dato
}

// Mira el de arriba sin sacarlo.
template <class T>
T Stack<T>::Top()
{
    if (IsEmpty()) // vacia...
    {
        std::cerr << "Stack::Top: pila vacia" << std::endl; // avisa
        return T(); // dato vacio
    }
    return _nodoTope->getDato(); // lee el tope
}

// True si no hay ningun plato (tope apunta a nada).
template <class T>
bool Stack<T>::IsEmpty()
{
    return _nodoTope == nullptr; // vacio = tope nulo
}

// Cuantos platos hay.
template <class T>
int Stack<T>::GetSize()
{
    return _cantidadElementos; // regresa el contador
}

// Tira todos los platos uno por uno.
template <class T>
void Stack<T>::Clear()
{
    while (!IsEmpty()) // mientras haya algo...
    {
        TNode<T>* nodoTemporal = _nodoTope; // guarda el tope
        _nodoTope = _nodoTope->getNext(); // avanza el tope
        delete nodoTemporal; // borra el viejo
        _cantidadElementos--; // resta uno
    }
    _nodoTope = nullptr; // queda vacia
    _cantidadElementos = 0; // contador en cero
}

// Imprime del tope al fondo (solo para debug).
template <class T>
void Stack<T>::Print()
{
    TNode<T>* nodoActual = _nodoTope; // empieza arriba
    std::cout << "[TOPE] "; // marca inicio
    while (nodoActual != nullptr) // mientras haya nodo...
    {
        std::cout << nodoActual->getDato(); // imprime dato
        if (nodoActual->getNext() != nullptr) std::cout << " -> "; // flecha si hay mas
        nodoActual = nodoActual->getNext(); // avanza
    }
    std::cout << " [FONDO]" << std::endl; // marca final
}
