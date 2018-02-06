#include "Renderer_SDL.hpp"
#include "LRUQueue.hpp"

///////////////////////////////////////////////////////////////////////
//
//	HARDWARE-ACCELERATED SDL RENDERER



//////////////////////////////
//
//	DCC Decompression

class SDLLRUItem : public LRUQueueItem
{
private:
	SDL_Texture** pTexture;

	DWORD dwDirectionW;
	DWORD dwDirectionH;

public:
	SDLLRUItem(handle itemHandle, int d);
	~SDLLRUItem();

	DCCDirection* pDirection;

	SDL_Texture* GetTextureForFrame(int nFrame)
	{
		return pTexture[nFrame];
	}

	DWORD GetDirectionWidth() { return dwDirectionW; }
	DWORD GetDirectionHeight() { return dwDirectionH; }
};

static SDL_Renderer* gpRenderer = nullptr;

static SDLPaletteEntry PaletteCache[PAL_MAX_PALETTES]{ 0 };

static SDLCommand gdrawCommands[MAX_SDL_DRAWCOMMANDS_PER_FRAME];
static DWORD numDrawCommandsThisFrame = 0;

// For DC6s
static SDLDC6CacheItem TextureCache[MAX_SDL_TEXTURECACHE_SIZE]{ 0 };
static SDLDC6AnimationCacheItem AnimCache[MAX_SDL_ANIMCACHE_SIZE]{ 0 };
static SDLFontCacheItem FontCache[MAX_SDL_FONTCACHE_SIZE]{ 0 };

// For DCCs - one LRU for each type
static LRUQueue<SDLLRUItem>* DCCLRU[ATYPE_MAX];

static SDL_Texture* gpRenderTexture = nullptr;

/////////////////////////////////////////////
//
//	DCC Decoding

/*
 *	Create a new SDL DCC LRU item by decompressing a preloaded DCC's direction, D.
 *	Special thanks to SVR, Paul Siramy, Bilian Belchev and Necrolis
 *	@author	eezstreet
 */
