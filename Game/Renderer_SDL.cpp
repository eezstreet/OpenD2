#include "Renderer.hpp"
#include <assert.h>
#include <memory>

///////////////////////////////////////////////////////////////////////
//
//	HARDWARE-ACCELERATED SDL RENDERER

static SDL_Renderer* gpRenderer = nullptr;
static SDL_Texture* gpScreenTexture = nullptr;

/*
 *	Render commands
 *
 *	Each "action" send to the renderer gets sent in a queue to be performed later.
 */

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

#define MAX_SDL_DRAWCOMMANDS_PER_FRAME	0x1000
static SDLCommand gdrawCommands[MAX_SDL_DRAWCOMMANDS_PER_FRAME];
static DWORD numDrawCommandsThisFrame = 0;

/*
 *	Texture cache
 *
 *	Used for atlases, as well as static textures
 */

struct SDLHardwareTextureCacheItem
{
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwOriginalHash;
	SDL_Texture* pTexture;
	bool bHasDC6;		// optional - dc6 image
	DC6Image dc6;		// optional - dc6 image
};

#define MAX_SDL_TEXTURECACHE_SIZE	0x100
static SDLHardwareTextureCacheItem TextureCache[MAX_SDL_TEXTURECACHE_SIZE]{ 0 };

///////////////////////////////////////////////////////////////////////
//
//	BACKEND FUNCTIONS

/*
 *	Backend - draws texture
 */
static void RB_DrawTexture(SDLCommand* pCmd)
{
	if (pCmd->DrawTexture.tex == INVALID_HANDLE)
	{	// bad handle, don't draw it
		return;
	}

	SDL_RenderCopy(gpRenderer, TextureCache[pCmd->DrawTexture.tex].pTexture, &pCmd->DrawTexture.src, &pCmd->DrawTexture.dst);
}

// All of the commands which get processed on the command queue
typedef void(*RenderProcessCommand)(SDLCommand* pCmd);
static RenderProcessCommand RenderingCommands[RCMD_MAX] = {
	RB_DrawTexture,
};

///////////////////////////////////////////////////////////////////////
//
//	FRONTEND FUNCTIONS

/*
 *	Clears out the texture cache
 */
static void Renderer_SDL_ClearTextureCache()
{
	for (int i = 0; i < MAX_SDL_TEXTURECACHE_SIZE; i++)
	{
		SDLHardwareTextureCacheItem* pCache = &TextureCache[i];
		if (pCache->pTexture != nullptr)
		{
			SDL_DestroyTexture(pCache->pTexture);
		}
		if (pCache->bHasDC6)
		{
			DC6_UnloadImage(&pCache->dc6);
		}
		memset(pCache, 0, sizeof(SDLHardwareTextureCacheItem));
	}
}

/*
 *	Adds a new texture to the texture cache
 */
static tex_handle Renderer_SDL_AddTextureToCache(SDL_Texture* pTexture, char* str, DWORD dwWidth, DWORD dwHeight)
{
	DWORD dwHash = D2_strhash(str, 32, MAX_SDL_TEXTURECACHE_SIZE);
	DWORD dwOriginalHash = dwHash;
	DWORD dwChecked = 1;
	SDLHardwareTextureCacheItem* pCache = &TextureCache[dwHash];

	while (dwChecked < MAX_SDL_TEXTURECACHE_SIZE)
	{
		if(pCache->dwOriginalHash == dwOriginalHash)
		{
			// We already added this item to the texture cache, no need to do so again
			return (tex_handle)dwHash;
		}

		if (pCache->pTexture == nullptr)
		{
			// Empty cache entry, add this item to the texture cache
			pCache->pTexture = pTexture;
			pCache->dwOriginalHash = dwOriginalHash;
			pCache->dwWidth = dwWidth;
			pCache->dwHeight = dwHeight;
			return (tex_handle)dwHash;
		}

		dwHash++;
		dwChecked++;
		if (dwHash >= MAX_SDL_TEXTURECACHE_SIZE)
		{	// the hash value might have rolled over, correct this appropriately
			dwHash %= MAX_SDL_TEXTURECACHE_SIZE;
		}
	}

	// we didn't add the texture to the texture cache because it's full. die?
	return INVALID_HANDLE;
}

/*
 *	Initializes the HW-Accelerated SDL renderer
 *	@author	eezstreet
 */
