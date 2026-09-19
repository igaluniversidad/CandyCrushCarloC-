#include "GameOverState.h"
#include "GameplayState.h"
#include "../GameStateManager.h"
#include "../Game/HighScores.h"
#include "../Game/CandyConfig.h"
#include "FontUtil.h"
#include "UiButton.h"
#include <cstdio>

GameOverState::GameOverState(int score) : finalScore(score), isRecord(false)
{
    platform = nullptr; manager = nullptr;
    bg = nullptr; panel = nullptr; btnImg = nullptr;
    title = nullptr; scoreT = nullptr; bestT = nullptr;
    newRecordT = nullptr; btnRetryT = nullptr; btnMenuT = nullptr; btnExitT = nullptr;
}

GameOverState::~GameOverState() {}

void GameOverState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color negro = { 60, 30, 30, 255 };
    SDL_Color rojo = { 200, 40, 80, 255 };
    SDL_Color cafe = { 90, 40, 20, 255 };

    // si el puntaje entra al top 5 lo guardo
    HighScores hs;
    hs.Load(CandyConfig::FILE_HIGHSCORES);
    if (hs.Qualifies(finalScore) >= 0)
    {
        isRecord = true;
        hs.Insert("CARLO", finalScore);
        hs.Save(CandyConfig::FILE_HIGHSCORES);
    }

    bg = new Image();
    bg->LoadImage("Assets/bg.png");
    panel = new Image();
    panel->LoadImage("Assets/panel.png");
    btnImg = new Image();
    btnImg->LoadImage("Assets/btn.png");

    title = new Text(font, 68, "GAME OVER", rojo);
    char buf[64];
    snprintf(buf, sizeof(buf), "Puntaje: %d", finalScore);
    scoreT = new Text(font, 40, buf, negro);
    HighScores hs2;
    hs2.Load(CandyConfig::FILE_HIGHSCORES);
    snprintf(buf, sizeof(buf), "Record: %d", hs2.Best());
    bestT = new Text(font, 36, buf, negro);
    newRecordT = new Text(font, 34, "NUEVO RECORD!", rojo);
    btnRetryT = new Text(font, 30, "OTRA VEZ (Enter)", cafe);
    btnMenuT = new Text(font, 30, "MENU (M)", cafe);
    btnExitT = new Text(font, 30, "SALIR", cafe);
}

void GameOverState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);

    if (panel != nullptr && panel->IsValid())
        platform->RenderImageScaled(panel, 610, 170, 700, 620, 0);

    if (title != nullptr) title->Display(730, 200);
    if (scoreT != nullptr) scoreT->Display(780, 320);
    if (bestT != nullptr) bestT->Display(780, 380);
    if (isRecord && newRecordT != nullptr) newRecordT->Display(780, 440);

    float mx = platform->lastmouseX, my = platform->lastmouseY;
    UiButton b1(740, 520, 440, 66, btnRetryT, btnImg);
    UiButton b2(740, 600, 440, 66, btnMenuT, btnImg);
    UiButton b3(740, 680, 440, 66, btnExitT, btnImg);
    b1.Draw(platform, b1.Contains(mx, my));
    b2.Draw(platform, b2.Contains(mx, my));
    b3.Draw(platform, b3.Contains(mx, my));
    platform->RenderPresent();
}

bool GameOverState::Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit)
{
    if (quit != nullptr && *quit) { manager->RequestQuit(); return true; }
    for (int i = 0; i < keyDowns->size; ++i)
    {
        int k = keyDowns->get_at(i)->value;
        if (k == SDLK_RETURN || k == SDLK_R || k == SDLK_SPACE) { manager->RequestReplace(new GameplayState()); return true; }
        if (k == SDLK_M || k == SDLK_ESCAPE) { manager->RequestPop(); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        UiButton b1(740, 520, 440, 66, btnRetryT, btnImg);
        UiButton b2(740, 600, 440, 66, btnMenuT, btnImg);
        UiButton b3(740, 680, 440, 66, btnExitT, btnImg);
        float mx = *mouseX, my = *mouseY;
        if (b1.Contains(mx, my)) { manager->RequestReplace(new GameplayState()); return true; }
        if (b2.Contains(mx, my)) { manager->RequestPop(); return true; }
        if (b3.Contains(mx, my)) { manager->RequestQuit(); return true; }
    }
    (void)keyUps;
    return false;
}

void GameOverState::Update(float dt) { (void)dt; }

void GameOverState::Close()
{
    delete bg; bg = nullptr;
    delete panel; panel = nullptr;
    delete btnImg; btnImg = nullptr;
    delete title; title = nullptr;
    delete scoreT; scoreT = nullptr;
    delete bestT; bestT = nullptr;
    delete newRecordT; newRecordT = nullptr;
    delete btnRetryT; btnRetryT = nullptr;
    delete btnMenuT; btnMenuT = nullptr;
    delete btnExitT; btnExitT = nullptr;
}
