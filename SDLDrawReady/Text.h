#pragma once
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3/SDL.h"
#include <string>
#include "Platform.h"

// =====================================================================
//  Text - Etiqueta con SDL_ttf (base del docente, con fixes de memoria).
//  Fixes: TTF_Init una sola vez (estatico), destructor libera la textura,
//  Update destruye la textura vieja antes de reemplazar (leak original).
//  Para 60 FPS: NO crear Text por frame; cachear y llamar Update solo
//  cuando cambie el mensaje (score/tiempo). Ver GameplayState.
// =====================================================================

class Text
{
public:
    Text(const std::string& font_path, int font_size, std::string message_text, const SDL_Color& color);

    void Display(float x, float y);

    static SDL_Texture* LoadFont(const std::string& font_path, float font_size, std::string message_text, const SDL_Color& color);
    void Update(std::string message_text);
    ~Text();

private:
    SDL_Texture* _text_texture = nullptr;
    mutable SDL_FRect _text_rect;
    std::string message;
    std::string _font_path;
    int _font_size;
    SDL_Color _color;
};
