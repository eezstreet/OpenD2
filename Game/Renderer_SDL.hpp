#pragma once
#include "Renderer.hpp"

#define MAX_SDL_DRAWCOMMANDS_PER_FRAME	0x1000
#define MAX_SDL_TEXTURECACHE_SIZE	0x100

/////////////////////////////////////////////////////////////////
//
//	Types and Data Structures

struct SDLPaletteEntry {
	SDL_Color palette[256];
	SDL_Palette* pPal;
};

enum SDLHardwareCommandType
{
	RCMD_DRAWTEXTURE,
	RCMD_MAX,
};

struct SDLDrawTextureCommand
{
	tex_handle tex;
	SDL_Rect src;
	SDL_Rect dst;
};

struct SDLCommand
{
	SDLHardwareCommandType cmdType;
	union
	{
		SDLDrawTextureCommand DrawTexture;
	};
};

typedef void(*RenderProcessCommand)(SDLCommand* pCmd);

struct SDLHardwareTextureCacheItem
{
	DWORD dwWidth;
	DWORD dwHeight;
	char szHandleName[32];
	SDL_Texture* pTexture;
	bool bHasDC6;		// optional - dc6 image
	DC6Image dc6;		// optional - dc6 image
};

//////////////////////////////////////////////////////////////
//
//	Functions

tex_handle Renderer_SDL_AddTextureToCache(SDL_Texture* pTexture, char* str, DWORD dwWidth, DWORD dwHeight, bool* bExists);