#include "Diablo2.hpp"
#include <assert.h>

///////////////////////////////////////////////////////
//
//	Diablo II (OpenD2) Window Management Code

namespace Window
{
	static SDL_Window* gpWindow;

	/*
	 *	Creates a window
	 */
	static SDL_Window* CreateWindow(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
	{
		SDL_Window* pWin = nullptr;
		DWORD dwWindowFlags = 0;
		int windowX = 800;
		int windowY = 600;

		if (!pConfig->bWindowed)
		{
			windowX = 0;
			windowY = 0;
			dwWindowFlags |= SDL_WINDOW_FULLSCREEN;
		}
		if (pOpenConfig->bBorderless)
		{
			dwWindowFlags |= SDL_WINDOW_BORDERLESS;
		}
		if (pConfig->bOpenGL)
		{
			dwWindowFlags |= SDL_WINDOW_OPENGL;
		}

		// TODO: make the size based on resolution
		pWin = SDL_CreateWindow("Diablo II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowX, windowY, dwWindowFlags);

		return pWin;
	}

	/*
	 *	Inits SDL and creates a window
	 */
	void InitSDL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);

		gpWindow = CreateWindow(pConfig, pOpenConfig);

		Log_ErrorAssert(gpWindow != nullptr);

		Renderer::Init(pConfig, pOpenConfig, gpWindow);
	}

	/*
	 *	Shuts down SDL and destroys the window
	 */
	void ShutdownSDL()
	{
		DCC::GlobalShutdown();
		RenderTarget->RF_Shutdown();
		SDL_DestroyWindow(gpWindow);
		SDL_Quit();
	}

	/*
	 *	Wrapper for SDL_ShowSimpleMessageBox
	 */
	void ShowMessageBox(int nMessageBoxType, char* szTitle, char* szMessage)
	{
		SDL_ShowSimpleMessageBox(nMessageBoxType, szTitle, szMessage, gpWindow);
	}

	/*
	 *	Determines if the current window is in focus by comparing the given window ID versus the actual window ID
	 *	@author	eezstreet
	 */
	bool InFocus(DWORD nWindowID)
	{
		return SDL_GetWindowID(gpWindow) == nWindowID;
	}
}