void Renderer_SDL_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow)
{
	DWORD dwFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

	if (pConfig->bVSync)
	{
		dwFlags |= SDL_RENDERER_PRESENTVSYNC;
	}

	gpRenderer = SDL_CreateRenderer(pWindow, -1, dwFlags);

	// Make sure that the renderer got created with the info that we want
	SDL_RendererInfo ri;
	SDL_GetRendererInfo(gpRenderer, &ri);
	if (!(ri.flags & SDL_RENDERER_TARGETTEXTURE))
	{	// We -must- support render to texture, otherwise we will need to fall back to software
		SDL_DestroyRenderer(gpRenderer);
		return;
	}

	assert(gpRenderer);

	// TODO: make the size based on resolution
	//gpScreenTexture = SDL_CreateTexture(gpRenderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
	//SDL_SetRenderTarget(gpRenderer, gpScreenTexture);
}

/*
 *	Shuts down the HW-Accelerated SDL renderer
 *	@author	eezstreet
 */
void Renderer_SDL_Shutdown()
{
	Renderer_SDL_ClearTextureCache();
	SDL_DestroyRenderer(gpRenderer);
}

/*
 *	Presents the screen
 *	@author	eezstreet
 */
void Renderer_SDL_Present()
{
	// Lock screen target texture
	void* pPixels = nullptr;
	int nPitch = 0;

	//SDL_RenderClear(gpRenderer);

	//SDL_LockTexture(gpScreenTexture, nullptr, &pPixels, &nPitch); // pPixels now contains the screen pixels - write only!

	// Process all render commands on the queue and clear it
	for (int i = 0; i < numDrawCommandsThisFrame; i++)
	{
		SDLCommand* pCmd = &gdrawCommands[i];

		RenderingCommands[pCmd->cmdType](pCmd);
	}
	numDrawCommandsThisFrame = 0;

	// Unlock screen target texture
	//SDL_UnlockTexture(gpScreenTexture);

	// Finally, present the renderer
	SDL_RenderPresent(gpRenderer);
}

/*
 *	Registers a tex_handle which we can use later in texture manipulation functions
 *	@author	eezstreet
 */
tex_handle Renderer_SDL_RegisterTexture(char* szHandleName, DWORD dwWidth, DWORD dwHeight)
{
	// HACK
	tex_handle tex = Renderer_SDL_AddTextureToCache(nullptr, szHandleName, dwWidth, dwHeight);
	if (tex == INVALID_HANDLE)
	{
		return tex;
	}

	// the texture handle is already guaranteed to be accurate at this point, so we can insert it here
	SDL_Texture* pTexture = 
		SDL_CreateTexture(gpRenderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, dwWidth, dwHeight);
	TextureCache[tex].pTexture = pTexture;
	return tex;
}

/*
 *	Registers a tex_handle based on a stitched-together DC6. The DC6 is not animated.
 */
