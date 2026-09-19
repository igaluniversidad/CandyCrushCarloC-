#include "MainMenuState.h"
#include "GameplayState.h"
#include "LeaderboardState.h"
#include "../GameStateManager.h"
#include "../Game/HighScores.h"
#include "../Game/CandyConfig.h"
#include "FontUtil.h"
#include "UiButton.h"
#include <cstdio>

MainMenuState::MainMenuState()
{
    platform = nullptr; manager = nullptr;
    bg = nullptr; logo = nullptr; mascot = nullptr; btnImg = nullptr;
    subtitle = nullptr; best = nullptr;
    btnPlayT = nullptr; btnBoardT = nullptr; btnExitT = nullptr; hint = nullptr;
    for (int i = 0; i < 6; ++i) { gemShow[i] = nullptr; bob[i] = 0; bdir[i] = 1; }
}

MainMenuState::~MainMenuState() {}

void MainMenuState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color blanco = { 255, 255, 255, 255 };
    SDL_Color amarillo = { 255, 230, 120, 255 };
    SDL_Color cafe = { 90, 40, 20, 255 }; // letras de los botones (van sobre amarillo)

    // todo el arte sale del pack match-3 del zip
    bg = new Image();
    bg->LoadImage("Assets/bg.png");
    logo = new Image();
    logo->LoadImage("Assets/logo.png");
    mascot = new Image();
    mascot->LoadImage("Assets/mascot.png");
    btnImg = new Image();
    btnImg->LoadImage("Assets/btn.png");
    for (int i = 0; i < 6; ++i)
    {
        gemShow[i] = new Image();
        char path[64];
        snprintf(path, sizeof(path), "Assets/gem%d.png", i);
        gemShow[i]->LoadImage(path);
    }

    subtitle = new Text(font, 30, "Junta 3 en linea - 4 o mas crea BOMBA", blanco);
    HighScores hs;
    hs.Load(CandyConfig::FILE_HIGHSCORES);
    char buf[64];
    snprintf(buf, sizeof(buf), "Record: %d pts", hs.Best());
    best = new Text(font, 34, buf, amarillo);
    btnPlayT = new Text(font, 30, "JUGAR (Enter)", cafe);
    btnBoardT = new Text(font, 30, "RECORDS", cafe);
    btnExitT = new Text(font, 30, "SALIR (Esc)", cafe);
    hint = new Text(font, 24, "Clic en un dulce y luego en su vecino - 90 segundos / 30 movimientos", blanco);
}

// me ayuda a no repetir los numeros de los botones
static UiButton MenuBtn(float y, Text* t, Image* img) { return UiButton(725, y, 470, 72, t, img); }

void MainMenuState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);

    // logo del pack en grande arriba
    if (logo != nullptr && logo->IsValid())
        platform->RenderImageScaled(logo, 660, 20, 600, 420, 0);

    // fila de personajes flotando
    for (int i = 0; i < 6; ++i)
    {
        if (gemShow[i] == nullptr || !gemShow[i]->IsValid()) continue;
        platform->RenderImageScaled(gemShow[i], 545.0f + i * 145.0f, 450.0f + bob[i], 120, 120, 0);
    }

    // la mascota a un lado
    if (mascot != nullptr && mascot->IsValid())
        platform->RenderImageScaled(mascot, 150, 620, 220, 220, 0);

    if (subtitle != nullptr) subtitle->Display(660, 600);
    if (best != nullptr) best->Display(830, 650);

    float mx = platform->lastmouseX, my = platform->lastmouseY;
    MenuBtn(700, btnPlayT, btnImg).Draw(platform, MenuBtn(700, btnPlayT, btnImg).Contains(mx, my));
    MenuBtn(785, btnBoardT, btnImg).Draw(platform, MenuBtn(785, btnBoardT, btnImg).Contains(mx, my));
    MenuBtn(870, btnExitT, btnImg).Draw(platform, MenuBtn(870, btnExitT, btnImg).Contains(mx, my));
    if (hint != nullptr) hint->Display(480, 970);
    platform->RenderPresent();
}

bool MainMenuState::Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit)
{
    if (quit != nullptr && *quit) { manager->RequestQuit(); return true; }
    for (int i = 0; i < keyDowns->size; ++i)
    {
        int k = keyDowns->get_at(i)->value;
        if (k == SDLK_ESCAPE) { manager->RequestQuit(); return true; }
        if (k == SDLK_RETURN || k == SDLK_SPACE) { manager->RequestPush(new GameplayState()); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        float mx = (mouseX != nullptr) ? *mouseX : 0;
        float my = (mouseY != nullptr) ? *mouseY : 0;
        if (MenuBtn(700, btnPlayT, btnImg).Contains(mx, my)) { manager->RequestPush(new GameplayState()); return true; }
        if (MenuBtn(785, btnBoardT, btnImg).Contains(mx, my)) { manager->RequestPush(new LeaderboardState()); return true; }
        if (MenuBtn(870, btnExitT, btnImg).Contains(mx, my)) { manager->RequestQuit(); return true; }
    }
    (void)keyUps;
    return false;
}

void MainMenuState::Update(float dt)
{
    // cada personaje sube y baja despacito
    for (int i = 0; i < 6; ++i)
    {
        bob[i] = bob[i] + bdir[i] * dt * 25.0f;
        if (bob[i] > 12) bdir[i] = -1;
        if (bob[i] < -12) bdir[i] = 1;
    }
}

void MainMenuState::Close()
{
    delete bg; bg = nullptr;
    delete logo; logo = nullptr;
    delete mascot; mascot = nullptr;
    delete btnImg; btnImg = nullptr;
    for (int i = 0; i < 6; ++i) { delete gemShow[i]; gemShow[i] = nullptr; }
    delete subtitle; subtitle = nullptr;
    delete best; best = nullptr;
    delete btnPlayT; btnPlayT = nullptr;
    delete btnBoardT; btnBoardT = nullptr;
    delete btnExitT; btnExitT = nullptr;
    delete hint; hint = nullptr;
}
