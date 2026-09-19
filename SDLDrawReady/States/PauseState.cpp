#include "PauseState.h"
#include "GameplayState.h"
#include "../GameStateManager.h"
#include "FontUtil.h"
#include "UiButton.h"

PauseState::PauseState()
{
    platform = nullptr; manager = nullptr;
    panel = nullptr; btnImg = nullptr;
    title = nullptr; btnResumeT = nullptr; btnRestartT = nullptr;
    btnMenuT = nullptr; btnExitT = nullptr;
}

PauseState::~PauseState() {}

void PauseState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color rojo = { 200, 40, 80, 255 };
    SDL_Color cafe = { 90, 40, 20, 255 };

    panel = new Image();
    panel->LoadImage("Assets/panel.png");
    btnImg = new Image();
    btnImg->LoadImage("Assets/btn.png");

    title = new Text(font, 60, "PAUSA", rojo);
    btnResumeT = new Text(font, 30, "SEGUIR (P)", cafe);
    btnRestartT = new Text(font, 30, "OTRA VEZ (R)", cafe);
    btnMenuT = new Text(font, 30, "MENU (M)", cafe);
    btnExitT = new Text(font, 30, "SALIR", cafe);
}

void PauseState::Draw()
{
    // velo oscuro encima de todo
    platform->RenderClear();
    platform->FillRect(0, 0, (float)platform->width, (float)platform->height, 10, 5, 20, 255);

    // panel blanco del pack en el centro
    if (panel != nullptr && panel->IsValid())
        platform->RenderImageScaled(panel, 660, 180, 600, 560, 0);

    if (title != nullptr) title->Display(850, 210);

    float mx = platform->lastmouseX, my = platform->lastmouseY;
    UiButton b1(740, 320, 440, 66, btnResumeT, btnImg);
    UiButton b2(740, 400, 440, 66, btnRestartT, btnImg);
    UiButton b3(740, 480, 440, 66, btnMenuT, btnImg);
    UiButton b4(740, 560, 440, 66, btnExitT, btnImg);
    b1.Draw(platform, b1.Contains(mx, my));
    b2.Draw(platform, b2.Contains(mx, my));
    b3.Draw(platform, b3.Contains(mx, my));
    b4.Draw(platform, b4.Contains(mx, my));
    platform->RenderPresent();
}

bool PauseState::Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit)
{
    if (quit != nullptr && *quit) { manager->RequestQuit(); return true; }
    for (int i = 0; i < keyDowns->size; ++i)
    {
        int k = keyDowns->get_at(i)->value;
        // seguir = solo quito la pausa
        if (k == SDLK_P || k == SDLK_ESCAPE) { manager->RequestPop(); return true; }
        // otra vez = quito pausa y cambio el gameplay por uno nuevo
        if (k == SDLK_R) {
            manager->RequestPop();
            manager->RequestReplace(new GameplayState());
            return true;
        }
        // menu = quito pausa y gameplay, queda el menu del fondo
        if (k == SDLK_M) { manager->RequestPop(); manager->RequestPop(); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        UiButton b1(740, 320, 440, 66, btnResumeT, btnImg);
        UiButton b2(740, 400, 440, 66, btnRestartT, btnImg);
        UiButton b3(740, 480, 440, 66, btnMenuT, btnImg);
        UiButton b4(740, 560, 440, 66, btnExitT, btnImg);
        float mx = *mouseX, my = *mouseY;
        if (b1.Contains(mx, my)) { manager->RequestPop(); return true; }
        if (b2.Contains(mx, my)) { manager->RequestPop(); manager->RequestReplace(new GameplayState()); return true; }
        if (b3.Contains(mx, my)) { manager->RequestPop(); manager->RequestPop(); return true; }
        if (b4.Contains(mx, my)) { manager->RequestQuit(); return true; }
    }
    (void)keyUps;
    return false;
}

void PauseState::Update(float dt) { (void)dt; }

void PauseState::Close()
{
    delete panel; panel = nullptr;
    delete btnImg; btnImg = nullptr;
    delete title; title = nullptr;
    delete btnResumeT; btnResumeT = nullptr;
    delete btnRestartT; btnRestartT = nullptr;
    delete btnMenuT; btnMenuT = nullptr;
    delete btnExitT; btnExitT = nullptr;
}
