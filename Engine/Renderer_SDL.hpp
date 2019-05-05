#pragma once
#include "Renderer.hpp"

#define MAX_SDL_DRAWCOMMANDS_PER_FRAME	0x1000
#define MAX_SDL_TEXTURECACHE_SIZE		0x100
#define MAX_SDL_ANIMCACHE_SIZE			0x100
#define MAX_SDL_ANIM_FRAMES				0x80
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
	RCMD_ALPHAMODULATE,
	RCMD_COLORMODULATE,
	RCMD_ALPHAMODULATEFONT,
	RCMD_COLORMODULATEFONT,
	RCMD_DRAWRECTANGLE,
	RCMD_DRAWTOKENINSTANCE,
	RCMD_CLEAR,
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

struct SDLAlphaModulateCommand
{
	tex_handle texture;
	int nAlpha;
};

struct SDLColorModulateCommand
{
	tex_handle texture;
	int nRed;
	int nGreen;
	int nBlue;
};

struct SDLDrawRectangleCommand
{
	int x, y, w, h;
	int r, g, b, a;
};

struct SDLDrawTokenInstanceCommand
{
	anim_handle handle;
	int x, y;
	int translvl, palette;
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
		SDLAlphaModulateCommand AlphaModulate;
		SDLColorModulateCommand ColorModulate;
		SDLDrawRectangleCommand DrawRectangle;
		SDLDrawTokenInstanceCommand DrawToken;
	};
};

typedef void(*RenderProcessCommand)(SDLCommand* pCmd);

struct SDLDC6CacheItem
{
	DWORD dwWidth;
	DWORD dwHeight;
	char szHandleName[CACHEHANDLE_LEN];
	SDL_Texture* pTexture;

	bool bHasDC6;			// optional - dc6 image
	DC6Image dc6;			// optional - dc6 image
};

struct SDLDC6AnimationCacheItem
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
	bool bKeyframePresent;
	int nKeyframeFrame;
	AnimKeyframeCallback keyframeCallback;
	int nExtraInt; // extra int for keyframe callbacks
};

struct SDLFontCacheItem
{
	SDL_Texture* pTexture;
	TBLFontFile* pFontData[2];
	char szHandleName[CACHEHANDLE_LEN];
	DC6Image dc6[2];
};