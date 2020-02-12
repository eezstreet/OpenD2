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
	D2MPQArchive* pMPQ;
	fs_handle fileHandle = engine->MPQ_FindFile(szFilePath, nullptr, &pMPQ);

	if (fileHandle == INVALID_HANDLE)
	{
		return nullptr;
	}

	// Find out the file size and allocate that many bytes to read
	size_t fileSize = engine->MPQ_FileSize(pMPQ, fileHandle);
	BYTE* fileData = (BYTE*)malloc(fileSize);
	if (fileData == nullptr)
	{
		return nullptr; // TODO: proper memory allocation
	}

	// Read the file from the MPQ (or, FIXME: from local disk if -direct is enabled)
	engine->MPQ_ReadFile(pMPQ, fileHandle, fileData, fileSize);



	free(fileData);
}