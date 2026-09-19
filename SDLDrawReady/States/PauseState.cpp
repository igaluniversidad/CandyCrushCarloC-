#include "PauseState.h"
#include "../GameStateManager.h"
#include "GameOverState.h"
#include "MainMenuState.h"
#include "FontUtil.h"
#include "UiButton.h"

extern GameState* NewGameplayState();

PauseState::PauseState()
{
    platform = nullptr; manager = nullptr;
    title = nullptr; btnResumeT = nullptr; btnRestartT = nullptr;
    btnMenuT = nullptr; btnExitT = nullptr;
}

PauseState::~PauseState() {}

GameState* NewPauseState() { return new PauseState(); }

void PauseState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    std::string font = ResolveFontPath();
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color pink = { 255, 150, 210, 255 };
    title = new Text(font, 64, "PAUSA", pink);
    btnResumeT = new Text(font, 32, "REANUDAR (P/Esc)", white);
    btnRestartT = new Text(font, 32, "REINICIAR (R)", white);
    btnMenuT = new Text(font, 32, "MENU (M)", white);
    btnExitT = new Text(font, 32, "SALIR", white);
}

void PauseState::Draw()
{
    // Overlay: no borramos el gameplay de fondo del todo; como el GameLoop
    // solo dibuja el tope, oscurecemos con un velo semitransparente sobre
    // un fondo liso (el gameplay sigue vivo debajo en la pila).
    platform->RenderClear();
    platform->FillRect(0, 0, (float)platform->width, (float)platform->height, 10, 5, 20, 255);
    platform->FillRect(660, 180, 600, 560, 30, 15, 50, 255);
    platform->FrameRect(660, 180, 600, 560, 255, 150, 210);
    if (title != nullptr) title->Display(830, 210);
    float mx = platform->lastmouseX, my = platform->lastmouseY;
    UiButton b1(660 + 100, 320, 400, 64, btnResumeT);
    UiButton b2(660 + 100, 400, 400, 64, btnRestartT);
    UiButton b3(660 + 100, 480, 400, 64, btnMenuT);
    UiButton b4(660 + 100, 560, 400, 64, btnExitT);
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
        if (k == SDLK_P || k == SDLK_ESCAPE) { manager->RequestPop(); return true; }
        if (k == SDLK_R) {
            // Pop(pausa) + Replace(gameplay viejo por uno nuevo):
            // = 2 pops + 1 push. El manager hace pops primero.
            manager->RequestPop();
            manager->RequestReplace(NewGameplayState());
            return true;
        }
        if (k == SDLK_M) { manager->RequestPop(); manager->RequestPop(); return true; }
    }
    if (leftclick != nullptr && *leftclick)
    {
        UiButton b1(760, 320, 400, 64, btnResumeT);
        UiButton b2(760, 400, 400, 64, btnRestartT);
        UiButton b3(760, 480, 400, 64, btnMenuT);
        UiButton b4(760, 560, 400, 64, btnExitT);
        float mx = *mouseX, my = *mouseY;
        if (b1.Contains(mx, my)) { manager->RequestPop(); return true; }
        if (b2.Contains(mx, my)) { manager->RequestPop(); manager->RequestReplace(NewGameplayState()); return true; }
        if (b3.Contains(mx, my)) { manager->RequestPop(); manager->RequestPop(); return true; }
        if (b4.Contains(mx, my)) { manager->RequestQuit(); return true; }
    }
    (void)keyUps;
    return false;
}

void PauseState::Update(float dt) { (void)dt; }

void PauseState::Close()
{
    delete title; delete btnResumeT; delete btnRestartT; delete btnMenuT; delete btnExitT;
    title = btnResumeT = btnRestartT = btnMenuT = btnExitT = nullptr;
}
