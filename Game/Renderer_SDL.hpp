#pragma once
#include "Renderer.hpp"

#define MAX_SDL_DRAWCOMMANDS_PER_FRAME	0x1000
#define MAX_SDL_TEXTURECACHE_SIZE		0x100
#define MAX_SDL_ANIMCACHE_SIZE			0x100
#define MAX_SDL_ANIM_FRAMES				0x40
#define MAX_SDL_FONTCACHE_SIZE			0x20

#define MAX_TEXT_DRAW_LINE				128

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
	RCMD_DRAWTEXTUREFRAMES,
	RCMD_DRAWTEXTUREFRAME,
	RCMD_ANIMATE,
	RCMD_SETANIMFRAME,
	RCMD_DRAWTEXT,
	RCMD_MAX,
};

struct SDLDrawTextureCommand
{
	tex_handle tex;
	SDL_Rect src;
	SDL_Rect dst;
};

struct SDLDrawTextureFramesCommand
{
	tex_handle tex;
	int dwDstX;
	int dwDstY;
	DWORD dwStart;
	DWORD dwEnd;
};

struct SDLDrawTextureFrameCommand
{
	tex_handle tex;
	int dwDstX;
	int dwDstY;
	DWORD dwFrame;
};

struct SDLAnimateCommand
{
	anim_handle anim;
	DWORD dwFramerate;
	int dwX;
	int dwY;
};

struct SDLSetAnimFrameCommand
{
	anim_handle anim;
	DWORD dwAnimFrame;
};

struct SDLDrawTextCommand
{
	char16_t text[MAX_TEXT_DRAW_LINE];
	font_handle font;
	int x;
	int y;
	int w;
	int h;
	D2TextAlignment horzAlign;
	D2TextAlignment vertAlign;
};

struct SDLCommand
{
	SDLHardwareCommandType cmdType;
	union
	{
		SDLDrawTextureCommand DrawTexture;
		SDLDrawTextureFramesCommand	DrawTextureFrames;
		SDLDrawTextureFrameCommand DrawTextureFrame;
		SDLAnimateCommand Animate;
		SDLSetAnimFrameCommand SetAnimFrame;
		SDLDrawTextCommand DrawText;
	};
};

typedef void(*RenderProcessCommand)(SDLCommand* pCmd);

struct SDLHardwareTextureCacheItem
{
	DWORD dwWidth;
	DWORD dwHeight;
	char szHandleName[CACHEHANDLE_LEN];
	SDL_Texture* pTexture;

	bool bHasDC6;			// optional - dc6 image
	DC6Image dc6;			// optional - dc6 image
};

struct SDLHardwareAnimationCacheItem
{
	DWORD dwFrame;
	tex_handle texture;
	struct AnimationFrame
	{
		DWORD x, y, w, h;
		int dwOffsetX;
		int dwOffsetY;
	};
	char szHandleName[CACHEHANDLE_LEN];
	AnimationFrame frames[MAX_SDL_ANIM_FRAMES];
	DWORD dwFrameCount;
	DWORD dwLastTick;
};

struct SDLFontCacheItem
{
	SDL_Texture* pTexture;
	TBLFontFile* pFontData[2];
	char szHandleName[CACHEHANDLE_LEN];
	DC6Image dc6[2];
};

//////////////////////////////////////////////////////////////
//
//	Functions

void Renderer_SDL_ClearTextureCache();
void Renderer_SDL_DeregisterAllFonts();