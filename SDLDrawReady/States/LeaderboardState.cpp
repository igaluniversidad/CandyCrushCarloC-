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
    bg = nullptr; panel = nullptr; btnImg = nullptr;
    title = nullptr; btnBackT = nullptr; empty = nullptr;
    for (int i = 0; i < 5; ++i) rows[i] = nullptr;
}

LeaderboardState::~LeaderboardState() {}

void LeaderboardState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color negro = { 60, 30, 30, 255 }; // el panel es blanco, las letras van oscuras
    SDL_Color rojo = { 200, 40, 80, 255 };
    SDL_Color cafe = { 90, 40, 20, 255 };

    bg = new Image();
    bg->LoadImage("Assets/bg.png");
    panel = new Image();
    panel->LoadImage("Assets/panel.png");
    btnImg = new Image();
    btnImg->LoadImage("Assets/btn.png");

    title = new Text(font, 60, "MEJORES PUNTAJES", rojo);
    HighScores hs;
    hs.Load(CandyConfig::FILE_HIGHSCORES);
    for (int i = 0; i < 5; ++i)
    {
        rows[i] = nullptr;
        if (i < hs.Count())
        {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d. %d pts - %s", i + 1, hs.At(i).score, hs.At(i).name);
            rows[i] = new Text(font, 36, buf, negro);
        }
    }
    if (hs.Count() == 0)
        empty = new Text(font, 30, "Sin records aun. Juega una partida!", negro);
    btnBackT = new Text(font, 34, "VOLVER (Esc)", cafe);
}

void LeaderboardState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);

    // panel blanco del pack
    if (panel != nullptr && panel->IsValid())
        platform->RenderImageScaled(panel, 560, 120, 800, 640, 0);

    if (title != nullptr) title->Display(650, 160);
    for (int i = 0; i < 5; ++i)
        if (rows[i] != nullptr) rows[i]->Display(700, 280.0f + i * 60.0f);
    if (empty != nullptr) empty->Display(660, 340);

        UiButton back(740, 660, 440, 70, btnBackT, btnImg);
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
    UiButton back(740, 660, 440, 70, btnBackT, btnImg);
        if (back.Contains(*mouseX, *mouseY)) { manager->RequestPop(); return true; }
    }
    (void)keyUps;
    return false;
}

void LeaderboardState::Update(float dt) { (void)dt; }

void LeaderboardState::Close()
{
    delete bg; bg = nullptr;
    delete panel; panel = nullptr;
    delete btnImg; btnImg = nullptr;
    delete title; title = nullptr;
    for (int i = 0; i < 5; ++i) { delete rows[i]; rows[i] = nullptr; }
    delete btnBackT; btnBackT = nullptr;
    delete empty; empty = nullptr;
}
