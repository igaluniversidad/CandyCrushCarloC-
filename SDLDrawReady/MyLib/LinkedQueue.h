#pragma once
#include "TNode.h"
#include <iostream>

// =====================================================================
//  LinkedQueue<T> - Cola FIFO sobre nodos (de tu libreria).
//  Enqueue por atras (_last), Dequeue por adelante (_first): O(1).
//  Corrige el bug clasico: al sacar el ultimo, _last = nullptr.
//  Uso en el juego: cola de Eventos de Destruccion. Cuando hay match,
//  cada celda matched se encola; el Gameplay drena la cola con animacion
//  y SOLO despues aplica gravedad y re-evalua cascadas.
//  Cero SDL. Cero std::vector.
// =====================================================================

template <class T>
class LinkedQueue
{
private:
    TNode<T>* _first;
    TNode<T>* _last;
    int _size;

public:
    LinkedQueue();
    ~LinkedQueue();

    void Enqueue(T value);
    T Dequeue();
    T Front();
    bool IsEmpty();
    int GetSize();
    void Clear();
    void Print();

    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;
};

template <class T>
LinkedQueue<T>::LinkedQueue()
{
    _first = nullptr;
    _last = nullptr;
    _size = 0;
}

template <class T>
LinkedQueue<T>::~LinkedQueue()
{
    Clear();
}

template <class T>
void LinkedQueue<T>::Enqueue(T value)
{
    TNode<T>* n = new TNode<T>(value, _size);
    n->setNext(nullptr);
    if (IsEmpty())
    {
        _first = n;
        _last = n;
    }
    else
    {
        _last->setNext(n);
        _last = n;
    }
    _size++;
}

template <class T>
T LinkedQueue<T>::Dequeue()
{
    if (IsEmpty())
    {
        std::cerr << "LinkedQueue::Dequeue: cola vacia" << std::endl;
        return T();
    }
    TNode<T>* nodo = _first;
    T dato = nodo->getDato();
    _first = nodo->getNext();
    delete nodo;
    _size--;
    if (_first == nullptr)
    {
        _last = nullptr;
    }
    return dato;
}

template <class T>
T LinkedQueue<T>::Front()
{
    if (IsEmpty())
    {
        std::cerr << "LinkedQueue::Front: cola vacia" << std::endl;
        return T();
    }
    return _first->getDato();
}

template <class T>
bool LinkedQueue<T>::IsEmpty()
{
    return _first == nullptr;
}

template <class T>
int LinkedQueue<T>::GetSize()
{
    return _size;
}

template <class T>
void LinkedQueue<T>::Clear()
{
    while (!IsEmpty())
    {
        TNode<T>* tmp = _first;
        _first = _first->getNext();
        delete tmp;
        _size--;
    }
    _first = nullptr;
    _last = nullptr;
    _size = 0;
}

template <class T>
void LinkedQueue<T>::Print()
{
    TNode<T>* cur = _first;
    std::cout << "[FRENTE] ";
    while (cur != nullptr)
    {
        std::cout << cur->getDato();
        if (cur->getNext() != nullptr) std::cout << " -> ";
        cur = cur->getNext();
    }
    std::cout << " [FINAL]" << std::endl;
}
