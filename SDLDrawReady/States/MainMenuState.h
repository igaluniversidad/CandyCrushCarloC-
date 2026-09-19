#pragma once
#include "../GameState.h"
#include "../Image.h"
#include "../Text.h"

// Menu principal. Es la primera escena, queda al fondo de la pila.
class MainMenuState : public GameState
{
private:
    Platform* platform;
    GameStateManager* manager;
    Image* bg;        // fondo del pack
    Image* logo;      // logo "Cutie Tutti Frutti" del pack
    Image* mascot;    // el pan tostado de mascota
    Image* btnImg;    // dibujo del boton
    Image* gemShow[6];// los 6 personajes para la vitrina
    Text* subtitle;
    Text* best;
    Text* btnPlayT;
    Text* btnBoardT;
    Text* btnExitT;
    Text* hint;
    float bob[6]; // cuanto flota cada personaje
    int bdir[6];  // 1 = sube, -1 = baja

public:
    MainMenuState();
    ~MainMenuState();
    void Init(Platform* platform, GameStateManager* manager) override;
    void Draw() override;
    bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) override;
    void Update(float dt) override;
    void Close() override;
};
