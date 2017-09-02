#include "Diablo2.hpp"

#define MAX_INPUT_COMMANDS	512		// there is no reason we should ever go higher than this

static D2CommandQueue gProcessedCommands[MAX_INPUT_COMMANDS];
static DWORD gdwNumProcessedCommands;

/*
 *	Maps an SDL button to an OpenD2 button
 */
static inline D2InputButton In_MapButton(int nSDLButton)
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
static inline DWORD In_MapModifiers(int nSDLModifiers)
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
static inline DWORD In_GetMappedModifiers()
{
	int nSDLModifiers = SDL_GetModState();
	return In_MapModifiers(nSDLModifiers);
}

/*
 *	Pumps all events out of the system
 */
void In_PumpEvents(OpenD2ConfigStrc* pOpenConfig)
{
	SDL_Event ev{ 0 };

	gdwNumProcessedCommands = 0;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_MOUSEMOTION:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEMOVE;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.x = ev.motion.x;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.y = ev.motion.y;
				gdwNumProcessedCommands++;
				break;
			case SDL_MOUSEBUTTONDOWN:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEDOWN;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = In_MapButton(ev.button.button);
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = In_GetMappedModifiers();
				gdwNumProcessedCommands++;
				break;
			case SDL_MOUSEBUTTONUP:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEUP;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = In_MapButton(ev.button.button);
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = In_GetMappedModifiers();
				gdwNumProcessedCommands++;
				break;
			case SDL_MOUSEWHEEL:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_MOUSEWHEEL;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.x = ev.wheel.x;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.motion.y = ev.wheel.y;
				gdwNumProcessedCommands++;
				break;
			case SDL_KEYDOWN:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_KEYDOWN;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = In_MapButton(ev.key.keysym.sym);
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = In_MapModifiers(ev.key.keysym.mod);
				gdwNumProcessedCommands++;
				break;
			case SDL_KEYUP:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_KEYUP;
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.buttonID = In_MapButton(ev.key.keysym.sym);
				gProcessedCommands[gdwNumProcessedCommands].cmdData.button.mod = In_MapModifiers(ev.key.keysym.mod);
				gdwNumProcessedCommands++;
				break;
			case SDL_TEXTINPUT:
				gProcessedCommands[gdwNumProcessedCommands].cmdType = IN_TEXT;
				gdwNumProcessedCommands++;
				break;
			case SDL_QUIT:
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