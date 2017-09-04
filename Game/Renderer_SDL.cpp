#include "Renderer_SDL.hpp"

///////////////////////////////////////////////////////////////////////
//
//	HARDWARE-ACCELERATED SDL RENDERER

static SDL_Renderer* gpRenderer = nullptr;

static SDLPaletteEntry PaletteCache[PAL_MAX_PALETTES]{ 0 };

static SDLCommand gdrawCommands[MAX_SDL_DRAWCOMMANDS_PER_FRAME];
static DWORD numDrawCommandsThisFrame = 0;

static SDLHardwareTextureCacheItem TextureCache[MAX_SDL_TEXTURECACHE_SIZE]{ 0 };
static SDLHardwareAnimationCacheItem AnimCache[MAX_SDL_ANIMCACHE_SIZE]{ 0 };

static SDL_Texture* gpRenderTexture = nullptr;

///////////////////////////////////////////////////////////////////////
//
//	BACKEND FUNCTIONS

/*
 *	Backend - draws texture
 */
static void RB_DrawTexture(SDLCommand* pCmd)
{
	SDL_RenderCopy(gpRenderer, TextureCache[pCmd->DrawTexture.tex].pTexture, &pCmd->DrawTexture.src, &pCmd->DrawTexture.dst);
}

/*
 *	Backend - draws specific texture frames
 */
static void RB_DrawTextureFrames(SDLCommand* pCmd)
{
	DWORD dwWidth = 0, dwHeight = 0;
	DWORD dwStartY = 0;
	SDL_Rect d{ pCmd->DrawTextureFrames.dwDstX, pCmd->DrawTextureFrames.dwDstY, 0, 0 };
	SDL_Rect s{ 0 };
	SDLHardwareTextureCacheItem* pCache = &TextureCache[pCmd->DrawTextureFrames.tex];
	DC6Frame* pFrame;

	for (int i = pCmd->DrawTextureFrames.dwStart; i <= pCmd->DrawTextureFrames.dwEnd; i++)
	{
		// get source width/height
		pFrame = &pCache->dc6.pFrames[i];
		dwHeight = pFrame->fh.dwHeight;
		dwWidth += pFrame->fh.dwWidth;
	}

	d.w = dwWidth;
	d.h = dwHeight;
	s.w = dwWidth;
	s.h = dwHeight;

	// get source y
	for (int i = 0; i < pCmd->DrawTextureFrames.dwStart; i++)
	{
		pFrame = &pCache->dc6.pFrames[i];
		if (pFrame->fh.dwWidth < 256)
		{
			dwStartY += pFrame->fh.dwHeight;
		}
	}

	s.y = dwStartY;

	SDL_RenderCopy(gpRenderer, pCache->pTexture, &s, &d);
}

/*
 *	Backend - Draws a single frame
 */
static void RB_DrawTextureFrame(SDLCommand* pCmd)
{
	SDLHardwareTextureCacheItem* pCache = &TextureCache[pCmd->DrawTextureFrame.tex];
	DC6Frame* pDesiredFrame;
	SDL_Rect s{ 0 };
	SDL_Rect d{ 0 };

	pDesiredFrame = &pCache->dc6.pFrames[pCmd->DrawTextureFrame.dwFrame];

	s.w = pDesiredFrame->fh.dwWidth;
	s.h = pDesiredFrame->fh.dwHeight;
	d.x = pCmd->DrawTextureFrame.dwDstX + pDesiredFrame->fh.dwOffsetX;
	d.y = pCmd->DrawTextureFrame.dwDstY + pDesiredFrame->fh.dwOffsetY;

	// get source X and Y
	for (int i = 0; i < pCmd->DrawTextureFrame.dwFrame; i++)
	{
		DC6Frame* pFrame = &pCache->dc6.pFrames[i];
		if (pFrame->fh.dwWidth < 256)
		{
			s.y += pFrame->fh.dwHeight;
		}
		if (pFrame->fh.dwWidth == 256)
		{
			s.x += pFrame->fh.dwWidth;
		}
		else
		{
			s.x = 0;
		}
	}

	SDL_RenderCopy(gpRenderer, pCache->pTexture, &s, &d);
}

