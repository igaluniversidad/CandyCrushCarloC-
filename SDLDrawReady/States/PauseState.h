#pragma once
#include "../GameState.h"
#include "../Image.h"
#include "../Text.h"

// =====================================================================
//  PauseState - Se apila (Push) encima del Gameplay sin destruirlo.
//  Dibuja el gameplay de fondo? No tenemos acceso: dibuja overlay oscuro
//  + panel. Reanudar = Pop. Reiniciar = Pop(pausa)+Replace(gameplay).
//  Menu = Pop x2 (pausa + gameplay, queda el menu de fondo).
// =====================================================================

class PauseState : public GameState
{
private:
    Platform* platform;
    GameStateManager* manager;
    Image* panel;
    Image* btnImg;
    Text* title;
    Text* btnResumeT;
    Text* btnRestartT;
    Text* btnMenuT;
    Text* btnExitT;

public:
    PauseState();
    ~PauseState();
    void Init(Platform* platform, GameStateManager* manager) override;
    void Draw() override;
    bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) override;
    void Update(float dt) override;
    void Close() override;
};
