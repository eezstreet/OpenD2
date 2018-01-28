#include "Renderer.hpp"

/*
 *	The renderer in OpenD2 is significantly different from the one in retail Diablo 2.
 *
 *	For starters, the window management and rendering are totally separated.
 *	There are two main render targets that we want to work with:
 *	- SDL
 *	- OpenGL
 *
 *	The render target can be changed in D2.ini, registry, or commandline.
 *	Later on we will want to create a VideoTest.exe that can be used to set optimal video settings.
 *	(by default we will use software since it works on literally everything)
 */

D2Renderer* RenderTarget = nullptr;

static D2Renderer RenderTargets[OD2RT_MAX] = {
	{	// SDL Hardware-Accelerated Renderer
		Renderer_SDL_Init,
		Renderer_SDL_Shutdown,
		Renderer_SDL_Present,
		Renderer_SDL_TextureFromDC6,
		Renderer_SDL_TextureFromAnimatedDC6,
		Renderer_SDL_DrawTexture,
		Renderer_SDL_DrawTextureFrames,
		Renderer_SDL_DrawTextureFrame,
		Renderer_SDL_DeregisterTexture,
		Renderer_SDL_SetTextureBlendMode,
		Renderer_SDL_PollTexture,
		Renderer_SDL_PixelPerfectDetect,
		Renderer_SDL_RegisterDC6Animation,
		Renderer_SDL_DeregisterAnimation,
		Renderer_SDL_Animate,
		Renderer_SDL_SetAnimFrame,
		Renderer_SDL_GetAnimFrame,
		Renderer_SDL_AddAnimKeyframe,
		Renderer_SDL_RemoveAnimKeyframe,
		Renderer_SDL_GetAnimFrameCount,
		Renderer_SDL_RegisterFont,
		Renderer_SDL_DeregisterFont,
		Renderer_SDL_DrawText,
		Renderer_SDL_AlphaModulateTexture,
		Renderer_SDL_ColorModulateTexture,
		Renderer_SDL_AlphaModulateFont,
		Renderer_SDL_ColorModulateFont,
		Renderer_SDL_DrawRectangle,
	},

	{	// OpenGL Renderer
		nullptr,
	},
};

/*
 *	Initializes the renderer.
 *	Call after the window has been created.
 */
void Render_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow)
{
	OpenD2RenderTargets DesiredRenderTarget = OD2RT_SDL;

	// Determine which render target to go with
	if (pConfig->bOpenGL || pConfig->bD3D || pOpenConfig->bNoSDLAccel)
	{
		DesiredRenderTarget = OD2RT_OPENGL;
	}
	else
	{
		DesiredRenderTarget = OD2RT_SDL;
	}

	// Load palettes
	Pal_Init();
	DCC_GlobalInit();

	RenderTarget = &RenderTargets[DesiredRenderTarget];
	RenderTarget->RF_Init(pConfig, pOpenConfig, pWindow);
}

/*
 *	Map rendertarget exports to game module exports
 *	@author	eezstreet
 */