/*
 *	Backend - Animates a texture
 */
static void RB_Animate(SDLCommand* pCmd)
{
	DWORD dwTicks = SDL_GetTicks();
	SDLHardwareAnimationCacheItem* pCache = &AnimCache[pCmd->Animate.anim];
	DWORD dwAnimRate = pCmd->Animate.dwFramerate;
	DWORD dwOriginalFrame = pCache->dwFrame;
	SDL_Rect s{ 0 };
	SDL_Rect d{ 0 };

	// advance (and limit) frame
	pCache->dwFrame += (dwTicks - pCache->dwLastTick) / (1000 / dwAnimRate);
	pCache->dwFrame %= pCache->dwFrameCount;

	// draw it
	s.x = pCache->frames[pCache->dwFrame].x;
	s.y = pCache->frames[pCache->dwFrame].y + 1;
	s.w = pCache->frames[pCache->dwFrame].w;
	s.h = pCache->frames[pCache->dwFrame].h - 1;
	d.x = pCmd->Animate.dwX + pCache->frames[pCache->dwFrame].dwOffsetX;
	d.y = pCmd->Animate.dwY + pCache->frames[pCache->dwFrame].dwOffsetY;
	d.w = s.w;
	d.h = s.h;
	SDL_RenderCopy(gpRenderer, TextureCache[pCache->texture].pTexture, &s, &d);

	if (dwOriginalFrame != pCache->dwFrame)
	{	// Frame changed, so change the last animation tick!
		pCache->dwLastTick = dwTicks;
	}
}

/*
 *	Backend - Sets an animation frame
 */
static void RB_SetAnimationFrame(SDLCommand* pCmd)
{
	AnimCache[pCmd->SetAnimFrame.anim].dwFrame = pCmd->SetAnimFrame.dwAnimFrame;
}

/*
 *	Backend - All functions enumerated
 */
static RenderProcessCommand RenderingCommands[RCMD_MAX] = {
	RB_DrawTexture,
	RB_DrawTextureFrames,
	RB_DrawTextureFrame,
	RB_Animate,
	RB_SetAnimationFrame,
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
			if (j == 0)
			{
				PaletteCache[i].palette[j].a = 0;
			}
			else
			{
				PaletteCache[i].palette[j].a = 255;
			}
		}

		SDL_SetPaletteColors(PaletteCache[i].pPal, PaletteCache[i].palette, 0, 256);
	}

	// Clear anim cache
	memset(AnimCache, 0, sizeof(SDLHardwareAnimationCacheItem) * MAX_SDL_ANIMCACHE_SIZE);
	for (int i = 0; i < MAX_SDL_ANIMCACHE_SIZE; i++)
	{
		AnimCache[i].texture = INVALID_HANDLE;
	}

	gpRenderTexture = SDL_CreateTexture(gpRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);
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

	SDL_DestroyTexture(gpRenderTexture);
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

	// Set render target to be render texture
	SDL_SetRenderTarget(gpRenderer, gpRenderTexture);

	// Process all render commands on the queue and clear it
	for (int i = 0; i < numDrawCommandsThisFrame; i++)
	{
		SDLCommand* pCmd = &gdrawCommands[i];

		RenderingCommands[pCmd->cmdType](pCmd);
	}
	numDrawCommandsThisFrame = 0;

	// Copy the render target texture to the main renderer
	SDL_SetRenderTarget(gpRenderer, nullptr);
	SDL_RenderCopy(gpRenderer, gpRenderTexture, nullptr, nullptr);

	// Finally, present the renderer
	SDL_RenderPresent(gpRenderer);
}

