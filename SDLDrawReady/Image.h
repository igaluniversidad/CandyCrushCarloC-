#pragma once
#include "SDL3/SDL.h"
#include <iostream>
#include <string>

// =====================================================================
//  Image - Envoltorio de textura SDL_image (base del docente, con fixes).
//  Fixes propios: chequeo de nulo (no crashea sin assets), DestroySurface
//  tras crear la textura (leak original), destructor que libera la textura
//  (leak original: sin destructor cada sprite fugaba VRAM en cada escena).
// =====================================================================

class Image
{
private:
    SDL_Texture* image = nullptr;
    std::string name;
    int w = 0;
    int h = 0;
public:
    Image() = default;
    ~Image();
    void LoadImage(std::string name);
    SDL_Texture* GetTexture();
    bool IsValid() const { return image != nullptr; }
    float GetWidth();
    float GetHeight();
};
