#include "Renderer.hpp"
#include "Renderer_GL.hpp"
#include "Palette.hpp"
#include "DCC.hpp"

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

IRenderer* RenderTarget = nullptr;

namespace Renderer
{
	/*
	 *	Initializes the renderer.
	 *	Call after the window has been created.
	 */
	void Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow)
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
		Pal::Init();
		DCC::GlobalInit();

		switch (DesiredRenderTarget)
		{
			default:
			case OD2RT_OPENGL:
				RenderTarget = new Renderer_GL(pConfig, pOpenConfig, pWindow);
				break;
#if 0	// no longer available
			case OD2RT_SDL:
				RenderTarget = new Renderer_SDL(pConfig, pOpenConfig, pWindow);
				break;
#endif
		}
	}

	/*
	 *	Map rendertarget exports to game module exports
	 *	@author	eezstreet
	 */
	void MapRenderTargetExports(D2ModuleImportStrc* pExport)
	{
		pExport->renderer = RenderTarget;
	}
}