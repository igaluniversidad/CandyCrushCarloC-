#include "Image.h"
#include "SDL3_image/SDL_image.h"
#include "Platform.h"

Image::~Image()
{
    if (image != nullptr)
    {
        SDL_DestroyTexture(image);
        image = nullptr;
    }
}

void Image::LoadImage(std::string name)
{
    this->name = name;
    SDL_Surface* loadedSurface = IMG_Load(name.c_str());
    if (loadedSurface == nullptr)
    {
        std::cerr << "Image::LoadImage no pudo cargar: " << name
                  << " (" << SDL_GetError() << ")" << std::endl;
        w = 0; h = 0;
        return;
    }
    w = loadedSurface->w;
    h = loadedSurface->h;
    if (Platform::renderer != nullptr)
        image = SDL_CreateTextureFromSurface(Platform::renderer, loadedSurface);
    else
        std::cerr << "Image::LoadImage sin renderer para: " << name << std::endl;
    SDL_DestroySurface(loadedSurface);
}

SDL_Texture* Image::GetTexture()
{
    return image;
}

float Image::GetWidth()
{
    return (float)w;
}

float Image::GetHeight()
{
    return (float)h;
}
