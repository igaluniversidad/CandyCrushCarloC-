#pragma once
#include <string>
#include "SDL3/SDL.h"
#include "Image.h"
#include "ListaT.h"

class GameState;

// =====================================================================
//  Platform - Ventana + renderer SDL3 (base del docente, extendida).
//  Cambios propios: RenderImageScaled, rects con color, NowSec(),
//  CheckEvent con quit + mas teclas + filtro de repeat + fix BACKSPACE.
//  Ventana 1920x1080 con VSync (60 FPS).
// =====================================================================

class Platform
{
private:
    SDL_Window* window;

public:
    int width;
    int height;
    float lastmouseX;
    float lastmouseY;
    static SDL_Renderer* renderer;
    Platform(std::string name);
    ~Platform();
    void RenderClear();
    void RenderClearColor(unsigned char r, unsigned char g, unsigned char b);
    void RenderImage(Image* image, float x, float y, float angle);
    void RenderImageScaled(Image* image, float x, float y, float w, float h, float angle);
    void RenderPresent();

    ListaT<SDL_Event>& GetFrameEvents();
    void CheckEvent(GameState* obj, bool (GameState::* f)(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit));

    void DrawRect(float x, float y, float w, float h);
    void FillRect(float x, float y, float w, float h,
                  unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void FrameRect(float x, float y, float w, float h,
                   unsigned char r, unsigned char g, unsigned char b);

    static float NowSec();
    static float ClampDt(float dt);

private:
    void RenderTexture(Image* image, float x, float y, double a);
    void RenderTextureScaled(Image* image, float x, float y, float w, float h, double a);
};
