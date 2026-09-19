#include "Text.h"
#include <iostream>
#include <fstream>
#include <exception>

static bool g_ttfInit = false;

Text::Text(const std::string& font_path, int font_size, std::string message_text, const SDL_Color& color)
{
    if (!g_ttfInit) { TTF_Init(); g_ttfInit = true; }
    message = message_text;
    _font_path = font_path;
    _font_size = font_size;
    _color = color;
    _text_rect.x = 0; _text_rect.y = 0; _text_rect.w = 0; _text_rect.h = 0;
    _text_texture = LoadFont(font_path, font_size, message, color);
    if (_text_texture != nullptr)
        SDL_GetTextureSize(_text_texture, &_text_rect.w, &_text_rect.h);
}

void Text::Display(float x, float y)
{
    if (_text_texture == nullptr || Platform::renderer == nullptr) return;
    _text_rect.x = x;
    _text_rect.y = y;
    SDL_RenderTexture(Platform::renderer, _text_texture, nullptr, &_text_rect);
}

SDL_Texture* Text::LoadFont(const std::string& font_path, float font_size, std::string message_text, const SDL_Color& color)
{
    if (!g_ttfInit) { TTF_Init(); g_ttfInit = true; }
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), font_size);
    if (!font)
    {
        std::cerr << "Text: no se pudo abrir fuente " << font_path << std::endl;
        return nullptr;
    }

    SDL_Surface* text_surface = TTF_RenderText_Solid(font, message_text.c_str(), message_text.length(), color);
    TTF_CloseFont(font);
    font = nullptr;
    if (!text_surface)
    {
        std::cerr << "Text: no se pudo rasterizar \"" << message_text << "\"" << std::endl;
        return nullptr;
    }
    SDL_Texture* text_texture = nullptr;
    if (Platform::renderer != nullptr)
        text_texture = SDL_CreateTextureFromSurface(Platform::renderer, text_surface);
    SDL_DestroySurface(text_surface);
    if (!text_texture)
        std::cerr << "Text: no se pudo crear textura" << std::endl;
    return text_texture;
}

void Text::Update(std::string message_text)
{
    if (message_text == message) return;
    message = message_text;
    SDL_Texture* fresh = LoadFont(_font_path, (float)_font_size, message, _color);
    if (fresh != nullptr)
    {
        if (_text_texture != nullptr) SDL_DestroyTexture(_text_texture);
        _text_texture = fresh;
        SDL_GetTextureSize(_text_texture, &_text_rect.w, &_text_rect.h);
    }
}

Text::~Text()
{
    if (_text_texture != nullptr)
    {
        SDL_DestroyTexture(_text_texture);
        _text_texture = nullptr;
    }
}
