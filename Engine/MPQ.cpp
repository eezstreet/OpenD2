#include "MPQ.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"
#include <memory>
#include <assert.h>
#include "../Libraries/adpcm/adpcm.h"
#include "../Libraries/huffman/huff.h"
#include "../Libraries/pkware/pklib.h"

#define MPQ_HASH_TABLE_INDEX    0x000
#define MPQ_HASH_NAME_A         0x100
#define MPQ_HASH_NAME_B         0x200
#define MPQ_HASH_FILE_KEY       0x300
#define MPQ_HASH_KEY2_MIX       0x400

namespace MPQ
{

	static char* gszListfile = "(listfile)";
	static char* gszHashTable = "(hash table)";
	static char* gszBlockTable = "(block table)";

	// Converts ASCII characters to uppercase. Converts slashes into backslashes.
	static unsigned char AsciiToUpperTable[256] =
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x5C,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	};

	// Converts ASCII characters to uppercase. Does NOT convert slashes into backslashes.
	static unsigned char AsciiToUpperTable_Slash[256] =
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	};

	static DWORD gdwFileHeaderBuffer[0x5A000];

	/*
	 *	Finds the header of the MPQ file.
	 *	In Diablo II, these bytes are always 4D 50 51 1A 20 00 00 00
	 *										 "M P Q"
	 *	Some important information follows this, like the number of files and so on.
	 */

