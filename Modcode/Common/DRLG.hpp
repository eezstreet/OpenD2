#pragma once
#include "../../Shared/D2World.hpp"

#define MAX_DS1_LOADED	1024

extern WorldTree gptActWorlds[MAX_ACTS];

//////////////////
//
//	DRLG_DS1.cpp

struct DS1File
{
	// Header
	DWORD dwDS1Version;	// Crucial; it determines much of the parsing
	DWORD dwDS1Width;
	DWORD dwDS1Height;
	DWORD dwAct;		// Not present in version < 8
	DWORD dwTagType;	// Not present in version < 10

	// Extra Files (not used in vanilla Diablo 2, used by Blizzard North developers for tools purposes)
	// Not present in version < 3
	DWORD dwNumberCommentFiles;
	char** szCommentFiles;

	// Two DWORD sized chunks are skipped between versions 9 and 13 (inclusive)

	// Layers information
	DWORD dwNumberWallLayers;	// Not present in version < 4 (hardcoded to be 1)
	DWORD dwNumberFloorLayers;	// Not present in version < 16 (hardcoded to be 1)

	// The order that the layers are streamed in depends on the version number.

};

struct DS1Hash
{
	char szFileName[MAX_D2PATH];
	DS1File file;
	bool bLoaded;
};

extern DS1Hash gtDS1Files[MAX_DS1_LOADED];

DS1File* DS1_Load(char* szFilePath);
void DS1_FreeAll();