#pragma once
#include "Platform.h"
#include "ListaT.h"

class Platform;
class GameStateManager;

// =====================================================================
//  GameState - Interfaz de escena (base del docente, extendida).
//  Cambios: Input ahora recibe bool* quit (cerrar ventana) y Update recibe
//  dt en segundos para animaciones suaves independientes del framerate.
// =====================================================================

class GameState
{
public:
    virtual ~GameState() {}
    virtual void Init(Platform* platform, GameStateManager* manager) = 0;
    virtual void Draw() = 0;
    virtual bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) = 0;
    virtual void Update(float dt) = 0;
    virtual void Close() = 0;
};