SDLLRUItem::SDLLRUItem(handle itemHandle, int d) : LRUQueueItem(itemHandle, d)
{
	// at this point, it's guaranteed that the DCC exists
	DCCFile* pFile = DCC::GetContents(itemHandle);
	DCCDirection* pDir = &pFile->directions[d];
	int n;

	if (d >= pFile->header.nNumberDirections)
	{	// tried to enter an invalid direction! don't do this!
		return;
	}

	pDirection = pDir;

	pTexture = new SDL_Texture*[pFile->header.dwFramesPerDirection];

	// Create a buffer containing the cells for this direction
	int nDirectionW = pDir->nMaxX - pDir->nMinX + 1;
	int nDirectionH = pDir->nMaxY - pDir->nMinY + 1;
	int nDirCellW = (nDirectionW >> 2) + 10;
	int nDirCellH = (nDirectionH >> 2) + 10;
	DWORD dwNumCellsThisDir = nDirCellW * nDirCellH;

	// Global cell buffer
	DCCCell** ppCellBuffer = new DCCCell*[dwNumCellsThisDir];
	memset(ppCellBuffer, 0, dwNumCellsThisDir * sizeof(DCCCell*));
	// Cell buffer for all frames
	DCCCell* pFrameCells[MAX_DCC_FRAMES];
	memset(pFrameCells, 0, sizeof(DCCCell*) * MAX_DCC_FRAMES);

	// Rewind all of the associated streams
	pDir->RewindAllStreams();

	// First part: iterate through the frames and get the colors
	for (int f = 0; f < pFile->header.dwFramesPerDirection; f++)
	{
		DCCFrame* pFrame = &pDir->frames[f];

		// Calculate the frame size, and number of cells in this frame
		int nFrameW = pFrame->dwWidth;
		int nFrameH = pFrame->dwHeight;
		int nFrameX = pFrame->nXOffset - pDir->nMinX;
		int nFrameY = pFrame->nYOffset - pDir->nMinY - nFrameH + 1;
		
		int nNumCellsW = DCC::GetCellCount(nFrameX, nFrameW);
		int nNumCellsH = DCC::GetCellCount(nFrameY, nFrameH);

		// Allocate cells
		DCCCell* pCell = pFrameCells[f] = new DCCCell[nNumCellsW * nNumCellsH];

		// Process cells left -> right / top -> bottom --SVR
		int nStartX = nFrameX >> 2;
		int nStartY = nFrameY >> 2;

		// Grab the four pixels (clrcode) color for each cell
		for (int y = nStartY; y < (nStartY + nNumCellsH); y++)
		{
			for (int x = nStartX; x < (nStartX + nNumCellsW); x++)
			{
				DCCCell* pCurCell = pCell++;
				DCCCell* pPrevCell = ppCellBuffer[(y * nDirCellW) + x];
				DWORD dwCLRMask = 0xF;

				*(DWORD*)(pCurCell->clrmap) = 0;

				// If we have a previous cell, read the contents of the EqualCellBitstream and ColorMask
				if (pPrevCell)
				{
					if (pDir->EqualCellStream != nullptr)
					{
						BYTE bit = 0;
						pDir->EqualCellStream->ReadBits(bit, 1);
						if (bit)
						{	// Skip if we read a '1' bit from EqualCells
							continue;
						}
					}
					if (pDir->PixelMaskStream != nullptr)
					{	// read the color mask
						pDir->PixelMaskStream->ReadBits(&dwCLRMask, 4);
					}
				}

				DWORD dwEncodingType = 0;
				DWORD dwCLRCode = 0;
				DWORD dwUnencoded = 0;
				DWORD dwLastColor = 0;
				DWORD dwTemp = 0;

				// Mask off the appropriate colors
				if (dwCLRMask != 0)
				{
					if (pDir->EncodingTypeStream != nullptr)
					{	// check the encoding type
						pDir->EncodingTypeStream->ReadBits(&dwEncodingType, 1);
					}

					for (n = 0; n < 4; n++)
					{	// read the colors in the mask
						if (dwCLRMask & (0x1 << n))
						{
							if (dwEncodingType != 0)
							{	// if encoding is 1, read it from the raw pixel stream
								pDir->RawPixelStream->ReadBits(&dwCLRCode, 8);
							}
							else
							{
								// read the difference from the pixel data and add it to the color
								do
								{
									pDir->PixelCodeDisplacementStream->ReadBits(&dwUnencoded, 4);
									dwCLRCode += dwUnencoded;
								} while (dwUnencoded == 15);
							}

							// Check to see if the same color was fetched.
							// If so, stop decoding (it's probably transparent)
							if (dwLastColor == dwCLRCode)
							{
								break;
							}

							dwTemp <<= 8;
							dwTemp |= dwCLRCode;
							dwLastColor = dwCLRCode;
						}
					}
				}

				// Merge previous colors
				for (n = 0; n < 4; n++)
				{
					if (dwCLRMask & (0x1 << n))
					{	// pop the current color bit
						pCurCell->clrmap[n] = (BYTE)(dwTemp & 0xFF);
						dwTemp >>= 8;
					}
					else
					{	// copy the previous color
						pCurCell->clrmap[n] = pPrevCell->clrmap[n];
					}
				}

				ppCellBuffer[(y * nDirCellW) + x] = pCurCell;
			}
		}
	}

	// Second part: build a bitmap based on the cells data
	// FIXME: segregate this code to being renderer specific instead of this whole function being renderer-specific
	BYTE* bitmap = new BYTE[nDirectionW * nDirectionH];
	SDL_Surface* pFrameSurf = nullptr;
	dwDirectionW = nDirectionW;
	dwDirectionH = nDirectionH;

	// ? clear all cells in the frame buffer list
	memset(ppCellBuffer, 0, sizeof(DCCCell*) * dwNumCellsThisDir);

	// Rewind the equal cell stream
	if (pDir->EqualCellStream != nullptr)
	{
		pDir->EqualCellStream->Rewind();
	}

	// Render each frame's cells onto the mini surface 
	for (int f = 0; f < pFile->header.dwFramesPerDirection; f++)
	{
		DCCFrame* pFrame = &pDir->frames[f];

		// Clear out the bitmap
		memset(bitmap, 0, nDirectionW * nDirectionH);

		// Calculate the frame size, and number of cells in this frame
		int nFrameW = pFrame->dwWidth;
		int nFrameH = pFrame->dwHeight;
		int nFrameX = pFrame->nXOffset - pDir->nMinX;
		int nFrameY = pFrame->nYOffset - pDir->nMinY - nFrameH + 1;

		int nNumCellsW = DCC::GetCellCount(nFrameX, nFrameW);
		int nNumCellsH = DCC::GetCellCount(nFrameY, nFrameH);

		int nStartX = nFrameX >> 2;
		int nStartY = nFrameY >> 2;

		int nFirstColumnW = 4 - (nFrameX & 3);
		int nFirstRowH = 4 - (nFrameY & 3);

		int nCountI = nFirstRowH;	// height counter
		int nCountJ;
		int nYPos = 0;
		int nXPos;

		DCCCell* pCells = pFrameCells[f];
		for (int y = nStartY; y < (nStartY + nNumCellsH); y++)
		{
			nXPos = 0;
			nCountJ = nFirstColumnW;

			if (y == ((nStartY + nNumCellsH) - 1))
			{	// If it's the last row, use the last height
				nCountI = nFrameH;
			}

			for (int x = nStartX; x < (nStartX + nNumCellsW); x++)
			{
				bool bTransparent = false;
				DCCCell* pCurCell = pCells++;
				DCCCell* pPrevCell = ppCellBuffer[(y * nDirCellW) + x];

				if (x == ((nStartX + nNumCellsW) - 1))
				{	// If it's the last column, use the last width
					nCountJ = nFrameW;
				}

				pCurCell->nH = nCountI;
				pCurCell->nW = nCountJ;
				pCurCell->nX = nFrameX + nXPos;
				pCurCell->nY = nFrameY + nYPos;

				// Check for equal cell
				if (pPrevCell && pDir->EqualCellStream != nullptr)
				{
					DWORD dwEqualCell = 0;
					pDir->EqualCellStream->ReadBits(&dwEqualCell, 1);

					if (dwEqualCell)
					{
						if (pPrevCell->nH == pCurCell->nH && pPrevCell->nW == pCurCell->nW)
						{	// same sized cell = it's definitely the same
							// check x/y - if they are not the same then we copy it
							if (pPrevCell->nX != pCurCell->nX || pPrevCell->nY != pCurCell->nY)
							{
								// source and destination rectangles
								int dY = pCurCell->nY;

								for (int i = pPrevCell->nY; i < pPrevCell->nY + pPrevCell->nH; i++)
								{
									int dX = pCurCell->nX;
									for (int j = pPrevCell->nX; j < pPrevCell->nX + pPrevCell->nW; j++)
									{
										bitmap[(dY * nDirectionW) + dX] = bitmap[(i * nDirectionW) + j];
										dX++;
									}
									dY++;
								}
							}

							ppCellBuffer[(y * nDirCellW) + x] = pCurCell;
							nXPos += nCountJ;
							nCountJ = 4;
							continue;
						}
						else
						{	// incongruent cell = it's definitely transparent
							bTransparent = true;
						}
					}
				}

				for (n = 0; n < 2; n++)
				{	// try to find a zero
					if (!pCurCell->clrmap[n])
					{
						break;
					}
				}

				// fill the cell
				if (bTransparent || !n)
				{	// if all of them are transparent, fill with 0s
					for (int i = nFrameY + nYPos; i < nFrameY + nYPos + nCountI; i++)
					{
						for (int j = nFrameX + nXPos; j < nFrameX + nXPos + nCountJ; j++)
						{
							bitmap[(i * nDirectionW) + j] = 0;
						}
					}
				}
				else
				{
					// Write the color pixels...one by one...
					for (int i = 0; i < nCountI; i++)
					{
						for (int j = 0; j < nCountJ; j++)
						{
							DWORD dwPixelData = 0;
							DWORD dwPixelPos = ((nFrameY + nYPos + i) * (nDirectionW)) + (nFrameX + nXPos + j);
							int nPalettePos;

							pDir->PixelCodeDisplacementStream->ReadBits(&dwPixelData, n);
							nPalettePos = pCurCell->clrmap[dwPixelData];
							bitmap[dwPixelPos] = pDir->nPixelValues[nPalettePos];
						}
					}
				}

				ppCellBuffer[(y * nDirCellW) + x] = pCurCell;
				nXPos += nCountJ;
				nCountJ = 4;
			}

			nYPos += nCountI;
			nCountI = 4;
		}

		if (pFrameSurf == nullptr)
		{
			pFrameSurf = SDL_CreateRGBSurfaceFrom(bitmap, nDirectionW, nDirectionH, 8, nDirectionW, 0, 0, 0, 0);
			SDL_SetSurfacePalette(pFrameSurf, PaletteCache[PAL_UNITS].pPal);
		}
		else
		{
			if (SDL_MUSTLOCK(pFrameSurf))
			{
				SDL_LockSurface(pFrameSurf);
			}

			memcpy(pFrameSurf->pixels, bitmap, nDirectionW * nDirectionH);

			if (SDL_MUSTLOCK(pFrameSurf))
			{
				SDL_UnlockSurface(pFrameSurf);
			}
		}
#if 0
		// save frame as BMP
		char testFrameName[32];
		snprintf(testFrameName, 32, "test%04d.bmp", f);
		SDL_SaveBMP(pFrameSurf, testFrameName);
#endif

		// Create a new texture from this surface, for this frame		
		pTexture[f] = SDL_CreateTextureFromSurface(gpRenderer, pFrameSurf);
	}

	if (pFrameSurf != nullptr)
	{
		// remove the surface, we don't need it anymore
		SDL_FreeSurface(pFrameSurf);
	}

	// Delete all of the cell data, we don't need it anymore
	for (int f = 0; f < pFile->header.dwFramesPerDirection; f++)
	{
		delete pFrameCells[f];
	}

	delete[] bitmap;
	delete[] ppCellBuffer;
}