#ifdef D2_BIG_ENDIAN
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
		WORD wVersion = 0;

		FS::Read(pMPQ->f, &dwFirstByte);
		FS::Read(pMPQ->f, &dwSecondByte);

		Log_ErrorAssertReturn(dwFirstByte == gdwFirstHeader && dwSecondByte == gdwSecondHeader, false);

		// Read length of file data
		FS::Read(pMPQ->f, &pMPQ->dwArchiveSize);

		// Read the version number. This HAS to be 0.
		FS::Read(pMPQ->f, &wVersion, sizeof(WORD));
		Log_ErrorAssertReturn(wVersion == 0, false);

		// Read sector size
		FS::Read(pMPQ->f, &pMPQ->wSectorSize, sizeof(WORD));
		pMPQ->wSectorSize = 0x1000;	// magic

		// Read offset to hash table
		FS::Read(pMPQ->f, &pMPQ->dwHashOffset);

		// Read offset to block table
		FS::Read(pMPQ->f, &pMPQ->dwBlockOffset);

		// Read length of hash table
		FS::Read(pMPQ->f, &pMPQ->dwNumHashEntries);

		// Read length of block table
		FS::Read(pMPQ->f, &pMPQ->dwNumBlockEntries);

		pMPQ->dwFileCount = pMPQ->dwNumBlockEntries; // the number of files is equivalent to the number of blocks (?)

		return true;
	}

	/*
	 *	Hashes a filename string.
	 *	Converts slashes into backslashes.
	 *	@author	Zezula
	 */
	static DWORD HashString(D2MPQArchive* pMPQ, const char * szFileName, DWORD dwHashType)
	{
		DWORD  dwSeed1 = 0x7FED7FED;
		DWORD  dwSeed2 = 0xEEEEEEEE;
		DWORD  ch;

		while (*szFileName != 0)
		{
			// Convert the input character to uppercase
			// DON'T convert slash (0x2F) to backslash (0x5C)
			ch = AsciiToUpperTable[*szFileName++];

			dwSeed1 = pMPQ->dwSlackSpace[dwHashType + ch] ^ (dwSeed1 + dwSeed2);
			dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
		}

		return dwSeed1;
	}

	/*
	 *	Hashes a filename string.
	 *	Does NOT convert slashes to backslashes.
	 *	@author	Zezula
	 */
	static DWORD HashStringSlash(D2MPQArchive* pMPQ, const char * szFileName, DWORD dwHashType)
	{
		DWORD  dwSeed1 = 0x7FED7FED;
		DWORD  dwSeed2 = 0xEEEEEEEE;
		DWORD  ch;

		while (*szFileName != 0)
		{
			// Convert the input character to uppercase
			// DON'T convert slash (0x2F) to backslash (0x5C)
			ch = AsciiToUpperTable_Slash[*szFileName++];

			dwSeed1 = pMPQ->dwSlackSpace[dwHashType + ch] ^ (dwSeed1 + dwSeed2);
			dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
		}

		return dwSeed1;
	}

	/*
	 *	Determine file key
	 *	@author	Zezula
	 */
	DWORD DecryptFileKey(D2MPQArchive* pMPQ, char * szFileName, unsigned long long MpqPos, DWORD dwFileSize, DWORD dwFlags)
	{
		DWORD dwFileKey;
		DWORD dwMpqPos = (DWORD)MpqPos;

		// File key is calculated from plain name
		szFileName = D2Lib::fnbld(szFileName);
		dwFileKey = HashString(pMPQ, szFileName, MPQ_HASH_FILE_KEY);

		// Fix the key, if needed
		if (dwFlags & MPQ_FILE_FIX_KEY)
			dwFileKey = (dwFileKey + dwMpqPos) ^ dwFileSize;

		// Return the key
		return dwFileKey;
	}

	/*
	 *	Attempt to determine a file's encryption key based on expected file content.
	 *	@author	Zezula
	 */
	DWORD DetectFileKeyByKnownContent(void *pvEncryptedData, DWORD* pdwSlackSpace, DWORD dwDecrypted0, DWORD dwDecrypted1)
	{
		DWORD* EncryptedData = (DWORD*)pvEncryptedData;
		DWORD dwKey1PlusKey2;
		DWORD DataBlock[2];

		// Get the value of the combined encryption key
		dwKey1PlusKey2 = (EncryptedData[0] ^ dwDecrypted0) - 0xEEEEEEEE;

		// Try all 256 combinations of dwKey1
		for (DWORD i = 0; i < 0x100; i++)
		{
			DWORD dwSaveKey1;
			DWORD dwKey1 = dwKey1PlusKey2 - pdwSlackSpace[MPQ_HASH_KEY2_MIX + i];
			DWORD dwKey2 = 0xEEEEEEEE;

			// Modify the second key and decrypt the first DWORD
			dwKey2 += pdwSlackSpace[MPQ_HASH_KEY2_MIX + (dwKey1 & 0xFF)];
			DataBlock[0] = EncryptedData[0] ^ (dwKey1 + dwKey2);

			// Did we obtain the same value like dwDecrypted0?
			if (DataBlock[0] == dwDecrypted0)
			{
				// Save this key value
				dwSaveKey1 = dwKey1;

				// Rotate both keys
				dwKey1 = ((~dwKey1 << 0x15) + 0x11111111) | (dwKey1 >> 0x0B);
				dwKey2 = DataBlock[0] + dwKey2 + (dwKey2 << 5) + 3;

				// Modify the second key again and decrypt the second DWORD
				dwKey2 += pdwSlackSpace[MPQ_HASH_KEY2_MIX + (dwKey1 & 0xFF)];
				DataBlock[1] = EncryptedData[1] ^ (dwKey1 + dwKey2);

				// Now compare the results
				if (DataBlock[1] == dwDecrypted1)
					return dwSaveKey1;
			}
		}

		// Key not found
		return 0;
	}

	/*
	 *	Attempt to determine a file's decryption key by deduction based on its type
	 *	@author	Zezula
	 */
