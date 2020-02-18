#pragma once
#include "Diablo2.hpp"

#define CACHEHANDLE_LEN	32

#define LRUSIZE_CHARS					128
#define LRUSIZE_MONSTERS				256
#define LRUSIZE_OBJECTS					64
#define LRUSIZE_MISSILES				32
#define LRUSIZE_OVERLAYS				32

// The render targets
enum OpenD2RenderTargets
{
	OD2RT_SDL,			// SDL renderer with hardware acceleration (default for now)
	OD2RT_OPENGL,		// OpenGL
	OD2RT_MAX
};

// Renderer.cpp
namespace Renderer
{
	void Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
	void MapRenderTargetExports(D2ModuleImportStrc* pExport);
}

extern class IRenderer* RenderTarget;	// nullptr if there isn't a render target


///////////////////////////////////////////////////////////////////////
//
//	FUNCTIONS

// Renderer_SDL.cpp
void Renderer_SDL_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
void Renderer_SDL_Shutdown();
void Renderer_SDL_Present();
tex_handle Renderer_SDL_TextureFromDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette);
tex_handle Renderer_SDL_TextureFromAnimatedDC6(char* szDc6Path, char* szHandle, int nPalette);
void Renderer_SDL_DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v);
void Renderer_SDL_DrawTextureFrame(tex_handle texture, int x, int y, DWORD dwFrame);
void Renderer_SDL_DrawTextureFrames(tex_handle texture, int x, int y, DWORD dwStartFrame, DWORD dwEndFrame);
void Renderer_SDL_DeregisterTexture(char* szHandleName, tex_handle texture);
void Renderer_SDL_SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode);
void Renderer_SDL_PollTexture(tex_handle texture, DWORD* dwWidth, DWORD* dwHeight);
bool Renderer_SDL_PixelPerfectDetect(anim_handle anim, int nSrcX, int nSrcY, int nDrawX, int nDrawY, bool bAllowAlpha);
anim_handle Renderer_SDL_RegisterDC6Animation(tex_handle texture, char* szHandle, DWORD dwStartingFrame);
void Renderer_SDL_DeregisterAnimation(anim_handle anim);
void Renderer_SDL_Animate(anim_handle anim, DWORD dwFramerate, int x, int y);
void Renderer_SDL_SetAnimFrame(anim_handle anim, DWORD dwFrame);
DWORD Renderer_SDL_GetAnimFrame(anim_handle anim);
void Renderer_SDL_AddAnimKeyframe(anim_handle anim, int nFrame, AnimKeyframeCallback callback, int nExtraInt);
void Renderer_SDL_RemoveAnimKeyframe(anim_handle anim);
DWORD Renderer_SDL_GetAnimFrameCount(anim_handle anim);
font_handle Renderer_SDL_RegisterFont(char* szFontName);
void Renderer_SDL_DeregisterFont(font_handle font);
void Renderer_SDL_DrawText(font_handle font, char16_t* szText, int x, int y, int w, int h,
	D2TextAlignment alignHorz, D2TextAlignment alignVert);
void Renderer_SDL_AlphaModulateTexture(tex_handle texture, int nAlpha);
void Renderer_SDL_ColorModulateTexture(tex_handle texture, int nRed, int nGreen, int nBlue);
void Renderer_SDL_AlphaModulateFont(font_handle font, int nAlpha);
void Renderer_SDL_ColorModulateFont(font_handle font, int nRed, int nGreen, int nBlue);
void Renderer_SDL_DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a);
void Renderer_SDL_DrawTokenInstance(anim_handle instance, int x, int y, int translvl, int palette);
void Renderer_SDL_Clear();

// Renderer_OpenGL.cpp