#include "Diablo2.hpp"

////////////////////////////////////////////////////////
//
//	Text-Based .TBL files store the game's strings.
//	These should not be confused with Font-Based TBLs.

#define MAX_TBL_FILES	8
#define TBL_VERSION		1

#define TBL_EXPANSION_OFFSET	20000
#define TBL_PATCH_OFFSET		10000
#define TBL_CLASSIC_OFFSET		0

namespace TBL
{

	static TBLFile TBLFiles[MAX_TBL_FILES]{ 0 };
	static int gnLastUsedTBL = 0;

	static tbl_handle gBaseStrings;
	static tbl_handle gExpansionStrings;
	static tbl_handle gPatchStrings;

	/*
	 *	Registers a TBL file
	 *	@author	eezstreet
	 */
	tbl_handle Register(char* szTblFile)
	{
		char szPathStr[MAX_D2PATH]{ 0 };
		TBLFile* pTBL = &TBLFiles[gnLastUsedTBL];
		BYTE* pFileBuffer;
		BYTE* pReadHead;
		DWORD dwTableSize = 0;
		size_t strTableRead = 0;

		Log_ErrorAssertReturn(gnLastUsedTBL < MAX_TBL_FILES, INVALID_HANDLE);

		// Search the existing TBL file records to see if one already exists
		for (int i = 0; i < gnLastUsedTBL; i++)
		{
			if (!D2Lib::stricmp(TBLFiles[i].szHandle, szTblFile))
			{
				return i;
			}
		}

		D2Lib::strncpyz(pTBL->szHandle, szTblFile, 16);

		// TODO: make this use something other than english
		snprintf(szPathStr, MAX_D2PATH, "data\\local\\LNG\\%s\\%s.tbl", GAME_LANGUAGE, szTblFile);

		pTBL->archiveHandle = FSMPQ::FindFile(szPathStr, nullptr, &pTBL->pArchive);
		if (pTBL->archiveHandle == INVALID_HANDLE)
		{	// couldn't find this TBL file
			return INVALID_HANDLE;
		}
		pTBL->dwFileSize = MPQ::FileSize(pTBL->pArchive, pTBL->archiveHandle);

		// Allocate file buffer and read
		pFileBuffer = (BYTE*)malloc(pTBL->dwFileSize);
		Log_ErrorAssertReturn(pFileBuffer, INVALID_HANDLE);
		pReadHead = pFileBuffer;
		MPQ::ReadFile(pTBL->pArchive, pTBL->archiveHandle, pFileBuffer, pTBL->dwFileSize);

		// Copy contents from buffer into the file
		memcpy(&pTBL->header, pReadHead, sizeof(TBLHeader));
		pReadHead += sizeof(TBLHeader);

		// Validate TBL (just by checking the version, not using the CRC)
		if (pTBL->header.Version != TBL_VERSION)
		{
			Log::Print(PRIORITY_MESSAGE, "Couldn't load TBL %s: wrong version (got %i, expected %i)",
				szPathStr, pTBL->header.Version, TBL_VERSION);
			free(pFileBuffer);
			return INVALID_HANDLE;
		}

		// Allocate and read indices
		pTBL->pIndices = (WORD*)malloc(sizeof(WORD) * pTBL->header.NodesNumber);
		Log_ErrorAssertReturn(pTBL->pIndices, INVALID_HANDLE);
		memcpy(pTBL->pIndices, pReadHead, sizeof(WORD) * pTBL->header.NodesNumber);
		pReadHead += sizeof(WORD) * pTBL->header.NodesNumber;

		// Allocate and read hash tables
		pTBL->pHashNodes = (TBLHashNode*)malloc(sizeof(TBLHashNode) * pTBL->header.HashTableSize);
		Log_ErrorAssertReturn(pTBL->pHashNodes, INVALID_HANDLE);
		memcpy(pTBL->pHashNodes, pReadHead, sizeof(TBLHashNode) * pTBL->header.HashTableSize);
		pReadHead += sizeof(TBLHashNode) * pTBL->header.HashTableSize;

		// Allocate data tables
		pTBL->pDataNodes = (TBLDataNode*)malloc(sizeof(TBLDataNode) * pTBL->header.HashTableSize);
		Log_ErrorAssertReturn(pTBL->pDataNodes, INVALID_HANDLE);
		memset(pTBL->pDataNodes, 0, sizeof(TBLDataNode) * pTBL->header.HashTableSize);

		// Allocate strings
		dwTableSize = sizeof(char16_t) * (pTBL->dwFileSize - (pReadHead - pFileBuffer));
		pTBL->szStringTable = (char16_t*)malloc(dwTableSize);
		Log_ErrorAssertReturn(pTBL->szStringTable, INVALID_HANDLE);
		memset(pTBL->szStringTable, 0, dwTableSize);

		// Compute data tables
		for (int i = 0; i < pTBL->header.HashTableSize; i++)
		{
			TBLHashNode* pHash = &pTBL->pHashNodes[i];
			size_t numRead;

			if (pHash->Active > 0) // Only recalculate the offset for hash entries which are valid
			{
				pHash->StringKeyOffset -= pTBL->header.DataStartOffset;
				pHash->StringValOffset -= pTBL->header.DataStartOffset;

				pTBL->pDataNodes[i].bActive = true;

				// Read key
				if (!D2Lib::stricmp(GAME_CHARSET, "LATIN"))
				{	// if we're on latin charset, we need to convert these from multibyte to widechar
					D2Lib::qmbtowc(pTBL->pDataNodes[i].key, MAX_TBL_KEY_SIZE, (char*)pReadHead + pHash->StringKeyOffset);
				}
				else
				{
					D2Lib::qstrncpyz(pTBL->pDataNodes[i].key,
						(char16_t*)(pReadHead + pHash->StringKeyOffset), // note: might not be accurate
						MAX_TBL_KEY_SIZE);
				}

				// Read string value
				if (!D2Lib::stricmp(GAME_CHARSET, "LATIN"))
				{
					numRead = D2Lib::qmbtowc(pTBL->szStringTable + strTableRead,
						pHash->StringValLength,
						(char*)pReadHead + pHash->StringValOffset);
				}
				else
				{
					numRead = D2Lib::qstrncpyz(pTBL->szStringTable + strTableRead,
						(char16_t*)(pReadHead + pHash->StringValOffset), // note: might not be accurate
						pHash->StringValLength // note: might not be accurate
					);
				}
				pTBL->pDataNodes[i].str = pTBL->szStringTable + strTableRead;
				strTableRead += numRead;
			}
		}

		free(pFileBuffer);
		return gnLastUsedTBL++;
	}

