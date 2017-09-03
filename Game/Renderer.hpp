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
tex_handle Renderer_SDL_TextureFromDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette);
tex_handle Renderer_SDL_TextureFromAnimatedDC6(char* szDc6Path, char* szHandle, int nPalette);
void Renderer_SDL_DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v);
void Renderer_SDL_DrawTextureFrame(tex_handle texture, int x, int y, DWORD dwFrame);
void Renderer_SDL_DrawTextureFrames(tex_handle texture, int x, int y, DWORD dwStartFrame, DWORD dwEndFrame);
void Renderer_SDL_DeregisterTexture(char* szHandleName, tex_handle texture);
void Renderer_SDL_SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode);
anim_handle Renderer_SDL_RegisterAnimation(tex_handle texture, char* szHandle, DWORD dwStartingFrame);
void Renderer_SDL_DeregisterAnimation(anim_handle anim);
void Renderer_SDL_Animate(anim_handle anim, DWORD dwFramerate, int x, int y);
void Renderer_SDL_SetAnimFrame(anim_handle anim, DWORD dwFrame);

// Renderer_SDL_Software.cpp

// Renderer_DirectX.cpp

// Renderer_OpenGL.cpp