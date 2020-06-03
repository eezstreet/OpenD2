#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Shared/D2Packets.hpp"
#include <SDL2/SDL.h>

enum D2CommandType
{
	CMD_BOOLEAN,
	CMD_DWORD,
	CMD_STRING,
	CMD_BYTE,
	CMD_WORD
};

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

/*
 *	The structure containing information about the system running the game
 */
struct D2SystemInfoStrc
{
	char szComputerName[64];
	char szOSName[128];
	char szProcessorVendor[32];
	char szProcessorModel[64];
	char szProcessorSpeed[64];	// maybe not present on linux?
	char szProcessorIdentifier[64];
	char szRAMPhysical[64];
	char szRAMVirtual[64];
	char szRAMPaging[64];
	char szWorkingDirectory[MAX_D2PATH_ABSOLUTE];
};

// Diablo2.cpp
int InitGame(int argc, char** argv);
DWORD GetMilliseconds();
bool ServerProcessPacket(D2Packet* pPacket);
bool ClientProcessPacket(D2Packet* pPacket);