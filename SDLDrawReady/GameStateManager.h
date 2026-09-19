#pragma once
#include "GameState.h"
#include "Platform.h"
#include "MyLib/Stack.h"

class GameState;

// =====================================================================
//  GameStateManager - OBLIGATORIO con tu Stack<T> (no StackT del docente).
//  Las escenas se apilan con Push y se desapilan con Pop: Menu -> Gameplay
//  -> Pause (encima, overlay) -> GameOver. Pausa apila visualmente sobre
//  el Gameplay sin destruirlo.
//
//  Anti self-delete: los estados NO se borran a mitad de su propio Input/
//  Update. Piden cambios con Request* y el manager los aplica al final del
//  frame (ProcessRequests). Asi nunca hay use-after-free del `this`.
//  Cada Pop hace Close() + delete: cero leaks en transiciones y reinicios.
// =====================================================================

class GameStateManager
{
private:
    Stack<GameState*> states; // TU pila: Push O(1), Pop O(1)
    Platform* platform;
    bool running;

    // Peticiones diferidas (se aplican entre frames, nunca a mitad de uno)
    int pendingPops;
    static const int MAX_PUSH = 4;
    GameState* pendingPush[MAX_PUSH];
    int pendingPushCount;
    bool pendingQuit;

    void ProcessRequests();
    void ClearAll();

public:
    GameStateManager();
    ~GameStateManager();
    void GameLoop();
    // Inmediatos solo para setup inicial (main). En juego usa Request*.
    void SetState(GameState* state);
    void RealaseState();

    // API diferida (segura desde cualquier estado):
    void RequestPush(GameState* state);
    void RequestPop();
    void RequestReplace(GameState* state); // Pop + Push
    void RequestQuit();
    void RequestMenu(GameState* menuState); // vacia todo y deja el menu

    GameState* Current();
    bool IsRunning() const { return running; }
    Platform* GetPlatform() { return platform; }
    int Depth();
};
