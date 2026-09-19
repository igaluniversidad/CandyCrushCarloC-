#pragma once
#include "../GameState.h"
#include "../Image.h"
#include "../Text.h"

// =====================================================================
//  GameOverState - Pantalla final con puntaje. Reemplaza al Gameplay.
//  Reiniciar = Replace por Gameplay nuevo. Menu = Pop (queda el menu).
// =====================================================================

class GameOverState : public GameState
{
private:
    Platform* platform;
    GameStateManager* manager;
    Image* bg;
    Image* panel;
    Image* btnImg;
    Text* title;
    Text* scoreT;
    Text* bestT;
    Text* newRecordT;
    Text* btnRetryT;
    Text* btnMenuT;
    Text* btnExitT;
    int finalScore;
    bool isRecord;

public:
    explicit GameOverState(int score);
    ~GameOverState();
    void Init(Platform* platform, GameStateManager* manager) override;
    void Draw() override;
    bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) override;
    void Update(float dt) override;
    void Close() override;
};
