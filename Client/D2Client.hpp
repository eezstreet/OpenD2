#pragma once
#include "../Shared/D2Shared.hpp"

/////////////////////////////////////////////////
//
//	Forward Declarations

class D2Menu;
class D2Panel;
class D2Widget;

/////////////////////////////////////////////////
//
//	Types

// Menu signals can be passed to the 
typedef void (*MenuSignal)(D2Panel* pCallingPanel, D2Widget* pCallingWidget);

/////////////////////////////////////////////////
//
//	Data structures

// What "gamestate" we are in
enum D2Client_GameState
{
	GS_TRADEMARK,
	GS_MAINMENU,	// main menu, or anything in between main menu and ingame (charselect, bnet, etc)
	GS_INGAME,		// in the actual game itself
};

// Global status structure
struct D2Client
{
	D2Client_GameState	gamestate;
	bool				bLocalServer;
	bool				bKillGame;
	DWORD				dwMouseX;
	DWORD				dwMouseY;
	bool				bMouseClicked;
	bool				bMouseDown;
	DWORD				dwMS;
	DWORD				dwStartMS;

	font_handle			fontExocet10;
	font_handle			font16;
	font_handle			fontFormal12;

	D2Menu*				pActiveMenu;
};

/////////////////////////////////////////////////
//
//	Global variables

extern D2ModuleImportStrc* trap;
extern D2GameConfigStrc* config;
extern OpenD2ConfigStrc* openConfig;
extern D2Client cl;

#define Log_WarnAssert(x, y)	if(!x) { trap->Warning(__FILE__, __LINE__, #x); return y; }
#define Log_ErrorAssert(x, y)	if(!x) { trap->Error(__FILE__, __LINE__, #x); return y; }