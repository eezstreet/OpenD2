#include "Renderer.hpp"
#include <assert.h>

static SDL_Renderer* gpRenderer = nullptr;

/*
 *	Initializes the HW-Accelerated SDL renderer
 */
void Renderer_SDL_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow)
{
	DWORD dwFlags = SDL_RENDERER_ACCELERATED;

	if (!pOpenConfig->bNoRenderText)
	{
		dwFlags |= SDL_RENDERER_TARGETTEXTURE;
	}
	if (pConfig->bVSync)
	{
		dwFlags |= SDL_RENDERER_PRESENTVSYNC;
	}

	gpRenderer = SDL_CreateRenderer(pWindow, -1, dwFlags);

	assert(gpRenderer);
}

/*
 *	Shuts down the HW-Accelerated SDL renderer
 */
void Renderer_SDL_Shutdown()
{
	SDL_DestroyRenderer(gpRenderer);
}
