#include "Platform.h"
#include "SDL3/SDL.h"
#include <iostream>
#include "GameState.h"
#include "ListaT.h"

SDL_Renderer* Platform::renderer;

Platform::Platform(std::string name)
{
	width = 1920;
	height = 1080;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "SDL_Init falló: " << SDL_GetError() << "\n";
		return;
	}

	window = SDL_CreateWindow(name.c_str(), width, height, 0);
	if (window == nullptr)
	{
		std::cout << "CreateWindow falló: " << SDL_GetError() << "\n";
		SDL_Quit();
		return;
	}

	renderer = SDL_CreateRenderer(window, nullptr);
	if (renderer == nullptr)
	{
		std::cout << "CreateRenderer falló: " << SDL_GetError() << "\n";
		SDL_Quit();
		return;
	}

	// Intenta VSync Adaptativo. Si no es compatible, activa el VSync normal.
	if (!SDL_SetRenderVSync(renderer, -1))
	{
		SDL_SetRenderVSync(renderer, 1);
	}
}

void Platform::RenderClear()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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

void Platform::RenderImage(Image* image, float x, float y, float angle)
{
	RenderTexture(image, x, y, angle);
}

void Platform::RenderTexture(Image* image, float x, float y, double a)
{
	SDL_FRect srcrect;
	srcrect.x = x;
	srcrect.y = y;
	srcrect.w = image->GetWidth();
	srcrect.h = image->GetHeight();
	SDL_RenderTextureRotated(renderer, image->GetTexture(), NULL, &srcrect, a, NULL, SDL_FLIP_NONE);
}

Platform::~Platform()
{
}

ListaT<SDL_Event>& Platform::GetFrameEvents()
{
	//static std::vector<SDL_Event> frame_events;
	static ListaT<SDL_Event> frame_events;
	return frame_events;
}

void Platform::CheckEvent(GameState* obj, bool (GameState::* f)(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool *leftclick, float *mouseX, float *mouseY))
{
	SDL_Event e;
	ListaT<int> keysDown;
	ListaT<int> keysUp;
	bool _leftclick = false;
	float _mouseX = lastmouseX;
	float _mouseY = lastmouseY;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_EVENT_KEY_DOWN:
			if (e.key.key == SDLK_LEFT)
			{
				keysDown.push_back(SDLK_LEFT);
			}
			if (e.key.key == SDLK_RIGHT)
			{
				keysDown.push_back(SDLK_RIGHT);
			}
			if (e.key.key == SDLK_UP)
			{
				keysDown.push_back(SDLK_UP);
			}
			if (e.key.key == SDLK_DOWN)
			{
				keysDown.push_back(SDLK_DOWN);
			}
			if (e.key.key == SDLK_SPACE)
			{
				keysDown.push_back(SDLK_SPACE);
			}
			if (e.key.key == SDLK_ESCAPE)
			{
				keysDown.push_back(SDLK_ESCAPE);
			}
			if (e.key.key == SDLK_S)
			{
				keysDown.push_back(SDLK_S);
			}
			if (e.key.key == SDLK_BACKSPACE)
			{
				keysDown.push_back(SDLK_BACKSPACE);
			}
			break;


		case SDL_EVENT_KEY_UP:
			if (e.key.key == SDLK_LEFT)
			{
				keysUp.push_back(SDLK_LEFT);
			}
			if (e.key.key == SDLK_RIGHT)
			{
				keysUp.push_back(SDLK_RIGHT);
			}
			if (e.key.key == SDLK_UP)
			{
				keysUp.push_back(SDLK_UP);
			}
			if (e.key.key == SDLK_DOWN)
			{
				keysUp.push_back(SDLK_DOWN);
			}
			if (e.key.key == SDLK_SPACE)
			{
				keysUp.push_back(SDLK_SPACE);
			}
			if (e.key.key == SDLK_ESCAPE)
			{
				keysUp.push_back(SDLK_ESCAPE);
			}
			if (e.key.key == SDLK_S)
			{
				keysUp.push_back(SDLK_S);
			}
			if (e.key.key == SDLK_BACKSPACE)
			{
				keysDown.push_back(SDLK_BACKSPACE);
			}
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
			if (b.button == SDL_BUTTON_LEFT)
			{
				_leftclick = true;
			}
		}
			
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			SDL_MouseButtonEvent& B = e.button;
			if (B.button == SDL_BUTTON_LEFT)
			{
				_leftclick = false;
			}
		}
			
			break;
		}
	}

	(obj->*f)(&keysDown, &keysUp, &_leftclick, &_mouseX, &_mouseY);
}