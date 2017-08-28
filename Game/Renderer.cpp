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

// Gets called on init
static RFInit RendererInitFunctions[] = {
	Renderer_SDL_Init,	// Renderer_SDL
	nullptr,	// Renderer_SDL_Software
	nullptr,	// Renderer_DirectX
	nullptr,	// Renderer_OpenGL
};

// Gets called on shutdown
static RFShutdown RendererShutdownFunctions[] = {
	Renderer_SDL_Shutdown,	// Renderer_SDL
	nullptr,	// Renderer_SDL_Software
	nullptr,	// Renderer_DirectX
	nullptr,	// Renderer_OpenGL
};

static OpenD2RenderTargets RenderTarget = OD2RT_SDL;

/*
 *	Initializes the renderer.
 *	Call after the window has been created.
 */
void Render_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow)
{
	// Determine which render target to go with
	if (pConfig->bOpenGL)
	{
		RenderTarget = OD2RT_OPENGL;
	}
	else if (pConfig->bD3D)
	{
		RenderTarget = OD2RT_DIRECTX;
	}
	else if (pOpenConfig->bNoSDLAccel)
	{
		RenderTarget = OD2RT_SDL_SOFTWARE;
	}
	else
	{
		RenderTarget = OD2RT_SDL;
	}

	// Initialize it!
	if (RendererInitFunctions[RenderTarget] != nullptr)
	{
		RendererInitFunctions[RenderTarget](pConfig, pOpenConfig, pWindow);
	}
}

/*
 *	Shuts down the renderer.
 *	Call before the window has been destroyed.
 */
void Render_Shutdown()
{
	if (RendererShutdownFunctions[RenderTarget] != nullptr)
	{
		RendererShutdownFunctions[RenderTarget]();
	}
}