/*
 *	Deletes all data associated with this LRU item (so deletes the bitmaps etc)
 *	@author	eezstreet
 */
SDLLRUItem::~SDLLRUItem()
{
	DCCFile* pFile = DCC::GetContents(itemHandle);
	if (pFile == nullptr)
	{
		return;
	}

	for (int i = 0; i < pFile->header.dwFramesPerDirection; i++)
	{
		SDL_DestroyTexture(pTexture[i]);
	}

	delete[] pTexture;
}

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
	SDLDC6CacheItem* pCache = &TextureCache[pCmd->DrawTextureFrames.tex];
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
	SDLDC6CacheItem* pCache = &TextureCache[pCmd->DrawTextureFrame.tex];
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
	SDLDC6AnimationCacheItem* pCache = &AnimCache[pCmd->Animate.anim];
	DWORD dwAnimRate = pCmd->Animate.dwFramerate;
	DWORD dwOriginalFrame = pCache->dwFrame;
	SDL_Rect s{ 0 };
	SDL_Rect d{ 0 };

	if (pCache->dwFrameCount == 0)
	{
		return;
	}

	// advance (and limit) frame
	if (dwAnimRate > 0)
	{
		pCache->dwFrame += (dwTicks - pCache->dwLastTick) / (1000 / dwAnimRate);
		pCache->dwFrame %= pCache->dwFrameCount;
	}
	

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

	if (pCache->bKeyframePresent)
	{
		if (dwOriginalFrame != pCache->dwFrame)
		{
			if (pCache->dwFrame == pCache->nKeyframeFrame)
			{
				pCache->keyframeCallback(pCmd->Animate.anim, pCache->nExtraInt);
			}
		}
	}

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
	AnimCache[pCmd->SetAnimFrame.anim].dwLastTick = SDL_GetTicks();
}

/*
 *	Backend - Draws a bit of text
 */
static void RB_DrawText(SDLCommand* pCmd)
{
	DWORD dwTextWidth = 0;
	DWORD dwTextHeight = 0;
	SDLFontCacheItem* pCache;
	char c;
	size_t len;
	DWORD dwOffsetX = 0, dwOffsetY = 0;

	pCache = &FontCache[pCmd->DrawText.font];
	len = D2Lib::qstrlen(pCmd->DrawText.text);

	if (len <= 0)
	{	// no sense in drawing a blank string
		return;
	}

	// We need to acquire the width and height of the text that's being drawn ..
	// but only in alignments that aren't left/top !
	if (pCmd->DrawText.horzAlign != ALIGN_LEFT || pCmd->DrawText.vertAlign != ALIGN_TOP)
	{
		for (int i = 0; i < len; i++)
		{
			TBLFontGlyph* pGlyph;

			c = (char)pCmd->DrawText.text[i];
			pGlyph = &pCache->pFontData[0]->glyphs[c];	// LATINHACK

			dwTextWidth += pGlyph->nWidth;
		}

		dwTextHeight = pCache->dc6->dwTotalHeight;

		if (pCmd->DrawText.horzAlign == ALIGN_CENTER)
		{
			pCmd->DrawText.x += (pCmd->DrawText.w / 2) - (dwTextWidth / 2);
		}
		else if (pCmd->DrawText.horzAlign == ALIGN_RIGHT)
		{
			pCmd->DrawText.x += pCmd->DrawText.w - dwTextWidth;
		}

		if (pCmd->DrawText.vertAlign == ALIGN_CENTER)
		{
			pCmd->DrawText.y += (pCmd->DrawText.h / 2);
			pCmd->DrawText.y -= dwTextHeight / 2 - 2;
		}
		else if (pCmd->DrawText.vertAlign == ALIGN_BOTTOM)
		{
			pCmd->DrawText.y += pCmd->DrawText.h - dwTextHeight;
		}
	}

	// Preprocess the string (transform newlines and color codes).... TODO

	// Draw the string by iterating through each character
	for (int i = 0; i < len; i++)
	{
		TBLFontGlyph* pGlyph;
		DWORD dwHeight;

		c = (char)pCmd->DrawText.text[i];
		pGlyph = &pCache->pFontData[0]->glyphs[c];	// LATINHACK
		dwHeight = pCache->dc6->dwTotalHeight;

		SDL_Rect s{ 
			(int)pGlyph->dwUnknown4, 
			(int)0, 
			(int)pGlyph->nWidth, 
			(int)dwHeight};
		SDL_Rect d{ 
			pCmd->DrawText.x + (int)dwOffsetX, pCmd->DrawText.y + (int)dwOffsetY, 
				pGlyph->nWidth, (int)dwHeight };

		SDL_RenderCopy(gpRenderer, pCache->pTexture, &s, &d);

		dwOffsetX += pGlyph->nWidth;
	}
}

