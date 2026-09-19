#pragma once
#include "../GameState.h"
#include "../Image.h"
#include "../Text.h"

// =====================================================================
//  LeaderboardState - Pantalla de records (push encima del menu).
// =====================================================================

class LeaderboardState : public GameState
{
private:
    Platform* platform;
    GameStateManager* manager;
    Image* bg;
    Text* title;
    Text* rows[5];
    Text* btnBackT;
    Text* empty;

public:
    LeaderboardState();
    ~LeaderboardState();
    void Init(Platform* platform, GameStateManager* manager) override;
    void Draw() override;
    bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) override;
    void Update(float dt) override;
    void Close() override;
};

GameState* NewLeaderboardState();
