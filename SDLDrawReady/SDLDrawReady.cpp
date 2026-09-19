// CandyCrushCarloC++ : punto de entrada. La pila inicia en el Menu Principal.
#include "SDL3/SDL_main.h"
#include "SDL3/SDL.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "States/MainMenuState.h"

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;
    GameStateManager* manager = new GameStateManager();
    manager->SetState(new MainMenuState());
    manager->GameLoop();
    delete manager;
    return 0;
}