/*
 *	Backend - alpha modulate a texture
 */
static void RB_AlphaModulateTexture(SDLCommand* pCmd)
{
	tex_handle texture = pCmd->AlphaModulate.texture;
	int nAlpha = pCmd->AlphaModulate.nAlpha;

	SDL_SetTextureAlphaMod(TextureCache[texture].pTexture, nAlpha);
}

/*
 *	Backend - color modulate a texture
 */
static void RB_ColorModulateTexture(SDLCommand* pCmd)
{
	tex_handle texture = pCmd->ColorModulate.texture;
	int nRed = pCmd->ColorModulate.nRed;
	int nGreen = pCmd->ColorModulate.nGreen;
	int nBlue = pCmd->ColorModulate.nBlue;

	SDL_SetTextureColorMod(TextureCache[texture].pTexture, nRed, nGreen, nBlue);
}

/*
 *	Backend - alpha modulate a font
 */
static void RB_AlphaModulateFont(SDLCommand* pCmd)
{
	font_handle font = pCmd->AlphaModulate.texture;
	int nAlpha = pCmd->AlphaModulate.nAlpha;

	SDL_SetTextureAlphaMod(FontCache[font].pTexture, nAlpha);
}

/*
 *	Backend - color modulate a font
 */
static void RB_ColorModulateFont(SDLCommand* pCmd)
{
	font_handle font = pCmd->ColorModulate.texture;
	int nRed = pCmd->ColorModulate.nRed;
	int nGreen = pCmd->ColorModulate.nGreen;
	int nBlue = pCmd->ColorModulate.nBlue;

	SDL_SetTextureColorMod(FontCache[font].pTexture, nRed, nGreen, nBlue);
}

/*
 *	Backend - Draw a rectangle
 */
static void RB_DrawRectangle(SDLCommand* pCmd)
{
	SDLDrawRectangleCommand* pRCmd = &pCmd->DrawRectangle;
	SDL_Rect rect{ pRCmd->x, pRCmd->y, pRCmd->w, pRCmd->h };

	SDL_SetRenderDrawColor(gpRenderer, pRCmd->r, pRCmd->g, pRCmd->b, pRCmd->a);
	SDL_RenderDrawRect(gpRenderer, &rect);
	SDL_RenderFillRect(gpRenderer, &rect);
	SDL_SetRenderDrawColor(gpRenderer, 0, 0, 0, 255);
}

/*
 *	Continues the animation on a token instance
 *	@author	eezstreet
 */
static void RB_ContinueTokenInstanceAnimation(AnimTokenInstance* pInstance, COFFile* pCOFFile)
{
	DWORD dwCurrentTime = SDL_GetTicks();

	if (pCOFFile->header.nFPS == 0)
	{
		return; // There is no reason to be animating at all.
	}

	pInstance->currentFrame = dwCurrentTime * pCOFFile->header.nFPS / 40;
	pInstance->currentFrame %= (pCOFFile->header.nFrames << 8);
	pInstance->currentFrame >>= 8;
}

/*
 *	Backend - Draw an anim token instance
 */
