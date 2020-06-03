#pragma once

#include "../Shared/D2Shared.hpp"

/*
 *	EVERYTHING TO DO WITH MPQ FILES
 *	The structure containing data about MPQ files
 *	Pieced together with code from Tom Amigo, Paul Siramy, and Zezula
 *	@author	eezstreet
 */

 // Compression types for multiple compressions
#define MPQ_COMPRESSION_HUFFMANN          0x01u  // Huffmann compression (used on WAVE files only)
#define MPQ_COMPRESSION_PKWARE            0x08u  // PKWARE DCL compression
#define MPQ_COMPRESSION_ADPCM_MONO        0x40u  // IMA ADPCM compression (mono)
#define MPQ_COMPRESSION_ADPCM_STEREO      0x80u  // IMA ADPCM compression (stereo)
#define MPQ_COMPRESSION_NEXT_SAME   0xFFFFFFFFu  // Same compression

#define MPQ_FILE_IMPLODE            0x00000100u  // Implode method (By PKWARE Data Compression Library)
#define MPQ_FILE_COMPRESS           0x00000200u  // Compress methods (By multiple methods)
#define MPQ_FILE_ENCRYPTED          0x00010000u  // Indicates whether file is encrypted
#define MPQ_FILE_FIX_KEY            0x00020000u  // File decryption key has to be fixed
#define MPQ_FILE_PATCH_FILE         0x00100000u  // The file is a patch file. Raw file data begin with TPatchInfo structure
#define MPQ_FILE_SINGLE_UNIT        0x01000000u  // File is stored as a single unit, not sectors
#define MPQ_FILE_DELETE_MARKER      0x02000000u  // File is a deletion marker.
#define MPQ_FILE_SECTOR_CRC         0x04000000u  // File has checksums for each sector.
 // Ignored if file is not compressed or imploded.
#define MPQ_FILE_SIGNATURE          0x10000000u  // Present on STANDARD.SNP\(signature). The only occurence ever observed
#define MPQ_FILE_EXISTS             0x80000000u  // Set if file exists, reset when the file was deleted
#define MPQ_FILE_REPLACEEXISTING    0x80000000u  // Replace when the file exist (SFileAddFile)

#define MPQ_FILE_COMPRESS_MASK      0x0000FF00u  // Mask for a file being compressed

#define MPQ_FILE_DEFAULT_INTERNAL   0xFFFFFFFFu  // Use default flags for internal files

#define MPQ_FILE_VALID_FLAGS     (MPQ_FILE_IMPLODE       |  \
                                  MPQ_FILE_COMPRESS      |  \
                                  MPQ_FILE_ENCRYPTED     |  \
                                  MPQ_FILE_FIX_KEY       |  \
                                  MPQ_FILE_PATCH_FILE    |  \
                                  MPQ_FILE_SINGLE_UNIT   |  \
                                  MPQ_FILE_DELETE_MARKER |  \
                                  MPQ_FILE_SECTOR_CRC    |  \
                                  MPQ_FILE_SIGNATURE     |  \
                                  MPQ_FILE_EXISTS)

// Decryption keys for MPQ tables - thanks Zezula
#define MPQ_KEY_HASH_TABLE          0xC3AF3770u  // Obtained by HashString("(hash table)", MPQ_HASH_FILE_KEY)
#define MPQ_KEY_BLOCK_TABLE         0xEC83B3A3u  // Obtained by HashString("(block table)", MPQ_HASH_FILE_KEY)

#pragma pack(push,enter_include)
#pragma pack(1)
/*
 *	@author Zezula
 */
struct MPQHash
{
	DWORD	dwMethodA;			// Hash (method A)
	DWORD	dwMethodB;			// Hash (method B)
	WORD	wLocale;
	BYTE	nPlatform;
	BYTE	nReserved;
	DWORD	dwBlockEntry;
};

/*
 *	@author	Zezula
 */
struct MPQBlock
{
	DWORD dwFilePos;	// Offset from the beginning of the file
	DWORD dwCSize;		// Compressed file size
	DWORD dwFSize;		// Uncompressed file size (0 if not a file)
	DWORD dwFlags;		// File flags
};
#pragma pack(pop,enter_include)

// @author eezstreet
typedef char MPQName[MAX_D2PATH];

/*
 *	@author eezstreet
 */
struct D2MPQArchive
{
	bool			bOpen;					// Whether the MPQ archive is currently open / has no errors
	DWORD			dwDataOffset;			// Offset to MPQ file data
	DWORD			dwHashOffset;			// Offset to MPQ hash table
	DWORD			dwBlockOffset;			// Offset to MPQ block table
	DWORD			dwArchiveSize;			// Length of MPQ file data
	DWORD			dwNumHashEntries;		// Length of MPQ hash table
	DWORD			dwNumBlockEntries;		// Length of MPQ block table
	MPQHash*		pHashTable;				// Pointer to MPQ hash table
	MPQBlock*		pBlockTable;			// Pointer to MPQ block table
	DWORD			dwFileCount;			// Number of files in MPQ
	WORD			wSectorSize;			// The size of MPQ sectors (always 0x200)
	DWORD			dwSlackSpace[0x500];
	DWORD*			pSectorOffsets;
	DWORD			dwSectorCount;

	MPQName*		pNameTable;				// Names of each file in the MPQ. Only filled as used.
	fs_handle		f;						// FS file handle
};

// MPQ.cpp
namespace MPQ
{
	void OpenMPQ(const char *szMPQPath, const char* szMPQName, D2MPQArchive* pMPQ);
	void CloseMPQ(D2MPQArchive* pMPQ);
	fs_handle FetchHandle(D2MPQArchive* pMPQ, const char* szFileName);
	size_t FileSize(D2MPQArchive* pMPQ, fs_handle fFile);
	size_t ReadFile(D2MPQArchive* pMPQ, fs_handle fFile, BYTE* buffer, size_t dwBufferLen);
	void Cleanup();
}