/*
 *	Finds a texture handle, but does not register it
 */
static tex_handle Renderer_SDL_GetTextureInCache(char* szHandleName)
{
	DWORD dwChecked = 0;
	DWORD dwTextureHash = D2_strhash(szHandleName, CACHEHANDLE_LEN, MAX_SDL_TEXTURECACHE_SIZE);

	while (dwChecked < MAX_SDL_TEXTURECACHE_SIZE)
	{
		if (!D2_stricmp(TextureCache[dwTextureHash].szHandleName, szHandleName))
		{
			return dwTextureHash;
		}
		else if (TextureCache[dwTextureHash].pTexture == nullptr)
		{
			return dwTextureHash;
		}

		dwTextureHash++;
		dwTextureHash %= MAX_SDL_TEXTURECACHE_SIZE;

		dwChecked++;
	}

	return INVALID_HANDLE;
}

/*
 *	Deregisters a texture, wiping it out from the game. We can re-register it later.
 *	@param	szHandleName	The handle name of the texture. You can supply this with nullptr if you don't know it or need it.
 *	@param	texture			The handle of the texture. It's always faster to use the handle.
 */
void Renderer_SDL_DeregisterTexture(char* szHandleName, tex_handle texture)
{
	SDLHardwareTextureCacheItem* pCache;

	if (texture == INVALID_HANDLE && szHandleName == nullptr)
	{	// We don't know *either* the handle or the handle name. Very bad.
		return;
	}

	if (texture == INVALID_HANDLE)
	{	// get the texture from the handle name
		texture = Renderer_SDL_GetTextureInCache(szHandleName);
	}

	if (texture == INVALID_HANDLE)
	{	// still couldn't find it
		return;
	}

	pCache = &TextureCache[texture];
	
	SDL_DestroyTexture(pCache->pTexture);
	if (pCache->bHasDC6)
	{
		DC6_UnloadImage(&pCache->dc6);
	}
	memset(pCache, 0, sizeof(SDLHardwareTextureCacheItem));
}

/*
*	Clears out the texture cache
*/
void Renderer_SDL_ClearTextureCache()
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
 *	Registers a tex_handle based on a stitched-together DC6. The DC6 is not animated.
 */
tex_handle Renderer_SDL_TextureFromDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette)
{
	D2Palette* pPal = Pal_GetPalette(nPalette);
	bool bExists = false;
	tex_handle tex = Renderer_SDL_GetTextureInCache(szHandle);
	SDLHardwareTextureCacheItem* pCache;
	SDL_Surface* pBigSurface;
	SDL_Texture* pTexture;

	if (tex == INVALID_HANDLE)
	{
		return tex;
	}

	pCache = &TextureCache[tex];

	if (pCache->pTexture != nullptr)
	{
		return tex; // already registered
	}

	DC6_LoadImage(szDc6Path, &pCache->dc6);
	pCache->bHasDC6 = true;

	// Calculate how wide it should be
	DWORD dwStitchRows = 0;
	DWORD dwStitchCols = 0;
	DWORD dwTotalWidth = 0;
	DWORD dwTotalHeight = 0;

	DC6_StitchStats(&pCache->dc6, dwStart, dwEnd, &dwStitchCols, &dwStitchRows, &dwTotalWidth, &dwTotalHeight);

	pBigSurface = SDL_CreateRGBSurface(0, dwTotalWidth, dwTotalHeight, 8, 0, 0, 0, 0);
	SDL_SetSurfacePalette(pBigSurface, PaletteCache[nPalette].pPal);

	for (int i = 0; i <= dwEnd - dwStart; i++)
	{
		DC6Frame* pFrame = &pCache->dc6.pFrames[dwStart + i];

		int dwBlitToX = (i % dwStitchCols) * 256;
		int dwBlitToY = floor(i / (float)dwStitchCols) * 255;

		SDL_Surface* pSmallSurface = SDL_CreateRGBSurface(0, pFrame->fh.dwWidth, pFrame->fh.dwHeight,
			8, 0, 0, 0, 0);
		SDL_SetSurfacePalette(pSmallSurface, PaletteCache[nPalette].pPal);

		memcpy(pSmallSurface->pixels, 
			DC6_GetPixelsAtFrame(&pCache->dc6, 0, i + dwStart, nullptr), 
			pFrame->fh.dwWidth * pFrame->fh.dwHeight);

		SDL_Rect dstRect = { dwBlitToX, dwBlitToY, pFrame->fh.dwWidth, pFrame->fh.dwHeight };
		SDL_Rect srcRect = { 0 , 1, pFrame->fh.dwWidth, pFrame->fh.dwHeight };
		SDL_BlitSurface(pSmallSurface, &srcRect, pBigSurface, &dstRect);
		SDL_FreeSurface(pSmallSurface);
	}

	pTexture = SDL_CreateTextureFromSurface(gpRenderer, pBigSurface);

	pCache->dwWidth = dwTotalWidth;
	pCache->dwHeight = dwTotalHeight;
	pCache->pTexture = pTexture;

	SDL_FreeSurface(pBigSurface);

	return tex;
}

