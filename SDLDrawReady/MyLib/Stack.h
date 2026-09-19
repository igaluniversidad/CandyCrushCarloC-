#pragma once
#include "TNode.h"
#include <iostream>

// =====================================================================
//  Stack<T> - Pila generica LIFO (de tu libreria EstructurasDeDatos26-3)
//  Autor original: Igal Shturman Poplawsky.
//  Vendorizado para CandyCrushCarloC++ con UN cambio obligatorio:
//    - Se elimino <vector> de la copia profunda (la regla de oro prohibe
//      std::vector). Ahora la copia usa un arreglo crudo new T[n].
//    - Se elimino la dependencia de ConsoleUI; los errores van a cerr.
//  El tope es la CABEZA: Push/Pop O(1), sin recorrer nada.
//  Uso en el juego: GameStateManager (Push = ir a escena, Pop = volver).
//  Cero #include <SDL.h>. Separacion logica vs. graficos garantizada.
// =====================================================================

template <class T>
class Stack
{
private:
    TNode<T>* _top;
    int _size;

    void Push(TNode<T>* n);

public:
    Stack();
    Stack(const Stack<T>& otro);
    Stack<T>& operator=(const Stack<T>& otro);
    ~Stack();

    void Push(T value);
    T Pop();
    T Top();
    bool IsEmpty();
    int GetSize();
    void Clear();
    void Print();
};

template <class T>
Stack<T>::Stack()
{
    _top = nullptr;
    _size = 0;
}

template <class T>
Stack<T>::Stack(const Stack<T>& otro)
{
    _top = nullptr;
    _size = 0;
    int n = otro._size;
    if (n <= 0) return;
    // Copia profunda sin std::vector: arreglo crudo temporal.
    T* vals = new T[n];
    TNode<T>* cur = otro._top;
    for (int i = 0; i < n && cur != nullptr; ++i)
    {
        vals[i] = cur->getDato();
        cur = cur->getNext();
    }
    // vals[0] = tope. Push en inverso para conservar orden.
    for (int i = n - 1; i >= 0; --i)
        Push(vals[i]);
    delete[] vals;
}

template <class T>
Stack<T>& Stack<T>::operator=(const Stack<T>& otro)
{
    if (this == &otro)
        return *this;
    Clear();
    int n = otro._size;
    if (n <= 0) return *this;
    T* vals = new T[n];
    TNode<T>* cur = otro._top;
    for (int i = 0; i < n && cur != nullptr; ++i)
    {
        vals[i] = cur->getDato();
        cur = cur->getNext();
    }
    for (int i = n - 1; i >= 0; --i)
        Push(vals[i]);
    delete[] vals;
    return *this;
}

template <class T>
Stack<T>::~Stack()
{
    Clear();
}

template <class T>
void Stack<T>::Push(TNode<T>* n)
{
    if (n == nullptr) return;
    n->setNext(_top);
    _top = n;
    _size++;
}

template <class T>
void Stack<T>::Push(T value)
{
    TNode<T>* n = new TNode<T>(value, _size);
    Push(n);
}

template <class T>
T Stack<T>::Pop()
{
    if (IsEmpty())
    {
        std::cerr << "Stack::Pop: pila vacia" << std::endl;
        return T();
    }
    TNode<T>* nodo = _top;
    T dato = nodo->getDato();
    _top = nodo->getNext();
    delete nodo;
    _size--;
    return dato;
}

template <class T>
T Stack<T>::Top()
{
    if (IsEmpty())
    {
        std::cerr << "Stack::Top: pila vacia" << std::endl;
        return T();
    }
    return _top->getDato();
}

template <class T>
bool Stack<T>::IsEmpty()
{
    return _top == nullptr;
}

template <class T>
int Stack<T>::GetSize()
{
    return _size;
}

template <class T>
void Stack<T>::Clear()
{
    while (!IsEmpty())
    {
        TNode<T>* tmp = _top;
        _top = _top->getNext();
        delete tmp;
        _size--;
    }
    _top = nullptr;
    _size = 0;
}

template <class T>
void Stack<T>::Print()
{
    TNode<T>* cur = _top;
    std::cout << "[TOPE] ";
    while (cur != nullptr)
    {
        std::cout << cur->getDato();
        if (cur->getNext() != nullptr) std::cout << " -> ";
        cur = cur->getNext();
    }
    std::cout << " [FONDO]" << std::endl;
}
