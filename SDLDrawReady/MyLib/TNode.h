#pragma once

// =====================================================================
//  TNode<T> - Nodo generico enlazado (de tu libreria EstructurasDeDatos26-3)
//  Autor original: Igal Shturman Poplawsky (tu repo EstructurasDeDatos26-3)
//  Vendorizado sin cambios de logica para CandyCrushCarloC++.
//  Sin dependencias graficas. Solo memoria cruda: dato + next + id.
//  contadorVivos permite auditar leaks en la defensa oral.
// =====================================================================

template <class T>
class TNode
{
private:
    T _dato;
    TNode* _next;
    int _id;

public:
    static int contadorVivos;
    void setDato(T d);
    T getDato();

    void setId(int id);
    int getId();

    void setNext(TNode* n);
    TNode* getNext();

    TNode();
    TNode(T dato, int id);
    TNode(T dato, TNode* nodo, int id);
    ~TNode();
};

template <class T>
void TNode<T>::setDato(T d)
{
    _dato = d;
}

template <class T>
T TNode<T>::getDato()
{
    return _dato;
}

template <class T>
void TNode<T>::setId(int id)
{
    _id = id;
}

template <class T>
int TNode<T>::getId()
{
    return _id;
}

template <class T>
void TNode<T>::setNext(TNode* n)
{
    _next = n;
}

template <class T>
TNode<T>* TNode<T>::getNext()
{
    return _next;
}

template <class T>
TNode<T>::TNode()
{
    contadorVivos++;
    _dato = T();
    _next = nullptr;
    _id = 0;
}

template <class T>
TNode<T>::TNode(T dato, TNode* nodo, int id)
{
    contadorVivos++;
    _dato = dato;
    _next = nodo;
    _id = id;
}

template <class T>
TNode<T>::TNode(T dato, int id)
{
    contadorVivos++;
    _dato = dato;
    _id = id;
    _next = nullptr;
}

template <class T>
TNode<T>::~TNode()
{
    contadorVivos--;
}

template <class T> int TNode<T>::contadorVivos = 0;