	/*
	 *	This (extremely useful) function prints a list of indices and their associated string values.
	 *	Note that it converts from widechar to multichar, so the string representations may not be perfect.
	 */
	void Debug(tbl_handle tbl)
	{
		TBLFile* pTBL = &TBLFiles[tbl];
		char filename[MAX_D2PATH]{ 0 };
		char line[8096]{ 0 };
		char key[MAX_TBL_KEY_SIZE]{ 0 };
		char str[8096]{ 0 };
		fs_handle f;

		snprintf(filename, MAX_D2PATH, "%s.txt", pTBL->szHandle);
		FS::Open(filename, &f, FS_WRITE);
		snprintf(line, 8096, "Contents of %s.tbl:" NL, pTBL->szHandle);
		FS::WritePlaintext(f, line);

		for (int i = 0; i < pTBL->header.NodesNumber; i++)
		{
			WORD index = pTBL->pIndices[i];
			TBLDataNode* pNode = &pTBL->pDataNodes[index];

			D2Lib::qwctomb(key, MAX_TBL_KEY_SIZE, pNode->key);
			D2Lib::qwctomb(str, 8096, pNode->str);

			snprintf(line, 8096, "(%i/%i) %s = %s" NL, i, index, key, str);
			FS::WritePlaintext(f, line);
		}

		FS::CloseFile(f);
	}

	/*
	 *	Tries and finds a string with a given index. Returns nullptr if it couldn't be found.
	 */
	static char16_t* TryFindStringInternal(tbl_handle tbl, tbl_handle dwIndex)
	{
		TBLFile* pTBL = &TBLFiles[tbl];
		if (dwIndex < 0 || dwIndex >= pTBL->header.NodesNumber)
		{
			return nullptr;
		}
		return pTBL->pDataNodes[pTBL->pIndices[dwIndex]].str;
	}