static void RB_DrawTokenInstance(SDLCommand* pCmd)
{
	SDLDrawTokenInstanceCommand* pTCmd = &pCmd->DrawToken;
	AnimTokenInstance* pInstance = TokenInstance::GetTokenInstanceData(pTCmd->handle);
	cof_handle currentCOF;
	COFFile* pCOFFile;
	LRUQueue<SDLLRUItem>* pQueue;

	if (pInstance == nullptr || !pInstance->bInUse || !pInstance->bActive)
	{
		// bad or inactive instance, don't do anything
		return;
	}

	currentCOF = Token::GetCOFData(pInstance->currentHandle, pInstance->currentMode);
	if (currentCOF == INVALID_HANDLE)
	{	// no COF data. probably invalid mode
		return;
	}
	pCOFFile = COF::GetFileData(currentCOF);
	if (pCOFFile == nullptr)
	{	// bad COF here
		return;
	}

	RB_ContinueTokenInstanceAnimation(pInstance, pCOFFile);

	// iterate through all components
	switch (pInstance->tokenType)
	{
		case TOKEN_CHAR:
			pQueue = DCCLRU[ATYPE_CHAR];
			break;
		case TOKEN_OBJECT:
			pQueue = DCCLRU[ATYPE_OBJECT];
			break;
		case TOKEN_MONSTER:
			pQueue = DCCLRU[ATYPE_MONSTER];
			break;
	}
	for (int i = COMP_MAX-1; i >= 0; i--)
	{
		anim_handle curAnim = pInstance->componentAnims[pInstance->currentMode][i];
		SDLLRUItem* pItem;

		if (curAnim == INVALID_HANDLE)
		{
			continue; // nothing in this component
		}

		pItem = pQueue->QueryItem(curAnim, pInstance->currentDirection);
		Log_ErrorAssert(pItem != nullptr);
		
		// render it!!
		SDL_Texture* pTexture = pItem->GetTextureForFrame((int)pInstance->currentFrame);
		DWORD dwWidth = pItem->GetDirectionWidth();
		DWORD dwHeight = pItem->GetDirectionHeight();

		// The destination rectangle is oriented from the upper left corner, but whenever we do a draw call,
		// we are orienting from the "base point" of the token's DCC files. So we need to correct that.
		SDL_Rect d{
			pTCmd->x,
			pTCmd->y,
			(int)dwWidth,
			(int)dwHeight,
		};
		d.x -= (pItem->pDirection->frames[pInstance->currentFrame].nMinX - pItem->pDirection->nMinX);
		d.x += pItem->pDirection->frames[pInstance->currentFrame].nXOffset;
		d.y -= (pItem->pDirection->frames[pInstance->currentFrame].nMinY - pItem->pDirection->nMinY);
		d.y += pItem->pDirection->frames[pInstance->currentFrame].nYOffset;
		d.y -= pItem->pDirection->frames[pInstance->currentFrame].dwHeight - 1;
		SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(gpRenderer, pTexture, nullptr, &d);

#if 0
		// debug: draw a rectangle around where the frames are
		d.x += (pItem->pDirection->frames[pInstance->currentFrame].nMinX - pItem->pDirection->nMinX);
		d.y += (pItem->pDirection->frames[pInstance->currentFrame].nMinY - pItem->pDirection->nMinY);
		d.w = pItem->pDirection->frames[pInstance->currentFrame].dwWidth;
		d.h = pItem->pDirection->frames[pInstance->currentFrame].dwHeight;
		SDL_SetRenderDrawColor(gpRenderer, 128, 128, 255, 255);
		SDL_RenderDrawRect(gpRenderer, &d);

		// debug: draw a small red box indicating the origin
		d.x = pTCmd->x;
		d.y = pTCmd->y;
		d.w = 4;
		d.h = 4;
		SDL_SetRenderDrawColor(gpRenderer, 255, 0, 0, 255);
		SDL_RenderDrawRect(gpRenderer, &d);
#endif
	}
}

/*
 *
 *	Backend - All functions enumerated
 *
 */
static RenderProcessCommand RenderingCommands[RCMD_MAX] = {
	RB_DrawTexture,
	RB_DrawTextureFrames,
	RB_DrawTextureFrame,
	RB_Animate,
	RB_SetAnimationFrame,
	RB_DrawText,
	RB_AlphaModulateTexture,
	RB_ColorModulateTexture,
	RB_AlphaModulateFont,
	RB_ColorModulateFont,
	RB_DrawRectangle,
	RB_DrawTokenInstance,
};


/*
 *	Kills all font handles
 */
void Renderer_SDL_DeregisterAllFonts()
{
	for (int i = 0; i < MAX_SDL_FONTCACHE_SIZE; i++)
	{
		if (FontCache[i].pTexture != nullptr)
		{
			SDL_DestroyTexture(FontCache[i].pTexture);
		}
		if (FontCache[i].dc6[0].pPixels != nullptr)
		{
			free(FontCache[i].dc6[0].pPixels);
		}
		if (FontCache[i].dc6[0].pFrames != nullptr)
		{
			free(FontCache[i].dc6[0].pFrames);
		}
	}

	memset(FontCache, 0, sizeof(SDLFontCacheItem) * MAX_SDL_FONTCACHE_SIZE);
}

/*
 *	Clears out the texture cache
 */
void Renderer_SDL_ClearTextureCache()
{
	for (int i = 0; i < MAX_SDL_TEXTURECACHE_SIZE; i++)
	{
		SDLDC6CacheItem* pCache = &TextureCache[i];
		if (pCache->pTexture != nullptr)
		{
			SDL_DestroyTexture(pCache->pTexture);
		}
		if (pCache->bHasDC6)
		{
			DC6::UnloadImage(&pCache->dc6);
		}
		memset(pCache, 0, sizeof(SDLDC6CacheItem));
	}
}

/*
 *	Sets up the LRUs
 *	@author	eezstreet
 */
static DWORD LRUSizes[ATYPE_MAX] = { LRUSIZE_CHARS, LRUSIZE_MONSTERS, LRUSIZE_OBJECTS, LRUSIZE_MISSILES, LRUSIZE_OVERLAYS };
void Renderer_SDL_InitLRUs()
{
	for (int i = 0; i < ATYPE_MAX; i++)
	{
		DCCLRU[i] = new LRUQueue<SDLLRUItem>(LRUSizes[i]);
	}
}

/*
 *	Clears out the LRUs
 *	@author	eezstreet
 */
void Renderer_SDL_ClearLRUs()
{
	for (int i = 0; i < ATYPE_MAX; i++)
	{
		delete DCCLRU[i];
	}
}

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
		D2Palette* pPal = Pal::GetPalette(i);

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
	memset(AnimCache, 0, sizeof(SDLDC6AnimationCacheItem) * MAX_SDL_ANIMCACHE_SIZE);
	for (int i = 0; i < MAX_SDL_ANIMCACHE_SIZE; i++)
	{
		AnimCache[i].texture = INVALID_HANDLE;
	}

	gpRenderTexture = SDL_CreateTexture(gpRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);

	// Create LRUs
	Renderer_SDL_InitLRUs();
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
	Renderer_SDL_DeregisterAllFonts();
	Renderer_SDL_ClearLRUs();
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
	DWORD dwTextureHash = D2Lib::strhash(szHandleName, CACHEHANDLE_LEN, MAX_SDL_TEXTURECACHE_SIZE);

	while (dwChecked < MAX_SDL_TEXTURECACHE_SIZE)
	{
		if (!D2Lib::stricmp(TextureCache[dwTextureHash].szHandleName, szHandleName))
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
	SDLDC6CacheItem* pCache;

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
		DC6::UnloadImage(&pCache->dc6);
	}
	memset(pCache, 0, sizeof(SDLDC6CacheItem));
}

