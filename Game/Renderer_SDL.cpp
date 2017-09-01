#include "Renderer_SDL.hpp"

///////////////////////////////////////////////////////////////////////
//
//	HARDWARE-ACCELERATED SDL RENDERER

static SDL_Renderer* gpRenderer = nullptr;

static SDLPaletteEntry PaletteCache[PAL_MAX_PALETTES]{ 0 };

static SDLCommand gdrawCommands[MAX_SDL_DRAWCOMMANDS_PER_FRAME];
static DWORD numDrawCommandsThisFrame = 0;

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

/*
 *	Backend - All functions enumerated
 */
static RenderProcessCommand RenderingCommands[RCMD_MAX] = {
	RB_DrawTexture,
};

///////////////////////////////////////////////////////////////////////
//
//	FRONTEND FUNCTIONS

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

	Log_ErrorAssert(gpRenderer);

	// Build palettes
	for (int i = 0; i < PAL_MAX_PALETTES; i++)
	{
		D2Palette* pPal = Pal_GetPalette(i);

		PaletteCache[i].pPal = SDL_AllocPalette(256);

		for (int j = 0; j < 256; j++)
		{
			pixel* pal = &((*pPal)[j]);
			PaletteCache[i].palette[j].r = (*pal)[2];
			PaletteCache[i].palette[j].g = (*pal)[1];
			PaletteCache[i].palette[j].b = (*pal)[0];
		}

		SDL_SetPaletteColors(PaletteCache[i].pPal, PaletteCache[i].palette, 0, 256);
	}
}

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
*	Shuts down the HW-Accelerated SDL renderer
*	@author	eezstreet
*/
void Renderer_SDL_Shutdown()
{
	// Free palettes
	for (int i = 0; i < PAL_MAX_PALETTES; i++)
	{
		SDL_FreePalette(PaletteCache[i].pPal);
	}

	Renderer_SDL_ClearTextureCache();
	SDL_DestroyRenderer(gpRenderer);
}

/*
*	Presents the screen
*	@author	eezstreet
*/
void Renderer_SDL_Present()
{
	// Clear backbuffer
	SDL_RenderClear(gpRenderer);

	// Process all render commands on the queue and clear it
	for (int i = 0; i < numDrawCommandsThisFrame; i++)
	{
		SDLCommand* pCmd = &gdrawCommands[i];

		RenderingCommands[pCmd->cmdType](pCmd);
	}
	numDrawCommandsThisFrame = 0;

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
	tex_handle tex = Renderer_SDL_AddTextureToCache(nullptr, szHandleName, dwWidth, dwHeight, nullptr);
	if (tex == INVALID_HANDLE)
	{
		return tex;
	}

	// the texture handle is already guaranteed to be accurate at this point, so we can insert it here
	SDL_Texture* pTexture =
		SDL_CreateTexture(gpRenderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, dwWidth, dwHeight);

	TextureCache[tex].pTexture = pTexture;
	return tex;
}

/*
 *	Adds a new texture to the texture cache
 */
tex_handle Renderer_SDL_AddTextureToCache(SDL_Texture* pTexture, char* str, DWORD dwWidth, DWORD dwHeight, bool* bExists)
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
			if (bExists != nullptr)
			{
				*bExists = true;
			}
			return (tex_handle)dwHash;
		}

		if (pCache->pTexture == nullptr)
		{
			// Empty cache entry, add this item to the texture cache
			pCache->pTexture = pTexture;
			pCache->dwOriginalHash = dwOriginalHash;
			pCache->dwWidth = dwWidth;
			pCache->dwHeight = dwHeight;
			if (bExists != nullptr)
			{
				*bExists = false;
			}
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
 *	Registers a tex_handle based on a stitched-together DC6. The DC6 is not animated.
 */
tex_handle Renderer_SDL_TextureFromStitchedDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette)
{
	D2Palette* pPal = Pal_GetPalette(nPalette);
	bool bExists = false;

	tex_handle tex = Renderer_SDL_AddTextureToCache(nullptr, szHandle, 0, 0, &bExists);
	if (tex == INVALID_HANDLE)
	{
		return tex;
	}

	if (bExists)
	{	// If this texture was registered already, there's no need to do any of this again.
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
	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_INDEX8, &bpp, 
		(Uint32*)&dwRMask, (Uint32*)&dwGMask, (Uint32*)&dwBMask, (Uint32*)&dwAMask);

	DC6_StitchStats(&pCache->dc6, dwStart, dwEnd, &dwStitchCols, &dwStitchRows, &dwTotalWidth, &dwTotalHeight);

	SDL_Surface* pBigSurface = SDL_CreateRGBSurface(0, dwTotalWidth, dwTotalHeight, bpp, dwRMask, dwGMask, dwBMask, dwAMask);
	SDL_SetSurfacePalette(pBigSurface, PaletteCache[nPalette].pPal);
	SDL_SetSurfaceBlendMode(pBigSurface, SDL_BLENDMODE_NONE);

	for (int i = 0; i <= dwEnd - dwStart; i++)
	{
		DC6Frame* pFrame = &pCache->dc6.pFrames[dwStart + i];

		DWORD dwBlitToX = (i % dwStitchCols) * 256;
		DWORD dwBlitToY = floor(i / (float)dwStitchCols) * 255;

		SDL_Surface* pSmallSurface = SDL_CreateRGBSurface(0, pFrame->fh.dwWidth, pFrame->fh.dwHeight,
			bpp, dwRMask, dwGMask, dwBMask, dwAMask);
		SDL_SetSurfacePalette(pSmallSurface, PaletteCache[nPalette].pPal);
		SDL_SetSurfaceBlendMode(pSmallSurface, SDL_BLENDMODE_NONE);

		memcpy(pSmallSurface->pixels, pFrame->pFramePixels, pFrame->fh.dwWidth * pFrame->fh.dwHeight);

		SDL_Rect dstRect = { dwBlitToX, dwBlitToY, pFrame->fh.dwWidth, pFrame->fh.dwHeight };
		SDL_Rect srcRect = { 0 , 1, pFrame->fh.dwWidth, pFrame->fh.dwHeight };
		SDL_BlitSurface(pSmallSurface, &srcRect, pBigSurface, &dstRect);
		SDL_FreeSurface(pSmallSurface);
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(gpRenderer, pBigSurface);

	pCache->dwWidth = dwTotalWidth;
	pCache->dwHeight = dwTotalHeight;
	pCache->pTexture = pTexture;

	SDL_FreeSurface(pBigSurface);

	return tex;
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