	/*
	 *	Finds a specific string based on an index key.
	 */
	char16_t* FindStringFromIndex(DWORD dwIndex)
	{
		char16_t* pReturnValue = nullptr;

		// MEGAHACK, but this is literally exactly what the game does, so idk
		if (dwIndex >= TBL_EXPANSION_OFFSET)
		{
#ifdef EXPANSION
			pReturnValue = TryFindStringInternal(gExpansionStrings, dwIndex - TBL_EXPANSION_OFFSET);
			if (pReturnValue != nullptr)
			{
				return pReturnValue;
			}
#else
			Log_Print(PRIORITY_MESSAGE, "Requested string %d but expansion strings not loaded", dwIndex);
#endif
		}
		if (dwIndex >= TBL_PATCH_OFFSET)
		{
			pReturnValue = TryFindStringInternal(gPatchStrings, dwIndex - TBL_PATCH_OFFSET);
			if (pReturnValue != nullptr)
			{
				return pReturnValue;
			}
		}
		if (dwIndex >= TBL_CLASSIC_OFFSET)
		{
			pReturnValue = TryFindStringInternal(gBaseStrings, dwIndex - TBL_CLASSIC_OFFSET);
			if (pReturnValue != nullptr)
			{
				return pReturnValue;
			}
		}

		// all options have failed, iterate until we can find it
		for (int i = gnLastUsedTBL - 1; i >= 0; i--)
		{
			pReturnValue = TryFindStringInternal(i, dwIndex);
			if (pReturnValue != nullptr)
			{
				return pReturnValue;
			}
		}

		// could not find it in any of the TBL files
		return nullptr;
	}

	/*
	 *	Finds a string index based on a textual key - internal
	 */
	tbl_handle FindStringIndexFromKey(tbl_handle tbl, char16_t* szReference)
	{
		TBLFile* pTBL = &TBLFiles[tbl];
		fs_handle hash = D2Lib::qstrhash(szReference, MAX_TBL_KEY_SIZE, pTBL->header.HashTableSize);
		DWORD dwHashTries = 0;

		while (dwHashTries < pTBL->header.HashMaxTries)
		{
			if (pTBL->pDataNodes[hash].bActive)
			{
				if (!D2Lib::qstrcmp(pTBL->pDataNodes[hash].str, szReference))
				{
					return hash;
				}
			}
			else
			{
				return INVALID_HANDLE;
			}

			hash++;
			hash %= pTBL->header.HashTableSize;

			dwHashTries++;
		}

		return INVALID_HANDLE;
	}

	/*
	 *	Finds a specific string based on a key text.
	 */
	char16_t* FindStringText(char16_t* szReference)
	{
		char16_t* pStr = nullptr;
		for (int i = gnLastUsedTBL - 1; i >= 0; i--)
		{
			tbl_handle x = FindStringIndexFromKey(i, szReference);
			if (x != INVALID_HANDLE)
			{
				return TryFindStringInternal(i, x);
			}
		}

		// could not find it in any of the TBL files
		return nullptr;
	}

	/*
	 *	Loads all of the TBL files
	 *	@author	eezstreet
	 */
	void Init()
	{
		gBaseStrings = Register("string");
		gExpansionStrings = Register("expansionstring");
		gPatchStrings = Register("patchstring");

		// uncomment if you are curious as to what's in these files
		//Debug(gBaseStrings);
		//Debug(gExpansionStrings);
		//Debug(gPatchStrings);
	}

	/*
	 *	Cleans up all of the TBL files
	 *	@author	eezstreet
	 */
	void Cleanup()
	{
		for (int i = 0; i < gnLastUsedTBL; i++)
		{
			if (TBLFiles[i].pHashNodes != nullptr)
			{
				free(TBLFiles[i].pHashNodes);
			}
			if (TBLFiles[i].pIndices != nullptr)
			{
				free(TBLFiles[i].pIndices);
			}
			if (TBLFiles[i].pDataNodes != nullptr)
			{
				free(TBLFiles[i].pDataNodes);
			}
			if (TBLFiles[i].szStringTable != nullptr)
			{
				free(TBLFiles[i].szStringTable);
			}
		}
	}
}