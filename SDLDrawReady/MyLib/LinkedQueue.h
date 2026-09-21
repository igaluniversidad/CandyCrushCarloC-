#pragma once // incluir una sola vez
#include "TNode.h" // usa eslabones
#include <iostream> // solo avisos en consola

// LinkedQueue = cola FIFO (primero en entrar, primero en salir).
// Como fila del banco: entra por atras, sale por adelante.
// Enqueue por atras (_ultimoNodo), Dequeue por adelante (_primerNodo): O(1).
// Truco clave: al sacar al ultimo, _ultimoNodo = nullptr (si no, crash).
// En el juego guarda celdas a destruir: se encolan los matches y se vacian
// al instante en orden, antes de aplicar gravedad y buscar cascadas.
// Sin SDL. Sin std::vector.

template <class T> // T = CellPos (fila,columna a destruir)
class LinkedQueue
{
private:
    TNode<T>* _primerNodo; // frente de la fila (por aqui sale)
    TNode<T>* _ultimoNodo; // final de la fila (por aqui entra)
    int _cantidadElementos; // cuantos hay en la fila

public:
    LinkedQueue(); // cola vacia
    ~LinkedQueue(); // tira todo lo que quede

    void Enqueue(T valorNuevo); // mete al final
    T Dequeue(); // saca del frente y lo borra
    T Front(); // mira el frente sin sacarlo
    bool IsEmpty(); // true si no hay nadie
    int GetSize(); // cuantos hay
    void Clear(); // vacia toda la fila
    void Print(); // imprime del frente al final

    LinkedQueue(const LinkedQueue&) = delete; // no copiar
    LinkedQueue& operator=(const LinkedQueue&) = delete; // no asignar
};

// Cola nueva: frente y final vacios, cero elementos.
template <class T>
LinkedQueue<T>::LinkedQueue()
{
    _primerNodo = nullptr; // nadie al frente
    _ultimoNodo = nullptr; // nadie al final
    _cantidadElementos = 0; // cero en fila
}

// Al morir, vacia lo que quede (cero fugas).
template <class T>
LinkedQueue<T>::~LinkedQueue()
{
    Clear(); // tira nodo por nodo
}

// Mete un valor al FINAL de la fila. O(1).
template <class T>
void LinkedQueue<T>::Enqueue(T valorNuevo)
{
    TNode<T>* nodoNuevo = new TNode<T>(valorNuevo, _cantidadElementos); // crea eslabon
    nodoNuevo->setNext(nullptr); // el ultimo nunca apunta a nadie
    if (IsEmpty()) // fila vacia?
    {
        _primerNodo = nodoNuevo; // el nuevo es el frente
        _ultimoNodo = nodoNuevo; // el nuevo tambien es el final
    }
    else // ya hay gente...
    {
        _ultimoNodo->setNext(nodoNuevo); // el viejo final apunta al nuevo
        _ultimoNodo = nodoNuevo; // el nuevo ahora es el final
    }
    _cantidadElementos++; // uno mas en la fila
}

// Saca el del FRENTE, lo borra y regresa su dato. O(1).
template <class T>
T LinkedQueue<T>::Dequeue()
{
    if (IsEmpty()) // fila vacia...
    {
        std::cerr << "LinkedQueue::Dequeue: cola vacia" << std::endl; // avisa
        return T(); // dato vacio
    }
    TNode<T>* nodoFrente = _primerNodo; // guarda el frente
    T datoGuardado = nodoFrente->getDato(); // copia su dato
    _primerNodo = nodoFrente->getNext(); // el frente ahora es el siguiente
    delete nodoFrente; // borra el viejo frente
    _cantidadElementos--; // uno menos
    if (_primerNodo == nullptr) // si la fila quedo vacia...
    {
        _ultimoNodo = nullptr; // ...el final tambien queda en nada (fix del crash)
    }
    return datoGuardado; // regresa el dato
}

// Mira el del frente sin sacarlo.
template <class T>
T LinkedQueue<T>::Front()
{
    if (IsEmpty()) // vacia...
    {
        std::cerr << "LinkedQueue::Front: cola vacia" << std::endl; // avisa
        return T(); // dato vacio
    }
    return _primerNodo->getDato(); // lee el frente
}

// True si no hay nadie (frente apunta a nada).
template <class T>
bool LinkedQueue<T>::IsEmpty()
{
    return _primerNodo == nullptr; // vacio = frente nulo
}

// Cuantos hay en la fila.
template <class T>
int LinkedQueue<T>::GetSize()
{
    return _cantidadElementos; // regresa el contador
}

// Vacia toda la fila nodo por nodo.
template <class T>
void LinkedQueue<T>::Clear()
{
    while (!IsEmpty()) // mientras haya alguien...
    {
        TNode<T>* nodoTemporal = _primerNodo; // guarda el frente
        _primerNodo = _primerNodo->getNext(); // avanza el frente
        delete nodoTemporal; // borra el viejo
        _cantidadElementos--; // resta uno
    }
    _primerNodo = nullptr; // frente en nada
    _ultimoNodo = nullptr; // final en nada
    _cantidadElementos = 0; // contador en cero
}

// Imprime del frente al final (debug).
template <class T>
void LinkedQueue<T>::Print()
{
    TNode<T>* nodoActual = _primerNodo; // empieza al frente
    std::cout << "[FRENTE] "; // marca inicio
    while (nodoActual != nullptr) // mientras haya nodo...
    {
        std::cout << nodoActual->getDato(); // imprime dato
        if (nodoActual->getNext() != nullptr) std::cout << " -> "; // flecha si hay mas
        nodoActual = nodoActual->getNext(); // avanza
    }
    std::cout << " [FINAL]" << std::endl; // marca final
}
