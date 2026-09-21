#pragma once // incluir una sola vez

// TNode = un eslabon de cadena. Guarda un dato y apunta al siguiente.
// Se usa en Stack (pila) y en LinkedQueue (cola).
// contadorVivos cuenta cuantos nodos existen para detectar fugas.

template <class T> // T = cualquier tipo (int, escenas, celdas...)
class TNode
{
private:
    T _datoGuardado; // el valor que guarda este eslabon
    TNode* _siguienteNodo; // a donde apunta (el siguiente eslabon)
    int _identificador; // numero para identificarlo en debug

public:
    static int contadorVivos; // cuantos nodos vivos hay ahora mismo
    void setDato(T nuevoDato); // guarda un dato nuevo
    T getDato(); // lee el dato guardado

    void setId(int nuevoIdentificador); // pone identificador
    int getId(); // lee identificador

    void setNext(TNode* siguienteNodo); // conecta con el siguiente
    TNode* getNext(); // dice quien es el siguiente

    TNode(); // nodo vacio
    TNode(T valorInicial, int identificadorInicial); // nodo con dato
    TNode(T valorInicial, TNode* siguienteNodo, int identificadorInicial); // dato + siguiente
    ~TNode(); // al morir resta 1 a contadorVivos
};

// Guarda el dato nuevo en este nodo.
template <class T>
void TNode<T>::setDato(T nuevoDato)
{
    _datoGuardado = nuevoDato; // copia el dato
}

// Devuelve el dato guardado.
template <class T>
T TNode<T>::getDato()
{
    return _datoGuardado; // regresa el valor
}

// Guarda el identificador nuevo.
template <class T>
void TNode<T>::setId(int nuevoIdentificador)
{
    _identificador = nuevoIdentificador; // copia el numero
}

// Devuelve el identificador.
template <class T>
int TNode<T>::getId()
{
    return _identificador; // regresa el numero
}

// Conecta este nodo con el siguiente eslabon.
template <class T>
void TNode<T>::setNext(TNode* siguienteNodo)
{
    _siguienteNodo = siguienteNodo; // apunta al siguiente
}

// Dice quien es el siguiente eslabon.
template <class T>
TNode<T>* TNode<T>::getNext()
{
    return _siguienteNodo; // regresa el puntero
}

// Nodo vacio: dato por defecto, sin siguiente, id 0.
template <class T>
TNode<T>::TNode()
{
    contadorVivos++; // nace un nodo mas
    _datoGuardado = T(); // dato vacio
    _siguienteNodo = nullptr; // no apunta a nadie
    _identificador = 0; // id cero
}

// Nodo con dato y siguiente eslabon.
template <class T>
TNode<T>::TNode(T valorInicial, TNode* siguienteNodo, int identificadorInicial)
{
    contadorVivos++; // nace un nodo mas
    _datoGuardado = valorInicial; // guarda el dato
    _siguienteNodo = siguienteNodo; // conecta al siguiente
    _identificador = identificadorInicial; // guarda el id
}

// Nodo con dato pero sin siguiente (para el tope de la pila / final de cola).
template <class T>
TNode<T>::TNode(T valorInicial, int identificadorInicial)
{
    contadorVivos++; // nace un nodo mas
    _datoGuardado = valorInicial; // guarda el dato
    _identificador = identificadorInicial; // guarda el id
    _siguienteNodo = nullptr; // aun no apunta a nadie
}

// Al morir el nodo, resta 1 al contador (sirve para ver fugas).
template <class T>
TNode<T>::~TNode()
{
    contadorVivos--; // muere un nodo
}

// Empieza el contador global en 0.
template <class T> int TNode<T>::contadorVivos = 0;