/*
 *	Registers a tex_handle based on an animated DC6
 */
tex_handle Renderer_SDL_TextureFromAnimatedDC6(char* szDc6Path, char* szHandle, int nPalette)
{
	D2Palette* pPal = Pal_GetPalette(nPalette);
	bool bExists = false;
	tex_handle tex = Renderer_SDL_GetTextureInCache(szHandle);
	SDLHardwareTextureCacheItem* pCache;
	int bpp = 0;
	Uint32 dwRMask, dwGMask, dwBMask, dwAMask;

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_BGRA8888, &bpp, &dwRMask, &dwGMask, &dwBMask, &dwAMask);

	if (tex == INVALID_HANDLE)
	{
		return tex;
	}

	pCache = &TextureCache[tex];

	if (pCache->pTexture != nullptr)
	{
		return tex; // already been registered
	}

	DC6_LoadImage(szDc6Path, &pCache->dc6);
	pCache->bHasDC6 = true;

	DC6Image* pImg = &pCache->dc6;

	// Create the big texture that we will blit into
	SDL_Surface* pBigSurface = SDL_CreateRGBSurface(0, pImg->dwTotalWidth, pImg->dwTotalHeight, 
		bpp, dwRMask, dwGMask, dwBMask, dwAMask);

	// Add each frame into the surface
	DWORD dwCursorX = 0;
	DWORD dwCursorY = 0;

	for (int i = 0; i < pImg->header.dwDirections; i++)
	{
		DWORD dwDirectionHeight = 0;

		for (int j = 0; j < pImg->header.dwFrames; j++)
		{
			DWORD dwFrame = (i * pImg->header.dwFrames) + j;
			DC6Frame* pFrame = &pImg->pFrames[dwFrame];
			SDL_Rect s;
			SDL_Rect d;

			s.x = 0;
			s.y = 0;
			s.w = pFrame->fh.dwWidth;
			s.h = pFrame->fh.dwHeight;

			d.x = dwCursorX;
			d.y = dwCursorY;
			d.w = pFrame->fh.dwWidth;
			d.h = pFrame->fh.dwHeight;

			SDL_Surface* pSmallSurface =
				SDL_CreateRGBSurfaceFrom(DC6_GetPixelsAtFrame(pImg, i, j, nullptr),
					pFrame->fh.dwWidth, pFrame->fh.dwHeight,
					8, pFrame->fh.dwWidth, 0, 0, 0, 0);
			SDL_SetSurfacePalette(pSmallSurface, PaletteCache[nPalette].pPal);

			SDL_Surface* pConvertedSurface = SDL_ConvertSurfaceFormat(pSmallSurface, SDL_PIXELFORMAT_BGRA8888, 0);

			SDL_BlitSurface(pConvertedSurface, &s, pBigSurface, &d);
			SDL_FreeSurface(pConvertedSurface);
			SDL_FreeSurface(pSmallSurface);

			// Advance the cursor
			dwCursorX += pFrame->fh.dwWidth;
			if (pFrame->fh.dwHeight > dwDirectionHeight)
			{
				dwDirectionHeight = pFrame->fh.dwHeight;
			}
		}
		dwCursorX = 0;
		dwCursorY += dwDirectionHeight;
	}

	// Now that the big surface contains all the pixels, the last step here is to create an SDL_Texture from the surface
	pCache->dwWidth = pImg->dwTotalWidth;
	pCache->dwHeight = pImg->dwTotalHeight;
	pCache->pTexture = SDL_CreateTextureFromSurface(gpRenderer, pBigSurface);

	SDL_FreeSurface(pBigSurface);
	return tex;
}

