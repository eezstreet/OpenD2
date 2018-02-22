#pragma once
#include "../Shared/D2Common_Shared.hpp"

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
	GS_LOADING,
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
	int					nLoadState;

	font_handle			fontExocet10;
	font_handle			font16;
	font_handle			font30;
	font_handle			font42;
	font_handle			fontFormal12;
	font_handle			fontRidiculous;

	char				szCurrentSave[MAX_D2PATH];

	D2Menu*				pActiveMenu;
};

/////////////////////////////////////////////////
//
//	Global variables

extern D2ModuleImportStrc* trap;
extern D2GameConfigStrc* config;
extern OpenD2ConfigStrc* openConfig;
extern D2Client cl;

/////////////////////////////////////////////////
//
//	Helper functions

bool Client_classMale(int nCharClass);
char16_t* Client_className(int nCharClass);

/////////////////////////////////////////////////
//
//	Client functions

void D2Client_CreateServer();