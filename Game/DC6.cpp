#include "Diablo2.hpp"
#include <memory>

/*
 *	Loads a DC6 from an MPQ
 */
void DC6_LoadImage(char* szPath, DC6Image* pImage)
{
	pImage->f = FSMPQ_FindFile(szPath, nullptr, (D2MPQArchive**)&pImage->mpq);
	if (pImage->f == (fs_handle)-1)
	{
		return;
	}

	// Now comes the fun part: reading and decoding the actual thing
	size_t dwFileSize = MPQ_FileSize((D2MPQArchive*)pImage->mpq, pImage->f);
	BYTE* pBytes = (BYTE*)malloc(dwFileSize);
	BYTE* pByteReadHead = pBytes;
	if (pBytes == nullptr)
	{	// couldn't allocate space to read from
		return;
	}

	MPQ_ReadFile((D2MPQArchive*)pImage->mpq, pImage->f, pBytes, dwFileSize);

	memcpy(&pImage->header, pBytes, sizeof(pImage->header));
	pBytes += sizeof(pImage->header);

	free(pBytes);
}

/*
 *	Frees a DC6 resource
 */
void DC6_UnloadImage(DC6Image* pImage)
{
	free(pImage);
}