/*
 *	Sets a texture's blend mode
 */
void Renderer_SDL_SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode)
{
	if (texture == INVALID_HANDLE)
	{
		return;
	}

	SDL_SetTextureBlendMode(TextureCache[texture].pTexture, (SDL_BlendMode)blendMode);
}

/*
 *	Creates an animation at the specified hash location
 */
static void Renderer_SDL_CreateAnimation(anim_handle anim, tex_handle texture, char* szHandle, DWORD dwStartingFrame)
{
	SDLHardwareAnimationCacheItem* pCache = &AnimCache[anim];
	DC6Image* pDC6 = &TextureCache[texture].dc6;
	DWORD dwCenterPos[64]{ 0 };

	D2_strncpyz(pCache->szHandleName, szHandle, CACHEHANDLE_LEN);
	pCache->texture = texture;
	pCache->dwFrame = dwStartingFrame;
	pCache->dwFrameCount = pDC6->header.dwDirections * pDC6->header.dwFrames;
	pCache->dwLastTick = SDL_GetTicks();

	// build frame data based on the information in the DC6
	DWORD dwCursorX = 0;
	DWORD dwCursorY = 0;

	for (int i = 0; i < pDC6->header.dwDirections; i++)
	{
		DWORD dwDirectionY = 0;

		for (int j = 0; j < pDC6->header.dwFrames; j++)
		{
			DWORD dwFramePos = (i * pDC6->header.dwFrames) + j;

			pCache->frames[dwFramePos].x = dwCursorX;
			pCache->frames[dwFramePos].y = dwCursorY;
			pCache->frames[dwFramePos].w = pDC6->pFrames[dwFramePos].fh.dwWidth;
			pCache->frames[dwFramePos].h = pDC6->pFrames[dwFramePos].fh.dwHeight;
			pCache->frames[dwFramePos].dwOffsetX = pDC6->pFrames[dwFramePos].fh.dwOffsetX;
			pCache->frames[dwFramePos].dwOffsetY = pDC6->pFrames[dwFramePos].fh.dwOffsetY;
			pCache->frames[dwFramePos].dwOffsetY += (pDC6->dwDirectionHeights[i] - pCache->frames[dwFramePos].h);
			
			dwCursorX += pDC6->pFrames[dwFramePos].fh.dwWidth;

			if (pDC6->pFrames[dwFramePos].fh.dwHeight > dwDirectionY)
			{
				dwDirectionY = pDC6->pFrames[dwFramePos].fh.dwHeight;
			}
		}

		dwCursorX = 0;
		dwCursorY += dwDirectionY;
		dwCenterPos[i] = dwDirectionY;
	}
}

/*
 *	Creates an animation instance from a texture
 */
