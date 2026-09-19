#include "Platform.h"
#include "SDL3/SDL.h"
#include <iostream>
#include "GameState.h"
#include "ListaT.h"

SDL_Renderer* Platform::renderer = nullptr;

Platform::Platform(std::string name)
{
    width = 1920;
    height = 1080;
    lastmouseX = width * 0.5f;
    lastmouseY = height * 0.5f;
    window = nullptr;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "SDL_Init fallo: " << SDL_GetError() << "\n";
        return;
    }

    window = SDL_CreateWindow(name.c_str(), width, height, 0);
    if (window == nullptr)
    {
        std::cout << "CreateWindow fallo: " << SDL_GetError() << "\n";
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        std::cout << "CreateRenderer fallo: " << SDL_GetError() << "\n";
        SDL_Quit();
        return;
    }

    if (!SDL_SetRenderVSync(renderer, -1))
    {
        SDL_SetRenderVSync(renderer, 1);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void Platform::RenderClear()
{
    SDL_SetRenderDrawColor(renderer, 18, 10, 28, 255);
    SDL_RenderClear(renderer);
}

void Platform::RenderClearColor(unsigned char r, unsigned char g, unsigned char b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void Platform::RenderPresent()
{
    SDL_RenderPresent(renderer);
}

void Platform::DrawRect(float x, float y, float w, float h)
{
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderRect(renderer, &rect);
}

void Platform::FillRect(float x, float y, float w, float h,
                        unsigned char r, unsigned char g,
                        unsigned char b, unsigned char a)
{
    SDL_FRect rect;
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void Platform::FrameRect(float x, float y, float w, float h,
                         unsigned char r, unsigned char g, unsigned char b)
{
    SDL_FRect rect;
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderRect(renderer, &rect);
}

void Platform::RenderImage(Image* image, float x, float y, float angle)
{
    RenderTexture(image, x, y, angle);
}

void Platform::RenderImageScaled(Image* image, float x, float y, float w, float h, float angle)
{
    RenderTextureScaled(image, x, y, w, h, angle);
}

void Platform::RenderTexture(Image* image, float x, float y, double a)
{
    if (image == nullptr || image->GetTexture() == nullptr) return;
    SDL_FRect srcrect;
    srcrect.x = x;
    srcrect.y = y;
    srcrect.w = image->GetWidth();
    srcrect.h = image->GetHeight();
    SDL_RenderTextureRotated(renderer, image->GetTexture(), NULL, &srcrect, a, NULL, SDL_FLIP_NONE);
}

void Platform::RenderTextureScaled(Image* image, float x, float y, float w, float h, double a)
{
    if (image == nullptr || image->GetTexture() == nullptr) return;
    if (w <= 0 || h <= 0) return;
    SDL_FRect srcrect;
    srcrect.x = x;
    srcrect.y = y;
    srcrect.w = w;
    srcrect.h = h;
    SDL_RenderTextureRotated(renderer, image->GetTexture(), NULL, &srcrect, a, NULL, SDL_FLIP_NONE);
}

Platform::~Platform()
{
    if (renderer != nullptr) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window != nullptr) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
}

ListaT<SDL_Event>& Platform::GetFrameEvents()
{
    static ListaT<SDL_Event> frame_events;
    return frame_events;
}

float Platform::NowSec()
{
    return (float)SDL_GetTicks() / 1000.0f;
}

float Platform::ClampDt(float dt)
{
    if (dt < 0.0f) return 0.0f;
    if (dt > 0.05f) return 0.05f;
    return dt;
}

static void PushKey(ListaT<int>& list, int code) { list.push_back(code); }

void Platform::CheckEvent(GameState* obj, bool (GameState::* f)(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit))
{
    SDL_Event e;
    ListaT<int> keysDown;
    ListaT<int> keysUp;
    bool _leftclick = false;
    bool _quit = false;
    float _mouseX = lastmouseX;
    float _mouseY = lastmouseY;

    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_EVENT_QUIT:
            _quit = true;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (e.key.repeat) break;
            if (e.key.key == SDLK_LEFT) PushKey(keysDown, SDLK_LEFT);
            else if (e.key.key == SDLK_RIGHT) PushKey(keysDown, SDLK_RIGHT);
            else if (e.key.key == SDLK_UP) PushKey(keysDown, SDLK_UP);
            else if (e.key.key == SDLK_DOWN) PushKey(keysDown, SDLK_DOWN);
            else if (e.key.key == SDLK_SPACE) PushKey(keysDown, SDLK_SPACE);
            else if (e.key.key == SDLK_ESCAPE) PushKey(keysDown, SDLK_ESCAPE);
            else if (e.key.key == SDLK_RETURN || e.key.key == SDLK_KP_ENTER) PushKey(keysDown, SDLK_RETURN);
            else if (e.key.key == SDLK_P) PushKey(keysDown, SDLK_P);
            else if (e.key.key == SDLK_U) PushKey(keysDown, SDLK_U);
            else if (e.key.key == SDLK_R) PushKey(keysDown, SDLK_R);
            else if (e.key.key == SDLK_M) PushKey(keysDown, SDLK_M);
            else if (e.key.key == SDLK_H) PushKey(keysDown, SDLK_H);
            else if (e.key.key == SDLK_S) PushKey(keysDown, SDLK_S);
            else if (e.key.key == SDLK_BACKSPACE) PushKey(keysDown, SDLK_BACKSPACE);
            break;

        case SDL_EVENT_KEY_UP:
            if (e.key.key == SDLK_LEFT) PushKey(keysUp, SDLK_LEFT);
            else if (e.key.key == SDLK_RIGHT) PushKey(keysUp, SDLK_RIGHT);
            else if (e.key.key == SDLK_UP) PushKey(keysUp, SDLK_UP);
            else if (e.key.key == SDLK_DOWN) PushKey(keysUp, SDLK_DOWN);
            else if (e.key.key == SDLK_SPACE) PushKey(keysUp, SDLK_SPACE);
            else if (e.key.key == SDLK_ESCAPE) PushKey(keysUp, SDLK_ESCAPE);
            else if (e.key.key == SDLK_RETURN || e.key.key == SDLK_KP_ENTER) PushKey(keysUp, SDLK_RETURN);
            else if (e.key.key == SDLK_P) PushKey(keysUp, SDLK_P);
            else if (e.key.key == SDLK_U) PushKey(keysUp, SDLK_U);
            else if (e.key.key == SDLK_R) PushKey(keysUp, SDLK_R);
            else if (e.key.key == SDLK_M) PushKey(keysUp, SDLK_M);
            else if (e.key.key == SDLK_H) PushKey(keysUp, SDLK_H);
            else if (e.key.key == SDLK_S) PushKey(keysUp, SDLK_S);
            else if (e.key.key == SDLK_BACKSPACE) PushKey(keysUp, SDLK_BACKSPACE);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            _mouseX = e.motion.x;
            lastmouseX = _mouseX;
            _mouseY = e.motion.y;
            lastmouseY = _mouseY;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            SDL_MouseButtonEvent& b = e.button;
            _mouseX = b.x; lastmouseX = _mouseX;
            _mouseY = b.y; lastmouseY = _mouseY;
            if (b.button == SDL_BUTTON_LEFT) _leftclick = true;
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
            break;
        }
    }

    (obj->*f)(&keysDown, &keysUp, &_leftclick, &_mouseX, &_mouseY, &_quit);
}
