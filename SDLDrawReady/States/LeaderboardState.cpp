#include "LeaderboardState.h"
#include "../GameStateManager.h"
#include "../Game/HighScores.h"
#include "../Game/CandyConfig.h"
#include "FontUtil.h"
#include "UiButton.h"
#include <cstdio>

LeaderboardState::LeaderboardState()
{
    platform = nullptr; manager = nullptr;
    bg = nullptr; title = nullptr; btnBackT = nullptr; empty = nullptr;
    for (int i = 0; i < 5; ++i) rows[i] = nullptr;
}

LeaderboardState::~LeaderboardState() {}

GameState* NewLeaderboardState() { return new LeaderboardState(); }

void LeaderboardState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color gold = { 255, 215, 100, 255 };
    bg = new Image();
    bg->LoadImage(CandyConfig::FILE_BG);
    title = new Text(font, 64, "MEJORES PUNTAJES", gold);
    HighScores hs;
    hs.Load(CandyConfig::FILE_HIGHSCORES);
    for (int i = 0; i < 5; ++i)
    {
        if (i < hs.Count())
        {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d. %d pts - %s", i + 1, hs.At(i).score, hs.At(i).name);
            rows[i] = new Text(font, 36, buf, white);
        }
        else rows[i] = nullptr;
    }
    if (hs.Count() == 0)
        empty = new Text(font, 30, "Sin records aun. Juega una partida!", white);
    btnBackT = new Text(font, 34, "VOLVER (Esc)", white);
}

void LeaderboardState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);
    platform->FillRect(560, 120, 800, 640, 20, 10, 35, 230);
    platform->FrameRect(560, 120, 800, 640, 255, 150, 210);
    if (title != nullptr) title->Display(640, 150);
    for (int i = 0; i < 5; ++i)
        if (rows[i] != nullptr) rows[i]->Display(700, 260.0f + i * 60.0f);
    if (empty != nullptr) empty->Display(660, 320);
    UiButton back(760, 660, 400, 70, btnBackT);
    back.Draw(platform, back.Contains(platform->lastmouseX, platform->lastmouseY));
    platform->RenderPresent();
}

bool LeaderboardState::Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit)
{
    if (quit != nullptr && *quit) { manager->RequestQuit(); return true; }
    for (int i = 0; i < keyDowns->size; ++i)
    {
        int k = keyDowns->get_at(i)->value;
        if (k == SDLK_ESCAPE || k == SDLK_RETURN) { manager->RequestPop(); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        UiButton back(760, 660, 400, 70, btnBackT);
        if (back.Contains(*mouseX, *mouseY)) { manager->RequestPop(); return true; }
    }
    (void)keyUps;
    return false;
}

void LeaderboardState::Update(float dt) { (void)dt; }

void LeaderboardState::Close()
{
    delete bg; bg = nullptr;
    delete title; title = nullptr;
    for (int i = 0; i < 5; ++i) { delete rows[i]; rows[i] = nullptr; }
    delete btnBackT; btnBackT = nullptr;
    delete empty; empty = nullptr;
}
