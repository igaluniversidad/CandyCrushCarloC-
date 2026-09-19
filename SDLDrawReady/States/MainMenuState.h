#pragma once
#include "../GameState.h"
#include "../Image.h"
#include "../Text.h"

// =====================================================================
//  MainMenuState - Escena Menu Principal (fondo de la pila).
// =====================================================================

class MainMenuState : public GameState
{
private:
    Platform* platform;
    GameStateManager* manager;
    Image* bg;
    Image* gemShow[6];
    Text* title;
    Text* subtitle;
    Text* best;
    Text* btnPlayT;
    Text* btnBoardT;
    Text* btnExitT;
    Text* hint;
    float t;

public:
    MainMenuState();
    ~MainMenuState();
    void Init(Platform* platform, GameStateManager* manager) override;
    void Draw() override;
    bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) override;
    void Update(float dt) override;
    void Close() override;
};
