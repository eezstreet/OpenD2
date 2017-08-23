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

/*
 *	The structure containing data about MPQ files
 *	@author Tom Amigo/Paul Siramy/eezstreet
 */
 // Based partially on Paul Siramy's win_ds1edit code
struct D2MPQArchive
{
	bool	bOpen;				// Whether the MPQ archive is currently open / has no errors
	DWORD	dwDataOffset;		// Offset to MPQ file data
	DWORD	dwHashOffset;		// Offset to MPQ hash table
	DWORD	dwBlockOffset;		// Offset to MPQ block table
	DWORD	dwDataLength;		// Length of MPQ file data
	DWORD	dwHashLength;		// Length of MPQ hash table
	DWORD	dwBlockLength;		// Length of MPQ block table
	DWORD*	pHashTable;			// Pointer to MPQ hash table
	DWORD*	pBlockTable;		// Pointer to MPQ block table
	DWORD	dwFileCount;		// Number of files in MPQ
	DWORD	dwSlackSpace[0x500];

	fs_handle	f;				// FS file handle
};


/////////////////////////////////////////////////////////
//
//	Functions

// Diablo2.cpp
int InitGame(int argc, char** argv, DWORD pid);

// Platform_*.cpp
void Sys_InitModules();
void Sys_GetWorkingDirectory(char* szBuffer, size_t dwBufferLen);

// FileSystem.cpp
void FS_Init(OpenD2ConfigStrc* pConfig);
void FS_Shutdown();
size_t FS_Open(char* filename, fs_handle* f, OpenD2FileModes mode, bool bBinary = false);
inline size_t FS_Read(fs_handle f, void* buffer, size_t dwBufferLen = 4, size_t dwCount = 1);
size_t FS_Write(fs_handle f, void* buffer, size_t dwBufferLen = 1, size_t dwCount = 1);
void FS_CloseFile(fs_handle f);
void FS_Seek(fs_handle f, size_t offset, int nSeekType);
size_t FS_Tell(fs_handle f);

// MPQ.cpp

void MPQ_OpenMPQ(char* szMPQPath, const char* szMPQName, D2MPQArchive* pMPQ);
void MPQ_CloseMPQ(D2MPQArchive* pMPQ);
fs_handle MPQ_FetchHandle(D2MPQArchive* pMPQ, char* szFileName);
size_t MPQ_FileSize(D2MPQArchive* pMPQ, fs_handle fFile);
size_t MPQ_ReadFile(D2MPQArchive* pMPQ, fs_handle fFile, BYTE* buffer, DWORD dwBufferLen);


/////////////////////////////////////////////////////////
//
//	Variables

extern run_t gpfModules[];