tex_handle Renderer_SDL_TextureFromStitchedDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette)
{
	D2Palette* pPal = Pal_GetPalette(nPalette);

	tex_handle tex = Renderer_SDL_AddTextureToCache(nullptr, szHandle, 0, 0);
	if (tex == INVALID_HANDLE)
	{
		return tex;
	}

	SDLHardwareTextureCacheItem* pCache = &TextureCache[tex];
	DC6_LoadImage(szDc6Path, &pCache->dc6);
	pCache->bHasDC6 = true;

	// Calculate how wide it should be
	DWORD dwStitchRows = 0;
	DWORD dwStitchCols = 0;
	DWORD dwTotalWidth = 0;
	DWORD dwTotalHeight = 0;

	DWORD dwRMask = 0, dwGMask = 0, dwBMask = 0, dwAMask = 0;
	int bpp = 0;
	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_BGRA8888, &bpp, 
		(Uint32*)&dwRMask, (Uint32*)&dwGMask, (Uint32*)&dwBMask, (Uint32*)&dwAMask);

	DC6_StitchStats(&pCache->dc6, dwStart, dwEnd, &dwStitchCols, &dwStitchRows, &dwTotalWidth, &dwTotalHeight);

	SDL_Surface* pSurface = SDL_CreateRGBSurface(0, 256, 256, bpp, dwRMask, dwGMask, dwBMask, dwAMask);
	SDL_Surface* pBigSurface = SDL_CreateRGBSurface(0, dwTotalWidth, dwTotalHeight, bpp, dwRMask, dwGMask, dwBMask, dwAMask);
	SDL_SetSurfaceBlendMode(pSurface, SDL_BLENDMODE_NONE);
	SDL_SetSurfaceBlendMode(pBigSurface, SDL_BLENDMODE_NONE);

	for (int i = 0; i <= dwEnd - dwStart; i++)
	{
		DC6Frame* pFrame = &pCache->dc6.pFrames[dwStart + i];

		DWORD dwBlitToX = (i % dwStitchCols) * 256;
		DWORD dwBlitToY = floor(i / dwStitchCols) * 255;
		
		for (int y = 0; y < pFrame->fh.dwHeight; y++)
		{
			for (int x = 0; x < pFrame->fh.dwWidth; x++)
			{
				BYTE nPixelColor = pFrame->pFramePixels[(y * pFrame->fh.dwWidth) + x];
				DWORD* outPixels = (DWORD*)pSurface->pixels;
				outPixels[(y * 256) + x] = (((*pPal)[nPixelColor][2] << 8) |
					((*pPal)[nPixelColor][1] << 16) |
					((*pPal)[nPixelColor][0] << 24));
			}
		}

		SDL_Rect srcRect = { 0, 1, pFrame->fh.dwWidth, pFrame->fh.dwHeight };
		SDL_Rect dstRect = { dwBlitToX, dwBlitToY, pFrame->fh.dwWidth, pFrame->fh.dwHeight };
		SDL_BlitSurface(pSurface, &srcRect, pBigSurface, &dstRect);
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(gpRenderer, pBigSurface);

	pCache->dwWidth = dwTotalWidth;
	pCache->dwHeight = dwTotalHeight;
	pCache->pTexture = pTexture;

	SDL_FreeSurface(pSurface);
	SDL_FreeSurface(pBigSurface);

	return tex;
}

/*
 *	Sets a texture's pixels and palette.
 *	This is fine for static images that won't really change. We'll need to use atlasing for things like items, etc
 *	@author	eezstreet
 */
void Renderer_SDL_SetTexturePixels(tex_handle texture, BYTE* pPixels, int nPalette)
{
	D2Palette* pPal = Pal_GetPalette(nPalette);
	if (pPal == nullptr)
	{	// bad palette
		return;
	}

	if (texture == INVALID_HANDLE)
	{	// bad handle
		return;
	}

	// note: we assume that the pixels provided are of the same width/height as the texture says...
	DWORD* pWriteToPixels;
	int nPitch = 0;

	SDLHardwareTextureCacheItem* pCache = &TextureCache[texture];
	if (pCache->pTexture == nullptr)
	{	// bad texture assigned to this item
		return;
	}

	// Lock the texture (so the VRAM is in RAM) and copy each individual pixel color (mapped from the palette) to the texture
	SDL_LockTexture(pCache->pTexture, NULL, (void**)&pWriteToPixels, &nPitch);
	for (int i = 0; i < pCache->dwHeight; i++)
	{
		for (int j = 0; j < pCache->dwWidth; j++)
		{
			BYTE nPixelColor = pPixels[(i * pCache->dwWidth) + j];
			DWORD dwPixelPosition = i * (nPitch / sizeof(DWORD)) + j;
			DWORD dwColor;

			if (nPixelColor == 0x80)
			{
				dwColor = 0xFF;	// fully transparent pixels
			}
			else
			{
				dwColor = 
					(((*pPal)[nPixelColor][2] << 8) | 
					((*pPal)[nPixelColor][1] << 16) | 
						((*pPal)[nPixelColor][0] << 24));
			}

			pWriteToPixels[dwPixelPosition] = dwColor;
		}
	}
	SDL_UnlockTexture(pCache->pTexture);
}

/*
 *	Draws the texture by creating a render command.
 *	@author eezstreet
 */
void Renderer_SDL_DrawTexture(tex_handle texture, DWORD x, DWORD y, DWORD w, DWORD h, DWORD u, DWORD v)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (texture == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame++];
	pCommand->cmdType = RCMD_DRAWTEXTURE;
	pCommand->DrawTexture.src.x = u;
	pCommand->DrawTexture.src.y = v;
	pCommand->DrawTexture.src.w = pCommand->DrawTexture.dst.w = w;
	pCommand->DrawTexture.src.h = pCommand->DrawTexture.dst.h = h;
	pCommand->DrawTexture.dst.x = x;
	pCommand->DrawTexture.dst.y = y;
	pCommand->DrawTexture.tex = texture;
}