#define MPQ_WAV_KEY		0x46464952
#define MPQ_EXE_KEY1	0x00905A4D
#define MPQ_EXE_KEY2	0x00000003
#define MPQ_XML_KEY1	0x6D783F3C
#define MPQ_XML_KEY2	0x6576206C
	DWORD DetectFileKeyByContent(void *pvEncryptedData, DWORD* pdwSlackSpace, DWORD dwSectorSize, DWORD dwFileSize)
	{
		DWORD dwFileKey = 0;

		if (dwSectorSize >= 0x0C)
		{	// It's probably a WAV file.
			dwFileKey = DetectFileKeyByKnownContent(pvEncryptedData, pdwSlackSpace, MPQ_WAV_KEY, dwFileSize - 8);
			if (dwFileKey != 0)
			{
				return dwFileKey;
			}
		}

		if (dwSectorSize >= 0x40)
		{	// It's probably an EXE file.
			dwFileKey = DetectFileKeyByKnownContent(pvEncryptedData, pdwSlackSpace, MPQ_EXE_KEY1, MPQ_EXE_KEY2);
			if (dwFileKey != 0)
			{
				return dwFileKey;
			}
		}

		if (dwSectorSize >= 0x04)
		{	// It's probably an XML file.
			dwFileKey = DetectFileKeyByKnownContent(pvEncryptedData, pdwSlackSpace, MPQ_XML_KEY1, MPQ_XML_KEY2);
			if (dwFileKey != 0)
			{
				return dwFileKey;
			}
		}

		// Not found, sorry.
		return dwFileKey;
	}

	/*
	 *	Decrypt an MPQ block
	 *	@author	Zezula
	 */
	static void DecryptMPQBlock(D2MPQArchive* pArchive, void * pvDataBlock, DWORD dwLength, DWORD dwKey1)
	{
		DWORD* DataBlock = (DWORD*)pvDataBlock;
		DWORD dwValue32;
		DWORD dwKey2 = 0xEEEEEEEE;

		// Round to DWORDs
		dwLength >>= 2;

		// Decrypt the data block at array of DWORDs
		for (DWORD i = 0; i < dwLength; i++)
		{
			// Modify the second key
			dwKey2 += pArchive->dwSlackSpace[MPQ_HASH_KEY2_MIX + (dwKey1 & 0xFF)];

			DataBlock[i] = DataBlock[i] ^ (dwKey1 + dwKey2);
			dwValue32 = DataBlock[i];

			dwKey1 = ((~dwKey1 << 0x15) + 0x11111111) | (dwKey1 >> 0x0B);
			dwKey2 = dwValue32 + dwKey2 + (dwKey2 << 5) + 3;
		}
	}

	/*
	 *	Initialize MPQ scratch space
	 *	@author Zezula
	 */
	static void InitScratchSpace(D2MPQArchive* pMPQ)
	{
		// FIXME: precalculate this table and copy it to memory
		DWORD dwSeed = 0x00100001;
		DWORD index1 = 0;
		DWORD index2 = 0;
		int   i;

		for (index1 = 0; index1 < 0x100; index1++)
		{
			for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
			{
				DWORD temp1, temp2;

				dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
				temp1 = (dwSeed & 0xFFFF) << 0x10;

				dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
				temp2 = (dwSeed & 0xFFFF);

				pMPQ->dwSlackSpace[index2] = (temp1 | temp2);
			}
		}
	}

	/*
	 *	Find a block index given a hash
	 */
	static inline DWORD GetHashBlockIndex(MPQHash* pHash)
	{
		return pHash->dwBlockEntry & 0x0FFFFFFF;
	}

	/*
	 *	Check whether a hash entry is valid
	 *	Only call this function on MPQ init
	 */
	static bool ValidHashEntry(D2MPQArchive* pMPQ, MPQHash* pHash, MPQBlock* pBlockTable)
	{
		DWORD dwBlockIndex = GetHashBlockIndex(pHash);
		if (dwBlockIndex >= pMPQ->dwNumBlockEntries)
		{	// This hash entry has a higher block entry than the number of blocks in the file..
			return false;
		}

		MPQBlock* pBlock = &pBlockTable[dwBlockIndex];
		if (!(pBlock->dwFlags & MPQ_FILE_EXISTS))
		{	// This block says it doesn't exist! I'm skeptical at best...
			return false;
		}

		DWORD dwOffset = pBlock->dwFilePos;
		if (dwOffset > pMPQ->dwArchiveSize)
		{	// This block has an offset that's outside the file
			return false;
		}

		return true;
	}

	/*
	 *	Allocate memory for (and then read) the MPQ's block and hash tables.
	 */
	static bool AllocateComputeMPQBlockHash(D2MPQArchive* pMPQ)
	{
		// Initialize the scratch space
		InitScratchSpace(pMPQ);

		// Allocate, read and decrypt hash table
		pMPQ->pHashTable = (MPQHash*)malloc(sizeof(MPQHash) * pMPQ->dwNumHashEntries);
		if (pMPQ->pHashTable == nullptr)
		{
			return false; // ran out of memory - throw error?
		}
		FS::Seek(pMPQ->f, pMPQ->dwHashOffset, FS_SEEK_SET);
		FS::Read(pMPQ->f, pMPQ->pHashTable, sizeof(MPQHash), pMPQ->dwNumHashEntries);
		DecryptMPQBlock(pMPQ, pMPQ->pHashTable, sizeof(MPQHash) * pMPQ->dwNumHashEntries, MPQ_KEY_HASH_TABLE);

		// Allocate, read and decrypt block table
		pMPQ->pBlockTable = (MPQBlock*)malloc(sizeof(MPQBlock) * pMPQ->dwNumBlockEntries);
		if (pMPQ->pBlockTable == nullptr)
		{
			return false; // ran out of memory - throw error?
		}
		FS::Seek(pMPQ->f, pMPQ->dwBlockOffset, FS_SEEK_SET);
		FS::Read(pMPQ->f, pMPQ->pBlockTable, sizeof(MPQBlock), pMPQ->dwNumBlockEntries);
		DecryptMPQBlock(pMPQ, pMPQ->pBlockTable, sizeof(MPQBlock) * pMPQ->dwNumBlockEntries, MPQ_KEY_BLOCK_TABLE);

		// Allocate name table
		pMPQ->pNameTable = (MPQName*)calloc(pMPQ->dwNumBlockEntries, sizeof(MPQName));
		memset(pMPQ->pNameTable, 0, sizeof(MPQName) * pMPQ->dwNumBlockEntries);

		return true; // all went well
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

		memset(pMPQ, 0, sizeof(D2MPQArchive));

		DWORD dwMPQSize = FS::Open(szMPQPath, &pMPQ->f, FS_READ, true);
		if (!dwMPQSize || pMPQ->f == INVALID_HANDLE)
		{	// couldn't load MPQ - throw error?
			return;
		}

		if (!ParseMPQHeader(pMPQ))
		{	// couldn't parse header - throw error?
			return;
		}

		if (!AllocateComputeMPQBlockHash(pMPQ))
		{	// couldn't allocate MPQ block and hash tables - throw error?
			return;
		}

		// ready to go
		pMPQ->bOpen = true;
	}

	/*
	 *	Closes an MPQ
	 *	@author	eezstreet
	 */
	void CloseMPQ(D2MPQArchive* pMPQ)
	{
		if (!pMPQ || !pMPQ->bOpen)
		{
			return;
		}
		// Close the file handle
		FS::CloseFile(pMPQ->f);

		// Free the hash, name and block tables
		free(pMPQ->pHashTable);
		free(pMPQ->pBlockTable);
		free(pMPQ->pNameTable);

		// Free sector offset table (if present)
		if (pMPQ->pSectorOffsets != nullptr)
		{
			free(pMPQ->pSectorOffsets);
		}
	}

	/*
	 *	Retrieves a file number (file handle) from the archive
	 *	@author	Zezula/eezstreet
	 */
	fs_handle FetchHandle(D2MPQArchive* pMPQ, const char* szFileName)
	{
		DWORD dwHashIndexMask = pMPQ->dwNumHashEntries - 1;
		DWORD dwStartIndex = HashStringSlash(pMPQ, szFileName, MPQ_HASH_TABLE_INDEX);
		DWORD dwName1 = HashStringSlash(pMPQ, szFileName, MPQ_HASH_NAME_A);
		DWORD dwName2 = HashStringSlash(pMPQ, szFileName, MPQ_HASH_NAME_B);
		DWORD dwIndex;

		if (!pMPQ->bOpen)
		{	// Didn't load, don't try to load from it
			return (fs_handle)-1;
		}

		dwStartIndex = dwIndex = (dwStartIndex & dwHashIndexMask);

		while (true)
		{
			MPQHash* pHash = &pMPQ->pHashTable[dwIndex];
			DWORD dwBlockIndex = GetHashBlockIndex(pHash);

			if (pHash->dwBlockEntry == 0xFFFFFFFF)
			{
				// Nonexistent hash entry = we reached the end of the list (somehow)
				return INVALID_HANDLE;
			}

			if (pHash->dwMethodA == dwName1 && pHash->dwMethodB == dwName2 && dwBlockIndex < pMPQ->dwNumBlockEntries)
			{
				// Copy name to name table
				D2Lib::strncpyz(pMPQ->pNameTable[dwBlockIndex], szFileName, MAX_D2PATH);
				return (fs_handle)dwBlockIndex;
			}

			// Both of the hashes don't match, which means we have hit a hash table collision that wasn't our file
			// In that case, continue walking along the collision chain
			dwIndex = (dwIndex + 1) & dwHashIndexMask;
			if (dwIndex == dwStartIndex)
			{	// wrapped around
				return INVALID_HANDLE;
			}
		}

		return INVALID_HANDLE;
	}

	/*
	 *	Retrieves the (decompressed) size of a file in an archive
	 *	@author	eezstreet
	 */
	size_t FileSize(D2MPQArchive* pMPQ, fs_handle fFile)
	{
		// Check for invalid handle
		if (fFile == (fs_handle)-1)
		{
			return 0;
		}

		return pMPQ->pBlockTable[fFile].dwFSize;
	}

