#pragma once
#include "Diablo2.hpp"

///////////////////////////////////////////////////////////////////////
//
//	TYPES

// The render targets
enum OpenD2RenderTargets
{
	OD2RT_SDL,			// SDL renderer with hardware acceleration (default for now)
	OD2RT_SDL_SOFTWARE,	// SDL renderer without hardware acceleration
	OD2RT_DIRECTX,		// DirectX / Direct3D
	OD2RT_OPENGL,		// OpenGL
	OD2RT_MAX
};

// Functions that get called from the renderer
typedef void(*RFInit)(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
typedef void(*RFShutdown)();

///////////////////////////////////////////////////////////////////////
//
//	FUNCTIONS

// Renderer_SDL.cpp
void Renderer_SDL_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
void Renderer_SDL_Shutdown();

// Renderer_SDL_Software.cpp

// Renderer_DirectX.cpp

// Renderer_OpenGL.cpp