#include "GameOverState.h"
#include "../GameStateManager.h"
#include "../Game/HighScores.h"
#include "../Game/CandyConfig.h"
#include "FontUtil.h"
#include "UiButton.h"
#include <cstdio>

extern GameState* NewGameplayState();
extern GameState* NewMainMenuState();

GameOverState::GameOverState(int score) : finalScore(score), isRecord(false)
{
    platform = nullptr; manager = nullptr;
    bg = nullptr; title = nullptr; scoreT = nullptr; bestT = nullptr;
    newRecordT = nullptr; btnRetryT = nullptr; btnMenuT = nullptr; btnExitT = nullptr;
}

GameOverState::~GameOverState() {}

GameState* NewGameOverState(int score) { return new GameOverState(score); }

void GameOverState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color pink = { 255, 120, 180, 255 };
    SDL_Color gold = { 255, 215, 100, 255 };

    HighScores hs;
    hs.Load(CandyConfig::FILE_HIGHSCORES);
    int pos = hs.Qualifies(finalScore);
    if (pos >= 0)
    {
        isRecord = true;
        hs.Insert("CARLO", finalScore);
        hs.Save(CandyConfig::FILE_HIGHSCORES);
    }

    bg = new Image();
    bg->LoadImage(CandyConfig::FILE_BG);
    title = new Text(font, 72, "GAME OVER", pink);
    char buf[64];
    snprintf(buf, sizeof(buf), "Puntaje: %d", finalScore);
    scoreT = new Text(font, 40, buf, white);
    HighScores hs2;
    hs2.Load(CandyConfig::FILE_HIGHSCORES);
    snprintf(buf, sizeof(buf), "Record: %d", hs2.Best());
    bestT = new Text(font, 36, buf, gold);
    newRecordT = new Text(font, 34, "NUEVO RECORD!", gold);
    btnRetryT = new Text(font, 32, "REINTENTAR (Enter/R)", white);
    btnMenuT = new Text(font, 32, "MENU (M/Esc)", white);
    btnExitT = new Text(font, 32, "SALIR", white);
}

void GameOverState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);
    platform->FillRect(610, 170, 700, 600, 20, 8, 30, 240);
    platform->FrameRect(610, 170, 700, 600, 255, 120, 180);
    if (title != nullptr) title->Display(730, 200);
    if (scoreT != nullptr) scoreT->Display(780, 320);
    if (bestT != nullptr) bestT->Display(780, 380);
    if (isRecord && newRecordT != nullptr) newRecordT->Display(780, 440);
    float mx = platform->lastmouseX, my = platform->lastmouseY;
    UiButton b1(760, 510, 400, 64, btnRetryT);
    UiButton b2(760, 590, 400, 64, btnMenuT);
    UiButton b3(760, 670, 400, 64, btnExitT);
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
        if (k == SDLK_RETURN || k == SDLK_R || k == SDLK_SPACE) { manager->RequestReplace(NewGameplayState()); return true; }
        if (k == SDLK_M || k == SDLK_ESCAPE) { manager->RequestPop(); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        UiButton b1(760, 510, 400, 64, btnRetryT);
        UiButton b2(760, 590, 400, 64, btnMenuT);
        UiButton b3(760, 670, 400, 64, btnExitT);
        float mx = *mouseX, my = *mouseY;
        if (b1.Contains(mx, my)) { manager->RequestReplace(NewGameplayState()); return true; }
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
    delete title; delete scoreT; delete bestT; delete newRecordT;
    delete btnRetryT; delete btnMenuT; delete btnExitT;
    title = scoreT = bestT = newRecordT = btnRetryT = btnMenuT = btnExitT = nullptr;
}
