#include "Diablo2.hpp"

#define MAX_INPUT_COMMANDS	512		// there is no reason we should ever go higher than this

namespace IN
{

	static D2CommandQueue gProcessedCommands[MAX_INPUT_COMMANDS];
	static DWORD gdwNumProcessedCommands;

	/*
	 *	Maps an SDL button to an OpenD2 button
	 */
	static inline D2InputButton MapButton(int nSDLButton)
	{
		switch (nSDLButton)
		{
		case SDL_BUTTON(SDL_BUTTON_LEFT):
			return B_MOUSE1;
		case SDL_BUTTON(SDL_BUTTON_RIGHT):
			return B_MOUSE2;
		case SDL_BUTTON(SDL_BUTTON_MIDDLE):
			return B_MOUSE3;
		case SDL_BUTTON(SDL_BUTTON_X1):
			return B_MOUSE4;
		case SDL_BUTTON(SDL_BUTTON_X2):
			return B_MOUSE5;
		default:
			return (D2InputButton)(nSDLButton);
		}
	}

	/*
	 *	Maps SDL keymod to OpenD2 keymod
	 */
	static inline DWORD MapModifiers(int nSDLModifiers)
	{
		DWORD dwModifiers = 0;
		if (nSDLModifiers & KMOD_SHIFT)
		{
			dwModifiers |= KEYMOD_SHIFT;
		}
		if (nSDLModifiers & KMOD_ALT)
		{
			dwModifiers |= KEYMOD_ALT;
		}
		if (nSDLModifiers & KMOD_CTRL)
		{
			dwModifiers |= KEYMOD_CTRL;
		}
		return dwModifiers;
	}

	/*
	 *	Get SDL keymods and map 'em
	 */
	static inline DWORD GetMappedModifiers()
	{
		int nSDLModifiers = SDL_GetModState();
		return MapModifiers(nSDLModifiers);
	}

	/*
	 *	Pumps all events out of the system
	 */
	void PumpEvents(OpenD2ConfigStrc* pOpenConfig)
	{
		SDL_Event ev{ 0 };

		gdwNumProcessedCommands = 0;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_WINDOWEVENT:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_WINDOW;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.window.event = (D2WindowEventType)ev.window.event;
				gdwNumProcessedCommands++;
				break;
			case SDL_MOUSEMOTION:
				//				if (D2Win_InFocus(ev.motion.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEMOVE;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.x = ev.motion.x;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.y = ev.motion.y;
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_MOUSEBUTTONDOWN:
				//				if (D2Win_InFocus(ev.button.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEDOWN;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = MapButton(ev.button.button);
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = GetMappedModifiers();
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_MOUSEBUTTONUP:
				//				if (D2Win_InFocus(ev.button.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEUP;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = MapButton(ev.button.button);
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = GetMappedModifiers();
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_MOUSEWHEEL:
				//				if (D2Win_InFocus(ev.wheel.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEWHEEL;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.x = ev.wheel.x;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.y = ev.wheel.y;
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_KEYDOWN:
				//				if (D2Win_InFocus(ev.key.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_KEYDOWN;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = ev.key.keysym.sym;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = MapModifiers(ev.key.keysym.mod);
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_KEYUP:
				//				if (D2Win_InFocus(ev.key.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_KEYUP;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = ev.key.keysym.sym;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = MapModifiers(ev.key.keysym.mod);
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_TEXTINPUT:
				//				if (D2Win_InFocus(ev.text.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_TEXTINPUT;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.text.length = 0;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.text.start = 0;
				D2Lib::strncpyz(gProcessedCommands[gdwNumProcessedCommands].cmdData.text.text, ev.text.text, 32);
				gdwNumProcessedCommands++;
			}

			break;
			case SDL_TEXTEDITING:
				//				if (D2Win_InFocus(ev.edit.windowID))
			{
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_TEXTEDITING;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.text.length = ev.edit.length;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.text.start = ev.edit.start;
				D2Lib::strncpyz(gProcessedCommands[gdwNumProcessedCommands].cmdData.text.text, ev.edit.text, 32);
				gdwNumProcessedCommands++;
			}
			break;
			case SDL_QUIT:
				// quit events are always considered to be in focus
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_QUIT;
				gdwNumProcessedCommands++;
				break;
			}
			if (gdwNumProcessedCommands >= MAX_INPUT_COMMANDS)
			{
				break;
			}
		}

		pOpenConfig->pCmds = gProcessedCommands;
		pOpenConfig->dwNumPendingCommands = gdwNumProcessedCommands;
	}
}