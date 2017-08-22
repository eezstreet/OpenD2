#pragma once
#include "../Shared/D2Shared.hpp"


/////////////////////////////////////////////////////////
//
//	Types

enum D2InterfaceModules
{
	D2I_NONE,
	D2I_CLIENT,
	D2I_SERVER,
	D2I_MULTI,
	D2I_LAUNCH,
	D2I_MAX,
};

typedef D2InterfaceModules(__fastcall *run_t)(D2GameConfigStrc*);
typedef run_t*(__fastcall *interface_t)();


/////////////////////////////////////////////////////////
//
//	Structures

/*
 *	The structure containing information about parsing commandline arguments
 *	@author	Necrolis
 */
struct D2CmdArgStrc
{
	char szSection[16];		// +00
	char szKeyName[16];		// +10
	char szCmdName[16];		// +20
	DWORD dwType;			// +30 ( 0 use GetProfInt - write bool, 1 DWORD , 2 string, 3 BYTE, 4 WORD)
	int nOffset;			// +34
	DWORD dwDefault;		// +38
};							// +3C


/////////////////////////////////////////////////////////
//
//	Functions

// Diablo2.cpp
int InitGame(int argc, char** argv, DWORD pid);

// Platform_*.cpp
void Sys_InitModules();


/////////////////////////////////////////////////////////
//
//	Variables

extern run_t gpfModules[];