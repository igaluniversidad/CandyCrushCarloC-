#include "MainMenuState.h"
#include "../GameStateManager.h"
#include "../Game/HighScores.h"
#include "../Game/CandyConfig.h"
#include "FontUtil.h"
#include "UiButton.h"
#include <cstdio>

extern GameState* NewGameplayState();
extern GameState* NewLeaderboardState();
GameState* NewMainMenuState() { return new MainMenuState(); }

MainMenuState::MainMenuState()
{
    platform = nullptr; manager = nullptr;
    bg = nullptr; title = nullptr; subtitle = nullptr; best = nullptr;
    btnPlayT = nullptr; btnBoardT = nullptr; btnExitT = nullptr; hint = nullptr;
    for (int i = 0; i < 6; ++i) gemShow[i] = nullptr;
    t = 0;
}

MainMenuState::~MainMenuState() {}

void MainMenuState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color pink = { 255, 150, 210, 255 };
    SDL_Color yellow = { 255, 230, 120, 255 };

    bg = new Image();
    bg->LoadImage(CandyConfig::FILE_BG);
    for (int i = 0; i < 6; ++i)
    {
        gemShow[i] = new Image();
        char path[64];
        snprintf(path, sizeof(path), "Assets/gem%d.png", i);
        gemShow[i]->LoadImage(path);
    }
    title = new Text(font, 84, "CandyCrushCarloC++", pink);
    subtitle = new Text(font, 30, "Match-3 arcade con tu propia STL (Stack/Grid/Queue/FloodFill)", white);
    HighScores hs;
    hs.Load(CandyConfig::FILE_HIGHSCORES);
    char buf[64];
    snprintf(buf, sizeof(buf), "Record: %d pts", hs.Best());
    best = new Text(font, 34, buf, yellow);
    btnPlayT = new Text(font, 34, "JUGAR (Enter)", white);
    btnBoardT = new Text(font, 34, "RECORDS", white);
    btnExitT = new Text(font, 34, "SALIR (Esc)", white);
    hint = new Text(font, 24, "Arrastra/clic 2 dulces adyacentes - Junta 3+ en linea - 4+ crea BOMBA - 90s / 30 movs", white);
}

static UiButton MenuBtn(float y, Text* t) { return UiButton(760, y, 400, 70, t); }

void MainMenuState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);
    else
    {
        platform->FillRect(0, 0, (float)platform->width, (float)platform->height, 25, 12, 40, 255);
    }
    // Vitrina de dulces con flotacion (idle animation)
    for (int i = 0; i < 6; ++i)
    {
        if (gemShow[i] == nullptr || !gemShow[i]->IsValid()) continue;
        float bob = 0.0f;
        // sin() sin <cmath>: aproximacion por serie? usamos SDL ticks:
        float ph = t * 2.0f + i * 0.9f;
        // pseudo-seno barato con triangulo suavizado
        float s = ph - (int)(ph / 6.2831f) * 6.2831f;
        bob = (s < 3.1416f) ? (s / 3.1416f * 2 - 1) : (1 - (s - 3.1416f) / 3.1416f * 2);
        bob *= 12.0f;
        platform->RenderImageScaled(gemShow[i], 560.0f + i * 140.0f, 250.0f + bob, 110, 110, 0);
    }
    if (title != nullptr) title->Display(520, 80);
    if (subtitle != nullptr) subtitle->Display(430, 190);
    if (best != nullptr) best->Display(830, 400);

    float mx = platform->lastmouseX, my = platform->lastmouseY;
    MenuBtn(470, btnPlayT).Draw(platform, MenuBtn(470, btnPlayT).Contains(mx, my));
    MenuBtn(560, btnBoardT).Draw(platform, MenuBtn(560, btnBoardT).Contains(mx, my));
    MenuBtn(650, btnExitT).Draw(platform, MenuBtn(650, btnExitT).Contains(mx, my));
    if (hint != nullptr) hint->Display(360, 780);
    platform->RenderPresent();
}

bool MainMenuState::Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit)
{
    if (quit != nullptr && *quit) { manager->RequestQuit(); return true; }
    for (int i = 0; i < keyDowns->size; ++i)
    {
        int k = keyDowns->get_at(i)->value;
        if (k == SDLK_ESCAPE) { manager->RequestQuit(); return true; }
        if (k == SDLK_RETURN || k == SDLK_SPACE) { manager->RequestPush(NewGameplayState()); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        float mx = (mouseX != nullptr) ? *mouseX : 0;
        float my = (mouseY != nullptr) ? *mouseY : 0;
        if (MenuBtn(470, btnPlayT).Contains(mx, my)) { manager->RequestPush(NewGameplayState()); return true; }
        if (MenuBtn(560, btnBoardT).Contains(mx, my)) { manager->RequestPush(NewLeaderboardState()); return true; }
        if (MenuBtn(650, btnExitT).Contains(mx, my)) { manager->RequestQuit(); return true; }
    }
    (void)keyUps;
    return false;
}

void MainMenuState::Update(float dt)
{
    t += dt;
}

void MainMenuState::Close()
{
    delete bg; bg = nullptr;
    for (int i = 0; i < 6; ++i) { delete gemShow[i]; gemShow[i] = nullptr; }
    delete title; delete subtitle; delete best;
    delete btnPlayT; delete btnBoardT; delete btnExitT; delete hint;
    title = subtitle = best = btnPlayT = btnBoardT = btnExitT = hint = nullptr;
}