/*
 *	Registers a tex_handle based on a stitched-together DC6. The DC6 is not animated.
 */
tex_handle Renderer_SDL_TextureFromDC6(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette)
{
	D2Palette* pPal = Pal::GetPalette(nPalette);
	bool bExists = false;
	tex_handle tex = Renderer_SDL_GetTextureInCache(szHandle);
	SDLDC6CacheItem* pCache;
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

	D2Lib::strncpyz(pCache->szHandleName, szHandle, 32);

	DC6::LoadImage(szDc6Path, &pCache->dc6);
	pCache->bHasDC6 = true;

	// Calculate how wide it should be
	DWORD dwStitchRows = 0;
	DWORD dwStitchCols = 0;
	DWORD dwTotalWidth = 0;
	DWORD dwTotalHeight = 0;

	DC6::StitchStats(&pCache->dc6, dwStart, dwEnd, &dwStitchCols, &dwStitchRows, &dwTotalWidth, &dwTotalHeight);

	pBigSurface = SDL_CreateRGBSurface(0, dwTotalWidth, dwTotalHeight, 8, 0, 0, 0, 0);
	SDL_SetSurfacePalette(pBigSurface, PaletteCache[nPalette].pPal);

	for (int i = 0; i <= dwEnd - dwStart; i++)
	{
		DC6Frame* pFrame = &pCache->dc6.pFrames[dwStart + i];

		int dwBlitToX = (i % dwStitchCols) * 256;
		int dwBlitToY = (int)floor(i / (float)dwStitchCols) * 255;

		SDL_Surface* pSmallSurface = SDL_CreateRGBSurface(0, pFrame->fh.dwWidth, pFrame->fh.dwHeight + 1,
			8, 0, 0, 0, 0);
		SDL_SetSurfacePalette(pSmallSurface, PaletteCache[nPalette].pPal);

		memcpy(pSmallSurface->pixels, 
			DC6::GetPixelsAtFrame(&pCache->dc6, 0, i + dwStart, nullptr), 
			pFrame->fh.dwWidth * (pFrame->fh.dwHeight + 1));

		SDL_Rect dstRect = { (int)dwBlitToX, (int)dwBlitToY, (int)pFrame->fh.dwWidth, (int)pFrame->fh.dwHeight + 1 };
		SDL_Rect srcRect = { 0 , 0, (int)pFrame->fh.dwWidth, (int)pFrame->fh.dwHeight };
		SDL_BlitSurface(pSmallSurface, &srcRect, pBigSurface, &dstRect);
		SDL_FreeSurface(pSmallSurface);
	}

	if (!D2Lib::stricmp(szHandle, "textbox"))
	{
		SDL_SaveBMP(pBigSurface, "textbox.bmp");
	}

	pTexture = SDL_CreateTextureFromSurface(gpRenderer, pBigSurface);

	pCache->dwWidth = dwTotalWidth;
	pCache->dwHeight = dwTotalHeight;
	pCache->pTexture = pTexture;

	SDL_FreeSurface(pBigSurface);
	DC6::FreePixels(&pCache->dc6);

	return tex;
}

/*
 *	Registers a tex_handle based on an animated DC6
 */
