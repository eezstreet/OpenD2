#include "Diablo2.hpp"
#include <memory>

static char* gszListfile = "(listfile)";
static char* gszHashTable = "(hash table)";
static char* gszBlockTable = "(block table)";

/*
 *	Finds the header of the MPQ file.
 *	In Diablo II, these bytes are always 4D 50 51 1A 20 00 00 00
 *										 "M P Q"
 *	Some important information follows this, like the number of files and so on.
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
	size_t dwOffset;
	WORD wVersion = 0;

	FS_Read(pMPQ->f, &dwFirstByte);
	FS_Read(pMPQ->f, &dwSecondByte);

	if (dwFirstByte != gdwFirstHeader || dwSecondByte != gdwSecondHeader)
	{
		return false;
	}

	// Read length of file data
	FS_Read(pMPQ->f, &pMPQ->dwDataLength);
	
	// Read the version number. This HAS to be 0.
	FS_Read(pMPQ->f, &wVersion, sizeof(WORD));
	if (wVersion != 0)
	{	// Burning Crusade (or newer version) MPQ detected!
		return false;
	}

	// Skip two bytes (we don't really care about the sector size, which is what's here)
	dwOffset = FS_Tell(pMPQ->f);
	FS_Seek(pMPQ->f, dwOffset + 2, SEEK_SET);

	// Read offset to hash table
	FS_Read(pMPQ->f, &pMPQ->dwHashOffset);

	// Read offset to block table
	FS_Read(pMPQ->f, &pMPQ->dwBlockOffset);

	// Read length of hash table
	FS_Read(pMPQ->f, &pMPQ->dwHashLength);

	// Read length of block table
	FS_Read(pMPQ->f, &pMPQ->dwBlockLength);

	// Fix some values
	pMPQ->dwFileCount = pMPQ->dwBlockLength; // the number of files is equivalent to the number of blocks (?)
	pMPQ->dwHashLength *= 4;	// block and hash length is in dwords length
	pMPQ->dwBlockLength *= 4;

	return true;
}

/*
 *	Allocate memory for (and then read) the MPQ's block and hash tables.
 */
static bool AllocateMPQBlockHash(D2MPQArchive* pMPQ)
{
	// Handle hash table
	pMPQ->pHashTable = (DWORD*)malloc(sizeof(DWORD) * pMPQ->dwHashLength);
	if (pMPQ->pHashTable == nullptr)
	{
		return false; // ran out of memory - throw error?
	}
	FS_Seek(pMPQ->f, pMPQ->dwHashOffset, FS_SEEK_SET);
	FS_Read(pMPQ->f, pMPQ->pHashTable, sizeof(DWORD), pMPQ->dwHashLength);

	// Handle block table
	pMPQ->pBlockTable = (DWORD*)malloc(sizeof(DWORD) * pMPQ->dwBlockLength);
	if (pMPQ->pBlockTable == nullptr)
	{
		return false; // ran out of memory - throw error?
	}
	FS_Seek(pMPQ->f, pMPQ->dwBlockOffset, FS_SEEK_SET);
	FS_Read(pMPQ->f, pMPQ->pBlockTable, sizeof(DWORD), pMPQ->dwBlockLength);

	return true; // all went well
}

/*
 *	Calculates a CRC32
 */
static DWORD CRC32(char* szStr, DWORD* pSlackSpace, DWORD dwOffset)
{
	char	byte;
	DWORD	crc = 0x7fed7fed;
	DWORD	s1 = 0xEEEEEEEE;

	byte = *szStr;
	while (byte) {
		if (byte>0x60 && byte<0x7B)
			byte -= 0x20;
		crc = *(pSlackSpace + dwOffset + byte) ^ (crc + s1);
		s1 += crc + (s1 << 5) + byte + 3;
		szStr++;
		byte = *szStr;
	}
	return crc;
}

/*
 *	Decodes a file in an MPQ
 */
static void MPQDecodeFile(DWORD* pData, D2MPQArchive* pMPQ, DWORD dwCRC, DWORD dwDataLength)
{
	DWORD	i, dec;
	DWORD	s1 = 0xEEEEEEEE;
	for (i = 0; i < dwDataLength; i++) {
		s1 += *(pMPQ->dwSlackSpace + 0x400 + (dwCRC & 0xFFL));
		dec = *(pData + i) ^ (s1 + dwCRC);
		s1 += dec + (s1 << 5) + 3;
		*(pData + i) = dec;
		dwCRC = (dwCRC >> 0x0b) | (0x11111111 + ((dwCRC ^ 0x7FFL) << 0x15));
	}
}

