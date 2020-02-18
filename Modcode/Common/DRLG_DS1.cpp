#include "D2Common.hpp"
#include "DRLG.hpp"

DS1Hash gtDS1Files[MAX_DS1_LOADED];

/*
 *	Loads a DS1 file.
 *	@author	eezstreet
 */
DS1File* DS1_Load(char* szFilePath)
{
	// Try to see if a string already exists with this hash
	char* szFileStripped = D2Lib::fnextstrb(szFilePath);
	DWORD dwHash = D2Lib::strhash(szFileStripped, MAX_D2PATH_ABSOLUTE, MAX_DS1_LOADED);

	// Find the file itself
	fs_handle fileHandle;
	DWORD dwFileSize = engine->FS_Open(szFilePath, &fileHandle, FS_READ, true);

	if (fileHandle == INVALID_HANDLE)
	{
		return nullptr;
	}
	if (dwFileSize == 0)
	{
		engine->FS_CloseFile(fileHandle);
		return nullptr;
	}

	// Allocate that many bytes to read and then read it
	BYTE* fileData = (BYTE*)malloc(dwFileSize);
	if (fileData == nullptr)
	{
		return nullptr; // TODO: proper memory allocation
	}

	engine->FS_Read(fileHandle, fileData, dwFileSize, 1);
	engine->FS_CloseFile(fileHandle);

	free(fileData); // FIXME
}