tex_handle Renderer_SDL_TextureFromAnimatedDC6(char* szDc6Path, char* szHandle, int nPalette)
{
	D2Palette* pPal = Pal::GetPalette(nPalette);
	bool bExists = false;
	tex_handle tex = Renderer_SDL_GetTextureInCache(szHandle);
	SDLDC6CacheItem* pCache;
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

	D2Lib::strncpyz(pCache->szHandleName, szHandle, 32);

	DC6::LoadImage(szDc6Path, &pCache->dc6);
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
				SDL_CreateRGBSurfaceFrom(DC6::GetPixelsAtFrame(pImg, i, j, nullptr),
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
 *	Determines if a point is within an animation's pixels.
 */
bool Renderer_SDL_PixelPerfectDetect(anim_handle anim, int nSrcX, int nSrcY, int nDrawX, int nDrawY, bool bAllowAlpha)
{
	SDLDC6AnimationCacheItem* pAnimCache;
	SDLDC6CacheItem* pTexCache;
	int nDrawWidth;
	int nDrawHeight;
	DC6Frame* pFrame;
	int nOffsetX, nOffsetY;
	BYTE* pPixels;

	if (anim == INVALID_HANDLE)
	{
		return false;
	}
	pAnimCache = &AnimCache[anim];

	if (pAnimCache->texture == INVALID_HANDLE)
	{
		return false;
	}
	pTexCache = &TextureCache[pAnimCache->texture];

	if (pTexCache->bHasDC6)
	{
		pFrame = &pTexCache->dc6.pFrames[pAnimCache->dwFrame];
		nDrawX += pFrame->fh.dwOffsetX;
		nDrawY += pFrame->fh.dwOffsetY;
		nDrawWidth = pFrame->fh.dwWidth;
		nDrawHeight = pFrame->fh.dwHeight;
		nOffsetX = nSrcX - nDrawX;
		nOffsetY = nSrcY - nDrawY;

		if (nOffsetX > nDrawWidth || nOffsetX < 0 ||
			nOffsetY > nDrawHeight || nOffsetY < 0)
		{
			return false;
		}

		if (bAllowAlpha)
		{
			// not actually pixel perfect collision but w/e
			return true;
		}

		pPixels = DC6::GetPixelsAtFrame(&pTexCache->dc6, 0, pAnimCache->dwFrame, nullptr);
		if (pPixels[(nOffsetY * pFrame->fh.dwWidth) + nOffsetX] != 0)
		{	// not a transparent pixel
			return true;
		}
	}

	return false;
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
 *	Gets stats about a texture
 */
void Renderer_SDL_PollTexture(tex_handle texture, DWORD* dwWidth, DWORD* dwHeight)
{
	if (texture == INVALID_HANDLE)
	{
		return;
	}

	if (dwWidth != nullptr)
	{
		*dwWidth = TextureCache[texture].dwWidth;
	}
	
	if (dwHeight != nullptr)
	{
		*dwHeight = TextureCache[texture].dwHeight;
	}
}

/*
 *	Creates an animation at the specified hash location
 */
static void Renderer_SDL_CreateAnimation(anim_handle anim, tex_handle texture, char* szHandle, DWORD dwStartingFrame)
{
	SDLDC6AnimationCacheItem* pCache = &AnimCache[anim];
	DC6Image* pDC6 = &TextureCache[texture].dc6;
	DWORD dwCenterPos[64]{ 0 };

	D2Lib::strncpyz(pCache->szHandleName, szHandle, CACHEHANDLE_LEN);
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
			Log_WarnAssert(dwFramePos < MAX_SDL_ANIM_FRAMES);

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
anim_handle Renderer_SDL_RegisterDC6Animation(tex_handle texture, char* szHandle, DWORD dwStartingFrame)
{
	DWORD dwHash = D2Lib::strhash(szHandle, CACHEHANDLE_LEN, MAX_SDL_ANIMCACHE_SIZE);
	DWORD dwIterations = 0;

	while (dwIterations < MAX_SDL_ANIMCACHE_SIZE)
	{
		if (AnimCache[dwHash].texture == INVALID_HANDLE)
		{
			Renderer_SDL_CreateAnimation(dwHash, texture, szHandle, dwStartingFrame);
			return dwHash;
		}
		else if (!D2Lib::stricmp(AnimCache[dwHash].szHandleName, szHandle))
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
	SDLDC6AnimationCacheItem* pCache = &AnimCache[anim];
	memset(pCache, 0, sizeof(SDLDC6AnimationCacheItem));
	pCache->texture = INVALID_HANDLE;
}

/*
 *	Adds a keyframe to an animation
 */
void Renderer_SDL_AddAnimKeyframe(anim_handle anim, int nFrame, AnimKeyframeCallback callback, int nExtraInt)
{
	if (anim == INVALID_HANDLE)
	{
		return;
	}

	AnimCache[anim].bKeyframePresent = true;
	AnimCache[anim].nKeyframeFrame = nFrame;
	AnimCache[anim].keyframeCallback = callback;
	AnimCache[anim].nExtraInt = nExtraInt;
}

/*
 *	Removes a keyframe from an animation
 */
void Renderer_SDL_RemoveAnimKeyframe(anim_handle anim)
{
	if (anim == INVALID_HANDLE)
	{
		return;
	}

	AnimCache[anim].bKeyframePresent = false;
}

/*
 *	Gets the number of frames in an animation
 */
DWORD Renderer_SDL_GetAnimFrameCount(anim_handle anim)
{
	if (anim == INVALID_HANDLE)
	{
		return 0;
	}

	return AnimCache[anim].dwFrameCount;
}

/*
 *	Creates a font instance
 */
font_handle Renderer_SDL_RegisterFont(char* szFontName)
{
	font_handle handle = D2Lib::strhash(szFontName, CACHEHANDLE_LEN, MAX_SDL_FONTCACHE_SIZE);
	DWORD dwHashTries = 0;
	char filename[MAX_D2PATH]{ 0 };
	tbl_handle tbl;
	SDL_Surface* pBigSurface;
	SDLFontCacheItem* pCache;
	int bpp;
	Uint32 dwRMask, dwGMask, dwBMask, dwAMask;
	DWORD dwXCounter = 0;
	
	// Find a free hash table entry
	while (dwHashTries < MAX_SDL_FONTCACHE_SIZE)
	{
		if (!D2Lib::stricmp(FontCache[handle].szHandleName, szFontName))
		{	// we already registered this font? return it
			return handle;
		}
		else if (FontCache[handle].szHandleName[0] == '\0')
		{
			break;
		}

		handle++;
		handle %= MAX_SDL_FONTCACHE_SIZE;

		dwHashTries++;
	}

	if (dwHashTries >= MAX_SDL_FONTCACHE_SIZE)
	{	// maybe couldn't find it
		return INVALID_HANDLE;
	}

	/*
	 *	OK LISTEN UP
	 *	For Latin fonts we only load two files (the DC6 and the TBL files themselves - TBL goes in slot 0)
	 *
	 *	I haven't coded support here for non-Latin fonts. If someone can find either d2delta.mpq or d2kfixup.mpq,
	 *	I might be able to figure out how those are rendered. For now, Latin we go!
	 */
	
	// Register the font TBL file
	pCache = &FontCache[handle];
	tbl = TBLFont::RegisterFont(szFontName);
	pCache->pFontData[0] = TBLFont::GetPointerFromHandle(tbl);
	D2Lib::strncpyz(pCache->szHandleName, szFontName, 32);

	// Load the DC6 file
	snprintf(filename, MAX_D2PATH, "data\\local\\FONT\\%s\\%s.dc6", GAME_CHARSET, szFontName);
	DC6::LoadImage(filename, &pCache->dc6[0]);

	// Create the big surface that we need to blit into
	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_BGRA8888, &bpp, &dwRMask, &dwGMask, &dwBMask, &dwAMask);
	pBigSurface = SDL_CreateRGBSurface(0, pCache->dc6[0].dwTotalWidth, pCache->dc6->dwTotalHeight, 
		32, dwRMask, dwGMask, dwBMask, dwAMask);

	// Blit each freaking sprite into the big font surface
	// Idea: maybe do this only on the first load, and save in a more convenient format elsewhere?
	// Seems a bit crummy to make this renderer-specific behavior though.
	for (int i = 0; i < pCache->dc6[0].header.dwFrames; i++)
	{
		DWORD dwFrameWidth = pCache->dc6[0].pFrames[i].fh.dwWidth;
		DWORD dwFrameHeight = pCache->dc6[0].pFrames[i].fh.dwHeight;
		SDL_Surface* pTinySurface = SDL_CreateRGBSurfaceFrom(DC6::GetPixelsAtFrame(&pCache->dc6[0], 0, i, nullptr),
			dwFrameWidth, dwFrameHeight, 8, dwFrameWidth, 0, 0, 0, 0);
		
		SDL_Rect dst{ (int)dwXCounter, -1, (int)dwFrameWidth, (int)dwFrameHeight };

		// Every font uses units palette.
		SDL_SetSurfacePalette(pTinySurface, PaletteCache[PAL_UNITS].pPal);

		// Blit the actual surface after making it
		SDL_BlitSurface(pTinySurface, nullptr, pBigSurface, &dst);
		SDL_FreeSurface(pTinySurface);

		// little hack here to make this run better later on
		pCache->pFontData[0]->glyphs[i].dwUnknown4 = dwXCounter;

		dwXCounter += dwFrameWidth;
	}

	// Create the texture from the big surface
	pCache->pTexture = SDL_CreateTextureFromSurface(gpRenderer, pBigSurface);
	SDL_FreeSurface(pBigSurface);

	// There is no reason we should be blending a font in anything besides alpha mode
	SDL_SetTextureBlendMode(pCache->pTexture, SDL_BLENDMODE_BLEND);

	return handle;
}

/*
 *	Gets the frame that an animation is on
 */
DWORD Renderer_SDL_GetAnimFrame(anim_handle anim)
{
	if (anim == INVALID_HANDLE)
	{
		return 0;
	}

	return AnimCache[anim].dwFrame;
}

/*
 *	Deletes a font handle
 */
void Renderer_SDL_DeregisterFont(font_handle font)
{
	SDLFontCacheItem* pCache;

	if (font == INVALID_HANDLE)
	{	// not valid?
		return;
	}

	pCache = &FontCache[font];

	if (pCache->szHandleName[0] == '\0')
	{	// never registered in the first place?
		return;
	}

	memset(pCache->szHandleName, 0, CACHEHANDLE_LEN);
	pCache->pFontData[0] = nullptr;
	pCache->pFontData[1] = nullptr;
	SDL_DestroyTexture(pCache->pTexture);
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

/*
 *	Renders a bit of text
 *	@author	eezstreet
 */
void Renderer_SDL_DrawText(font_handle font, char16_t* szText, int x, int y, int w, int h,
	D2TextAlignment alignHorz, D2TextAlignment alignVert)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (font == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_DRAWTEXT;
	pCommand->DrawText.font = font;
	pCommand->DrawText.x = x;
	pCommand->DrawText.y = y;
	pCommand->DrawText.w = w;
	pCommand->DrawText.h = h;
	pCommand->DrawText.horzAlign = alignHorz;
	pCommand->DrawText.vertAlign = alignVert;
	D2Lib::qstrncpyz(pCommand->DrawText.text, szText, MAX_TEXT_DRAW_LINE);
	numDrawCommandsThisFrame++;
}

/*
 *	Alter the alpha modulation on a texture
 */
void Renderer_SDL_AlphaModulateTexture(tex_handle texture, int nAlpha)
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
	pCommand->cmdType = RCMD_ALPHAMODULATE;
	pCommand->AlphaModulate.texture = texture;
	pCommand->AlphaModulate.nAlpha = nAlpha;
	numDrawCommandsThisFrame++;
}

/*
 *	Alter the color modulation on a texture
 */
void Renderer_SDL_ColorModulateTexture(tex_handle texture, int nRed, int nGreen, int nBlue)
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
	pCommand->cmdType = RCMD_COLORMODULATE;
	pCommand->ColorModulate.texture = texture;
	pCommand->ColorModulate.nRed = nRed;
	pCommand->ColorModulate.nGreen = nGreen;
	pCommand->ColorModulate.nBlue = nBlue;
	numDrawCommandsThisFrame++;
}

/*
 *	Alter the alpha modulation on a font
 */
void Renderer_SDL_AlphaModulateFont(font_handle font, int nAlpha)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (font == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_ALPHAMODULATEFONT;
	pCommand->AlphaModulate.texture = font;
	pCommand->AlphaModulate.nAlpha = nAlpha;
	numDrawCommandsThisFrame++;
}

/*
 *	Alter the color modulation on a font
 */
void Renderer_SDL_ColorModulateFont(font_handle font, int nRed, int nGreen, int nBlue)
{
	if (numDrawCommandsThisFrame >= MAX_SDL_DRAWCOMMANDS_PER_FRAME)
	{
		return;
	}

	if (font == INVALID_HANDLE)
	{
		return;
	}

	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_COLORMODULATEFONT;
	pCommand->ColorModulate.texture = font;
	pCommand->ColorModulate.nRed = nRed;
	pCommand->ColorModulate.nGreen = nGreen;
	pCommand->ColorModulate.nBlue = nBlue;
	numDrawCommandsThisFrame++;
}

/*
 *	Draws a rectangle
 */
void Renderer_SDL_DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_DRAWRECTANGLE;
	pCommand->DrawRectangle.x = x;
	pCommand->DrawRectangle.y = y;
	pCommand->DrawRectangle.w = w;
	pCommand->DrawRectangle.h = h;
	pCommand->DrawRectangle.r = r;
	pCommand->DrawRectangle.g = g;
	pCommand->DrawRectangle.b = b;
	pCommand->DrawRectangle.a = a;
	numDrawCommandsThisFrame++;
}

/*
 *	Start drawing a token instance
 *	@author	eezstreet
 */
void Renderer_SDL_DrawTokenInstance(anim_handle instance, int x, int y, int translvl, int palette)
{
	// Add a drew command
	SDLCommand* pCommand = &gdrawCommands[numDrawCommandsThisFrame];
	pCommand->cmdType = RCMD_DRAWTOKENINSTANCE;
	pCommand->DrawToken.handle = instance;
	pCommand->DrawToken.x = x;
	pCommand->DrawToken.y = y;
	pCommand->DrawToken.translvl = translvl;
	pCommand->DrawToken.palette = palette;
	numDrawCommandsThisFrame++;
}