void Render_MapRenderTargetExports(D2ModuleImportStrc* pExport)
{
	pExport->R_RegisterDC6Texture = RenderTarget->RF_TextureFromStitchedDC6;
	pExport->R_RegisterAnimatedDC6 = RenderTarget->RF_TextureFromAnimatedDC6;
	pExport->R_DrawTexture = RenderTarget->RF_DrawTexture;
	pExport->R_DrawTextureFrame = RenderTarget->RF_DrawTextureFrame;
	pExport->R_DrawTextureFrames = RenderTarget->RF_DrawTextureFrames;
	pExport->R_SetTextureBlendMode = RenderTarget->RF_SetTextureBlendMode;
	pExport->R_Present = RenderTarget->RF_Present;
	pExport->R_DeregisterTexture = RenderTarget->RF_DeregisterTexture;
	pExport->R_PollTexture = RenderTarget->RF_PollTexture;
	pExport->R_PixelPerfectDetect = RenderTarget->RF_PixelPerfectDetect;
	pExport->R_RegisterAnimation = RenderTarget->RF_RegisterDCCAnimation;
	pExport->R_DeregisterAnimation = RenderTarget->RF_DeregisterAnimation;
	pExport->R_Animate = RenderTarget->RF_Animate;
	pExport->R_SetAnimFrame = RenderTarget->RF_SetAnimFrame;
	pExport->R_GetAnimFrame = RenderTarget->RF_GetAnimFrame;
	pExport->R_AddAnimKeyframe = RenderTarget->RF_AddAnimKeyframe;
	pExport->R_RemoveAnimKeyframe = RenderTarget->RF_RemoveAnimKeyframe;
	pExport->R_GetAnimFrameCount = RenderTarget->RF_GetAnimFrameCount;
	pExport->R_RegisterFont = RenderTarget->RF_RegisterFont;
	pExport->R_DeregisterFont = RenderTarget->RF_DeregisterFont;
	pExport->R_DrawText = RenderTarget->RF_DrawText;
	pExport->R_AlphaModTexture = RenderTarget->RF_AlphaModTexture;
	pExport->R_ColorModTexture = RenderTarget->RF_ColorModTexture;
	pExport->R_AlphaModFont = RenderTarget->RF_AlphaModFont;
	pExport->R_ColorModFont = RenderTarget->RF_ColorModFont;
	pExport->R_DrawRectangle = RenderTarget->RF_DrawRectangle;
}

/*
 *
 *	LRU Implementation
 *
 */
LRUQueue::LRUQueue(DWORD dwInitialSize)
{
	dwHitCount = dwMissCount = dwQueryCount = dwInUseCount = 0;
	dwLRUSize = dwInitialSize;
	pHead = pTail = nullptr;
}

LRUQueue::~LRUQueue()
{
	LRUQueueItem* pCurrent = pHead;
	LRUQueueItem* pPrev = nullptr;
	while (pCurrent != nullptr)
	{
		pPrev = pCurrent;
		pCurrent = pCurrent->pNext;
		delete pPrev;
	}
}

LRUQueueItem* LRUQueue::QueryItem(handle itemHandle)
{
	// see if it's in the LRU first
	LRUQueueItem* pCurrent = pHead;

	dwQueryCount++;

	while (pCurrent != nullptr)
	{
		if (pCurrent->itemHandle == itemHandle)
		{
			// Mark this as a hit...
			dwHitCount++;
			// ...move it to the front...
			MoveToFront(pCurrent);
			// ... and return it
			return pCurrent;
		}
		pCurrent = pCurrent->pNext;
	}

	// MISS!
	// we need to make a new LRU item and push it to the front
	dwMissCount++;

	pCurrent = new LRUQueueItem(itemHandle);
	dwInUseCount++;
	MoveToFront(pCurrent);
	return pCurrent;
}

void LRUQueue::MoveToFront(LRUQueueItem* pItem)
{
	// If this thing is the front, we don't need to do anything
	if (pItem == pHead)
	{
		return;
	}

	// Move this item out of its current queue slot (if it exists)
	if (pItem->pPrev)
	{
		pItem->pPrev->pNext = pItem->pNext;
	}
	if (pItem->pNext)
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}

	// If this thing was the tail, we need to set it to the previous thing
	if (pItem == pTail)
	{
		pTail = pItem->pPrev;
	}

	// Fix the pointers on the item so it points in the correct directions
	pItem->pPrev = nullptr;
	pItem->pNext = pHead;

	// Make the tail fall off if we exceed the size
	if (dwInUseCount > dwLRUSize)
	{
		LRUQueueItem* pOldTail = pTail;
		pTail = pTail->pPrev;
		delete pOldTail;
		dwInUseCount = dwLRUSize;
	}

	// Insert this thing as the head
	if (pHead)
	{
		pHead->pPrev = pItem;
	}
	pHead = pItem;

	// ...and possibly as the tail too if there isn't one
	if (pTail == nullptr)
	{
		pTail = pHead;
	}
}