/*
 *	Computes the MPQ hash tables
 */
static bool ComputeMPQTables(D2MPQArchive* pMPQ)
{
	// This is some real black magic and I'm not totally sure how it works. 
	// Nevertheless, I've replicated Paul Siramy's (actually Tom Amigo's) code here.
	ldiv_t divres;
	DWORD s1;

	divres.rem = 0x100001;
	for (int i = 0; i < 0x100; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			divres = ldiv(divres.rem * 125 + 3, 0x002AAAAB);
			s1 = (divres.rem & 0xFFFFL) << 0x10;
			divres = ldiv(divres.rem * 125 + 3, 0x002AAAAB);
			s1 = s1 | (divres.rem & 0xFFFFL);
			pMPQ->dwSlackSpace[i + 0x100 * j] = s1;
		}
	}

	// Decode data in the files "(block table)" and "(hash table)"
	s1 = CRC32(gszHashTable, pMPQ->dwSlackSpace, 0x300);
	MPQDecodeFile(pMPQ->pHashTable, pMPQ, s1, pMPQ->dwHashLength);

	s1 = CRC32(gszBlockTable, pMPQ->dwSlackSpace, 0x300);
	MPQDecodeFile(pMPQ->pBlockTable, pMPQ, s1, pMPQ->dwBlockLength);

	return true;
}

/*
 *	Opens an MPQ at szMPQPath with name szMPQName.
 */
void MPQ_OpenMPQ(char* szMPQPath, const char* szMPQName, D2MPQArchive* pMPQ)
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

	if (!AllocateMPQBlockHash(pMPQ))
	{	// couldn't allocate MPQ block and hash tables - throw error?
		return;
	}

	if (!ComputeMPQTables(pMPQ))
	{	// couldn't decode the MPQ - throw error?
		return;
	}

	// ready to go
}

/*
 *	Closes an MPQ
 */
void MPQ_CloseMPQ(D2MPQArchive* pMPQ)
{
	if (!pMPQ || !pMPQ->bOpen)
	{
		return;
	}
	// Close the file handle
	FS_CloseFile(pMPQ->f);

	// Free the hash and block tables
	free(pMPQ->pHashTable);
	free(pMPQ->pBlockTable);
}

/*
 *	Retrieves a file number (file handle) from the archive
 *	@author	Paul Siramy/eezstreet
 */
fs_handle MPQ_FetchHandle(D2MPQArchive* pMPQ, char* szFileName)
{
	DWORD scrc1, scrc2, scrc3, pointer_ht;

	scrc1 = CRC32(szFileName, pMPQ->dwSlackSpace, 0x000);
	scrc2 = CRC32(szFileName, pMPQ->dwSlackSpace, 0x100);
	scrc3 = CRC32(szFileName, pMPQ->dwSlackSpace, 0x200);

	for (pointer_ht = (scrc1 & (pMPQ->dwHashLength / sizeof(DWORD) - 1)) * 4;
		pointer_ht < pMPQ->dwHashLength;
		pointer_ht += sizeof(DWORD))
	{
		if (pMPQ->pHashTable[pointer_ht] == scrc2 &&
			pMPQ->pHashTable[pointer_ht + 1] == scrc3)
		{
			return pMPQ->pHashTable[pointer_ht + 3];
		}
	}

	// FIXME: double check and make sure this doesn't cause problems later on.
	// I want this as 0 to remain consistent with other applications of fs_handle
	return 0;
}

/*
 *	Retrieves the size of a file in an archive
 */
size_t MPQ_FileSize(D2MPQArchive* pMPQ, fs_handle fFile)
{
	// Check for invalid handle
	if (fFile == 0)
	{
		return 0;
	}

	return pMPQ->pBlockTable[(fFile * 4) + 2]; // presumably offset because hash and block table files are the first?
}

/*
 *	Reads a file from an archive into a memory buffer
 */
size_t MPQ_ReadFile(D2MPQArchive* pMPQ, fs_handle fFile, BYTE* buffer, DWORD dwBufferLen)
{
	// Check for invalid handle
	if (fFile == 0)
	{
		return 0;
	}


}