///////////////////////////////////////////////////////////////////////////////////
//
// MPQ COMPRESSION

	/*
	 *	Compression functions
	 *	@author Zezula / Paul Siramy / eezstreet
	 */

	typedef struct
	{
		unsigned char * pbInBuff;           // Pointer to input data buffer
		unsigned char * pbInBuffEnd;        // End of the input buffer
		unsigned char * pbOutBuff;          // Pointer to output data buffer
		unsigned char * pbOutBuffEnd;       // Pointer to output data buffer
	} TDataInfo;

	static unsigned int ReadPKWare(char* buf, unsigned int* size, void* param)
	{
		TDataInfo * pInfo = (TDataInfo *)param;
		unsigned int nMaxAvail = (unsigned int)(pInfo->pbInBuffEnd - pInfo->pbInBuff);
		unsigned int nToRead = *size;

		// Check the case when not enough data available
		if (nToRead > nMaxAvail)
			nToRead = nMaxAvail;

		// Load data and increment offsets
		memcpy(buf, pInfo->pbInBuff, nToRead);
		pInfo->pbInBuff += nToRead;
		assert(pInfo->pbInBuff <= pInfo->pbInBuffEnd);
		return nToRead;
	}

	static void WritePKWare(char* buf, unsigned int* size, void* param)
	{
		TDataInfo * pInfo = (TDataInfo *)param;
		unsigned int nMaxWrite = (unsigned int)(pInfo->pbOutBuffEnd - pInfo->pbOutBuff);
		unsigned int nToWrite = *size;

		// Check the case when not enough space in the output buffer
		if (nToWrite > nMaxWrite)
			nToWrite = nMaxWrite;

		// Write output data and increments offsets
		memcpy(pInfo->pbOutBuff, buf, nToWrite);
		pInfo->pbOutBuff += nToWrite;
		assert(pInfo->pbOutBuff <= pInfo->pbOutBuffEnd);
	}

	static void MPQDecompress_PKWare(void* pInBuffer, DWORD* dwInRead, void* pOutBuffer, DWORD* dwOutRead)
	{
		TDataInfo Info;                             // Data information
		char * work_buf = (char*)malloc(EXP_BUFFER_SIZE);// Pklib's work buffer

														 // Handle no-memory condition
		if (work_buf == NULL)
			return;

		// Fill data information structure
		memset(work_buf, 0, EXP_BUFFER_SIZE);
		Info.pbInBuff = (unsigned char *)pInBuffer;
		Info.pbInBuffEnd = (unsigned char *)pInBuffer + *dwInRead;
		Info.pbOutBuff = (unsigned char *)pOutBuffer;
		Info.pbOutBuffEnd = (unsigned char *)pOutBuffer + *dwOutRead;

		// Do the decompression
		explode(ReadPKWare, WritePKWare, work_buf, &Info);

		// If PKLIB is unable to decompress the data, return 0;
		if (Info.pbOutBuff == pOutBuffer)
		{
			free(work_buf);
			return;
		}

		// Give away the number of decompressed bytes
		*dwOutRead = (int)(Info.pbOutBuff - (unsigned char *)pOutBuffer);
		free(work_buf);
	}

	static void MPQDecompress_Huffmann(void* pInBuffer, DWORD* dwInRead, void* pOutBuffer, DWORD* dwOutRead)
	{
		THuffmannTree ht(false);
		TInputStream is(pInBuffer, *dwInRead);

		*dwOutRead = ht.Decompress(pOutBuffer, *dwOutRead, &is);
	}

	static void MPQDecompress_PCMMono(void* pInBuffer, DWORD* dwInRead, void* pOutBuffer, DWORD* dwOutRead)
	{
		*dwOutRead = ::DecompressADPCM(pOutBuffer, *dwOutRead, pInBuffer, *dwInRead, 1);
	}

	static void MPQDecompress_PCMStereo(void* pInBuffer, DWORD* dwInRead, void* pOutBuffer, DWORD* dwOutRead)
	{
		*dwOutRead = ::DecompressADPCM(pOutBuffer, *dwOutRead, pInBuffer, *dwInRead, 2);
	}

	/*
	 *	Compression table maps compression methods to functions to call to compress the data
	 */
	struct D2MPQCompression {
		BYTE nCompressionType;
		void(*pFunc)(void* pInBuffer, DWORD* dwInPos, void* pOutBuffer, DWORD* dwOutPos);
	};

	static D2MPQCompression CompressionModels[] = {
		{MPQ_COMPRESSION_PKWARE, MPQDecompress_PKWare},
		{MPQ_COMPRESSION_HUFFMANN, MPQDecompress_Huffmann},
		{MPQ_COMPRESSION_ADPCM_MONO, MPQDecompress_PCMMono},
		{MPQ_COMPRESSION_ADPCM_STEREO, MPQDecompress_PCMStereo},
		{0, nullptr},
	};

	/*
	 *	Reads a file from an archive into a memory buffer
	 *	@author	Paul Siramy/eezstreet
	 */
	static char* gpTempBuffer = nullptr;
	size_t gpTempBufferSize = 0;

	size_t ReadFile(D2MPQArchive* pMPQ, fs_handle fFile, BYTE* buffer, DWORD dwBufferLen)
	{
		DWORD dwNumBlocks;
		size_t dwTotalAmountRead = 0;
		DWORD dwBufferFilled = 0;
		DWORD dwEncryptionKey = 0;

		char* pTempBuffer;

		if (fFile == (fs_handle)-1)
		{	// invalid file
			return 0;
		}

		if (!pMPQ || pMPQ->f == (fs_handle)-1)
		{	// bad MPQ pointer or MPQ is not opened
			return 0;
		}

		if (!buffer || dwBufferLen <= 0)
		{	// bad buffer or buffer length
			return 0;
		}

		MPQBlock* pBlock = &pMPQ->pBlockTable[fFile];

		if (dwBufferLen < pBlock->dwFSize)
		{	// not enough room in the buffer to fit the file - should probably complain about this
			return 0;
		}

		// We use a global buffer to cut down on the number of memory allocations
		// This is important if we're loading lots of files at once (ie, palettes)
		if (gpTempBuffer == nullptr)
		{
			gpTempBufferSize = pBlock->dwFSize;
			gpTempBuffer = (char*)malloc(gpTempBufferSize);
			if (gpTempBuffer == nullptr)
			{
				// global buffer couldn't be allocated. die?
				return 0;
			}
		}
		else if (gpTempBufferSize < pBlock->dwFSize)
		{
			gpTempBufferSize = pBlock->dwFSize;
			char* temp = (char*)realloc(gpTempBuffer, gpTempBufferSize);
			if (temp == nullptr)
			{
				free(gpTempBuffer);
				gpTempBuffer = nullptr;
				return 0;
			}
			gpTempBuffer = temp;
		}

		pTempBuffer = gpTempBuffer;

		if (pBlock->dwFlags & MPQ_FILE_ENCRYPTED || pBlock->dwFlags & MPQ_FILE_FIX_KEY)
		{
			// Decrypt the key 
			dwEncryptionKey = DecryptFileKey(pMPQ, pMPQ->pNameTable[fFile], pBlock->dwFilePos, pBlock->dwFSize, pBlock->dwFlags);
		}

		if (pBlock->dwFlags & MPQ_FILE_IMPLODE || pBlock->dwFlags & MPQ_FILE_COMPRESS)
		{	// Compressed file. Around 90% of the blocks are compressed in this manner.
			dwNumBlocks = ((pBlock->dwFSize - 1) / pMPQ->wSectorSize) + 2;

			// Read the sector header to determine what we need to read
			FS::Seek(pMPQ->f, pBlock->dwFilePos, FS_SEEK_SET);
			FS::Read(pMPQ->f, gdwFileHeaderBuffer, sizeof(DWORD), dwNumBlocks);

			// If this is an encrypted file, we need to fix the header
			if (pBlock->dwFlags & MPQ_FILE_ENCRYPTED)
			{
				// For this one, we use the encryption key - 1.
				// Don't know why, just roll with it. Probably a bug on Blizzard's part.
				DecryptMPQBlock(pMPQ, gdwFileHeaderBuffer, dwNumBlocks << 2, dwEncryptionKey - 1);
			}

			for (int i = 0; i < dwNumBlocks - 1; i++)
			{
				DWORD dwBlockLengthRead = gdwFileHeaderBuffer[i + 1] - gdwFileHeaderBuffer[i];
				DWORD dwInputLength;
				BYTE nMethod;
				size_t dwAmountRead = FS::Read(pMPQ->f, pTempBuffer, sizeof(BYTE), dwBlockLengthRead);

				// Decrypt it!
				if (pBlock->dwFlags & MPQ_FILE_ENCRYPTED)
				{
					DecryptMPQBlock(pMPQ, pTempBuffer, dwAmountRead, dwEncryptionKey + i);
				}

				if (dwAmountRead == pMPQ->wSectorSize
					|| (i == dwNumBlocks - 2 && dwAmountRead == (pBlock->dwFSize & (pMPQ->wSectorSize - 1))))
				{
					memcpy(buffer + dwTotalAmountRead, pTempBuffer, dwAmountRead);
					pTempBuffer += dwBlockLengthRead;
					dwTotalAmountRead += dwBlockLengthRead;
					continue;
				}

				if (pBlock->dwCSize < pBlock->dwFSize)
				{
					// Only decompress if the compress size is smaller than the real size
					if (pBlock->dwFlags & MPQ_FILE_IMPLODE)
					{	// Diablo 1 style compression (PKWARE)
						nMethod = MPQ_COMPRESSION_PKWARE;
					}
					else if (pBlock->dwFlags & MPQ_FILE_COMPRESS)
					{	// StarCraft and Diablo 2 style compression (mixed method)
						nMethod = *(pTempBuffer);
						pTempBuffer++;
						dwBlockLengthRead--;
					}

					dwInputLength = dwBlockLengthRead;
					for (int j = 0; CompressionModels[j].pFunc != nullptr; j++)
					{
						if (nMethod & CompressionModels[j].nCompressionType)
						{
							dwBufferFilled = dwBufferLen;
							CompressionModels[j].pFunc(pTempBuffer, &dwInputLength, buffer + dwTotalAmountRead, &dwBufferFilled);
							// Pipe previous output into new input
							memcpy(pTempBuffer, buffer + dwTotalAmountRead, dwBufferFilled);
							dwInputLength = dwBufferFilled;
						}
					}
				}

				pTempBuffer += dwBlockLengthRead;
				dwTotalAmountRead += dwBufferFilled;
			}
		}
		else
		{	// Uncompressed file - Very few files are uncompressed but some (like Druid and Assassin character animations) are.
			DWORD dwFullBlocks, dwPartialBlock;
			DWORD dwFileSize = pBlock->dwCSize;
			DWORD dwReadSize = 0x60000;

			if (dwFileSize < dwReadSize)
			{	// This last block is very short, we need to only read that part
				dwReadSize = dwFileSize;
			}

			// Compute the number of blocks that we need to read
			dwPartialBlock = dwFileSize % dwReadSize;
			dwFullBlocks = (dwFileSize - dwPartialBlock) / dwReadSize;

			// Seek to the part of the MPQ in question
			FS::Seek(pMPQ->f, pBlock->dwFilePos, FS_SEEK_SET);

			// Read all of the full blocks
			for (DWORD j = 0; j < dwFullBlocks; j++)
			{
				FS::Read(pMPQ->f, buffer + dwTotalAmountRead, dwReadSize);
				dwTotalAmountRead += dwReadSize;
			}

			// Read the remaining partial block
			FS::Read(pMPQ->f, buffer + dwTotalAmountRead, dwBufferLen - dwTotalAmountRead);
			dwTotalAmountRead += dwPartialBlock;
		}

		return dwTotalAmountRead;
	}

	/*
	 * Frees the temporary block decompression buffer
	 */
	void Cleanup()
	{
		free(gpTempBuffer);
		gpTempBuffer = nullptr;
		gpTempBufferSize = 0;
	}
}
