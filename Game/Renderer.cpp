#include "Renderer.hpp"

/*
 *	The renderer in OpenD2 is significantly different from the one in retail Diablo 2.
 *
 *	For starters, the window management and rendering are totally separated.
 *	There are three main render targets that we want to work with:
 *	- Software Renderer (SDL)
 *	- DirectX (d3d)
 *	- OpenGL (opengl)
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
		Renderer_SDL_RegisterTexture,
		Renderer_SDL_TextureFromStitchedDC6,
		Renderer_SDL_SetTexturePixels,
		Renderer_SDL_DrawTexture,
	},

	{	// SDL Software Renderer
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	},

	{	// DirectX Renderer (Windows-only)
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	},

	{	// OpenGL Renderer
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
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
	if (pConfig->bOpenGL)
	{
		DesiredRenderTarget = OD2RT_OPENGL;
	}
	else if (pConfig->bD3D)
	{
		DesiredRenderTarget = OD2RT_DIRECTX;
	}
	else if (pOpenConfig->bNoSDLAccel)
	{
		DesiredRenderTarget = OD2RT_SDL_SOFTWARE;
	}
	else
	{
		DesiredRenderTarget = OD2RT_SDL;
	}

	RenderTarget = &RenderTargets[DesiredRenderTarget];
	RenderTarget->RF_Init(pConfig, pOpenConfig, pWindow);

	// Load palettes
	Pal_Init();
}