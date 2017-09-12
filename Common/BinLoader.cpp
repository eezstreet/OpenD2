#include "D2Common.hpp"

/*
 *	Compiles a .txt into a .bin file
 *	@author	eezstreet
 */
void BIN_Compile(const char* szTextName, D2TxtColumnStrc* pColumns, int* nNumberColumns)
{
	
}

/*
 *	Reads a .bin file
 *	@author	eezstreet
 */
bool BIN_Read(char* szBinName, void** pDestinationData, size_t* pFileSize)
{
	D2MPQArchive* pArchive = nullptr;
	fs_handle f = trap->MPQ_FindFile(szBinName, nullptr, &pArchive);
	DWORD dwNumRecords = 0;

	if (f == INVALID_HANDLE)
	{
		return false;	// couldn't find it...this is probably a bad thing
	}

	*pFileSize = trap->MPQ_FileSize(pArchive, f);
	Log_ErrorAssert(*pFileSize != 0, false);

	*pDestinationData = malloc(*pFileSize);
	trap->MPQ_ReadFile(pArchive, f, (BYTE*)*pDestinationData, *pFileSize);

	//	Somewhat of a hack here, but the first field in the BIN actually seems to be a DWORD
	//	that specifies how many records in the file.
	//	Needless to say, this will screw everything up if we don't account for it correctly.
	void* offset = (void*)(((BYTE*)*pDestinationData) + 4);
	*pFileSize -= sizeof(DWORD);
	memmove(*pDestinationData, offset, *pFileSize);

	return true;
}