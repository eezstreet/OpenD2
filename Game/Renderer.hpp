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

///////////////////////////////////////////////////////////////////////
//
//	FUNCTIONS

// Renderer_SDL.cpp
void Renderer_SDL_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
void Renderer_SDL_Shutdown();
void Renderer_SDL_Present();
tex_handle Renderer_SDL_RegisterTexture(char* szHandleName, DWORD dwWidth, DWORD dwHeight);
tex_handle Renderer_SDL_TextureFromStitchedDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette);
void Renderer_SDL_SetTexturePixels(tex_handle texture, BYTE* pPixels, int nPalette);
void Renderer_SDL_DrawTexture(tex_handle texture, DWORD x, DWORD y, DWORD w, DWORD h, DWORD u, DWORD v);


// Renderer_SDL_Software.cpp

// Renderer_DirectX.cpp

// Renderer_OpenGL.cpp