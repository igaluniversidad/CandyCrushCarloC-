#pragma once
#include "SDL3/SDL.h"
#include <iostream>

class Image
{
private:
	SDL_Texture* image;
	std::string name;
	int w;
	int h;
public:
	void LoadImage(std::string name);
	SDL_Texture* GetTexture();
	float GetWidth();
	float GetHeight();
};