anim_handle Renderer_SDL_RegisterAnimation(tex_handle texture, char* szHandle, DWORD dwStartingFrame)
{
	DWORD dwHash = D2_strhash(szHandle, CACHEHANDLE_LEN, MAX_SDL_ANIMCACHE_SIZE);
	DWORD dwIterations = 0;

	while (dwIterations < MAX_SDL_ANIMCACHE_SIZE)
	{
		if (AnimCache[dwHash].texture == INVALID_HANDLE)
		{
			Renderer_SDL_CreateAnimation(dwHash, texture, szHandle, dwStartingFrame);
			return dwHash;
		}
		else if (!D2_stricmp(AnimCache[dwHash].szHandleName, szHandle))
		{
			return dwHash;
		}

		dwHash++;
		dwHash %= MAX_SDL_ANIMCACHE_SIZE;

		dwIterations++;
	}

	return INVALID_HANDLE;
}

/*
 *	Destroys an animation instance
 */
void Renderer_SDL_DeregisterAnimation(anim_handle anim)
{
	SDLHardwareAnimationCacheItem* pCache = &AnimCache[anim];
	memset(pCache, 0, sizeof(SDLHardwareAnimationCacheItem));
	pCache->texture = INVALID_HANDLE;
}

/*
 *	Draws the texture by creating a render command.
 *	@author eezstreet
 */
void Renderer_SDL_DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (texture == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_DRAWTEXTURE;
	pCommand->DrawTexture.src.x = u;
	pCommand->DrawTexture.src.y = v;
	pCommand->DrawTexture.src.w = pCommand->DrawTexture.dst.w = w;
	pCommand->DrawTexture.src.h = pCommand->DrawTexture.dst.h = h;
	pCommand->DrawTexture.dst.x = x;
	pCommand->DrawTexture.dst.y = y;
	pCommand->DrawTexture.tex = texture;
	numDrawCommandsThisFrame++;
}

/*
 *	Draws specific texture frames by creating a render command.
 *	@author	eezstreet
 */
void Renderer_SDL_DrawTextureFrames(tex_handle texture, int x, int y, DWORD dwStartFrame, DWORD dwEndFrame)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (texture == INVALID_HANDLE)
	{
		return;
	}

	if (dwStartFrame > dwEndFrame)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_DRAWTEXTUREFRAMES;
	pCommand->DrawTextureFrames.tex = texture;
	pCommand->DrawTextureFrames.dwDstX = x;
	pCommand->DrawTextureFrames.dwDstY = y;
	pCommand->DrawTextureFrames.dwStart = dwStartFrame;
	pCommand->DrawTextureFrames.dwEnd = dwEndFrame;
	numDrawCommandsThisFrame++;
}

/*
 *	Draws one specific frame of a texture by creating a render command
 *	@author	eezstreet
 */
void Renderer_SDL_DrawTextureFrame(tex_handle texture, int x, int y, DWORD dwFrame)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (texture == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_DRAWTEXTUREFRAME;
	pCommand->DrawTextureFrame.tex = texture;
	pCommand->DrawTextureFrame.dwDstX = x;
	pCommand->DrawTextureFrame.dwDstY = y;
	pCommand->DrawTextureFrame.dwFrame = dwFrame;
	numDrawCommandsThisFrame++;
}

/*
 *	Keeps an animation going
 *	@author eezstreet
 */
void Renderer_SDL_Animate(anim_handle anim, DWORD dwFramerate, int x, int y)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (anim == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_ANIMATE;
	pCommand->Animate.anim = anim;
	pCommand->Animate.dwFramerate = dwFramerate;
	pCommand->Animate.dwX = x;
	pCommand->Animate.dwY = y;
	numDrawCommandsThisFrame++;
}

/*
 *	Sets the frame of an animation
 *	@author	eezstreet
 */
void Renderer_SDL_SetAnimFrame(anim_handle anim, DWORD dwFrame)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (anim == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_SETANIMFRAME;
	pCommand->SetAnimFrame.anim = anim;
	pCommand->SetAnimFrame.dwAnimFrame = dwFrame;
	numDrawCommandsThisFrame++;
}