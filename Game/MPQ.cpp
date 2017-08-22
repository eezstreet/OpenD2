#include "MPQ.hpp"

static const char* gszListfile = "(listfile)";
static const char* gszHashTable = "(hash table)";
static const char* gszBlockTable = "(block table)";

/*
 *	Finds the header of the MPQ file.
 *	In Diablo II, these bytes are always 4D 50 51 1A 20 00 00 00
 *										 "M P Q"
 */

#ifdef BIG_ENDIAN
static DWORD gdwFirstHeader = 0x4D50511A;
static DWORD gdwSecondHeader = 0x20000000;
#else
static DWORD gdwFirstHeader = 0x1A51504D;
static DWORD gdwSecondHeader = 0x00000020;
#endif

static bool ParseMPQHeader(D2MPQArchive* pMPQ)
{
	DWORD dwFirstByte;
	DWORD dwSecondByte;

	FS_Read(pMPQ->f, &dwFirstByte, sizeof(DWORD));
	FS_Read(pMPQ->f, &dwSecondByte, sizeof(DWORD));

	if (dwFirstByte != gdwFirstHeader || dwSecondByte)
	{
		return false;
	}

	return true;
}

/*
 *	Opens an MPQ at szMPQPath with name szMPQName.
 */
void OpenMPQ(char* szMPQPath, const char* szMPQName, D2MPQArchive* pMPQ)
{
	if (!pMPQ)
	{	// should never happen
		return;
	}

	DWORD dwMPQSize = FS_Open(szMPQPath, &pMPQ->f, FS_READ);
	if (!dwMPQSize || !pMPQ->f)
	{	// couldn't load MPQ - throw error?
		return;
	}

	if (!ParseMPQHeader(pMPQ))
	{	// couldn't parse header - throw error?
		return;
	}


}

/*
 *	Closes an MPQ
 */
void CloseMPQ(D2MPQArchive* pMPQ)
{
	if (pMPQ && pMPQ->bOpen)
	{
		FS_CloseFile(pMPQ->f);
	}
}