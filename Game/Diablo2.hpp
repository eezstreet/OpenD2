#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Libraries/sdl/SDL.h"

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
 *	The structure containing information about the system running the game
 */
struct D2SystemInfoStrc
{
	char szComputerName[64];
	char szOSName[128];
	char szProcessorVendor[32];
	char szProcessorModel[64];
	char szProcessorSpeed[64];	// maybe not present on linux?
	char szProcessorIdentifier[64];
	char szRAMPhysical[64];
	char szRAMVirtual[64];
	char szRAMPaging[64];
	char szWorkingDirectory[MAX_D2PATH_ABSOLUTE];
};

/*
 *	EVERYTHING TO DO WITH MPQ FILES
 *	The structure containing data about MPQ files
 *	@author Tom Amigo/Paul Siramy/eezstreet/Zezula
 */

 // Compression types for multiple compressions
#define MPQ_COMPRESSION_HUFFMANN          0x01  // Huffmann compression (used on WAVE files only)
#define MPQ_COMPRESSION_PKWARE            0x08  // PKWARE DCL compression
#define MPQ_COMPRESSION_ADPCM_MONO        0x40  // IMA ADPCM compression (mono)
#define MPQ_COMPRESSION_ADPCM_STEREO      0x80  // IMA ADPCM compression (stereo)
#define MPQ_COMPRESSION_NEXT_SAME   0xFFFFFFFF  // Same compression

#define MPQ_FILE_IMPLODE            0x00000100  // Implode method (By PKWARE Data Compression Library)
#define MPQ_FILE_COMPRESS           0x00000200  // Compress methods (By multiple methods)
#define MPQ_FILE_ENCRYPTED          0x00010000  // Indicates whether file is encrypted 
#define MPQ_FILE_FIX_KEY            0x00020000  // File decryption key has to be fixed
#define MPQ_FILE_PATCH_FILE         0x00100000  // The file is a patch file. Raw file data begin with TPatchInfo structure
#define MPQ_FILE_SINGLE_UNIT        0x01000000  // File is stored as a single unit, not sectors
#define MPQ_FILE_DELETE_MARKER      0x02000000  // File is a deletion marker.
#define MPQ_FILE_SECTOR_CRC         0x04000000  // File has checksums for each sector.
 // Ignored if file is not compressed or imploded.
#define MPQ_FILE_SIGNATURE          0x10000000  // Present on STANDARD.SNP\(signature). The only occurence ever observed
#define MPQ_FILE_EXISTS             0x80000000  // Set if file exists, reset when the file was deleted
#define MPQ_FILE_REPLACEEXISTING    0x80000000  // Replace when the file exist (SFileAddFile)

#define MPQ_FILE_COMPRESS_MASK      0x0000FF00  // Mask for a file being compressed

#define MPQ_FILE_DEFAULT_INTERNAL   0xFFFFFFFF  // Use default flags for internal files

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
#define MPQ_KEY_HASH_TABLE          0xC3AF3770  // Obtained by HashString("(hash table)", MPQ_HASH_FILE_KEY)
#define MPQ_KEY_BLOCK_TABLE         0xEC83B3A3  // Obtained by HashString("(block table)", MPQ_HASH_FILE_KEY)

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

	fs_handle		f;						// FS file handle
};

/*
 *	Renderer related structures
 */
struct D2Renderer
{
	//
	void		(*RF_Init)(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
	void		(*RF_Shutdown)();
	void		(*RF_Present)();

	//
	tex_handle	(*RF_RegisterTexture)(char* handleName, DWORD dwWidth, DWORD dwHeight);
	tex_handle	(*RF_TextureFromStitchedDC6)(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette);
	void		(*RF_SetTexturePixels)(tex_handle texture, BYTE* pPixels, int nPalette);
	void		(*RF_DrawTexture)(tex_handle texture, DWORD x, DWORD y, DWORD w, DWORD h, DWORD u, DWORD v);
	// TODO: version of RF_DrawTexture that handles palshifted cels
};

extern D2Renderer* RenderTarget;	// nullptr if there isn't a render target

/////////////////////////////////////////////////////////
//
//	Functions

// DC6.cpp
void DC6_LoadImage(char* szPath, DC6Image* pImage);
void DC6_UnloadImage(DC6Image* pImage);
BYTE* DC6_GetPixelsAtFrame(DC6Image* pImage, int nDirection, int nFrame, size_t* pNumPixels);
void DC6_PollFrame(DC6Image* pImage, DWORD nDirection, DWORD nFrame,
	DWORD* dwWidth, DWORD* dwHeight, DWORD* dwOffsetX, DWORD* dwOffsetY);
void DC6_StitchStats(DC6Image* pImage, 
	DWORD dwStart, DWORD dwEnd, DWORD* pWidth, DWORD* pHeight, DWORD* pTotalWidth, DWORD* pTotalHeight);

// Diablo2.cpp
int InitGame(int argc, char** argv, DWORD pid);

// Logging.cpp
void Log_InitSystem(const char* szLogHeader, const char* szGameName, OpenD2ConfigStrc* pOpenConfig);
void Log_Shutdown();
void Log_Print(OpenD2LogFlags nPriority, char* szFormat, ...);
void Log_Warning(char* szFile, int nLine, char* szCondition);
void Log_Error(char* szFile, int nLine, char* szCondition);
#define Log_WarnAssert(x, y) if(!(x)) { Log_Warning(__FILE__, __LINE__, "" #x); return y; }
#define Log_ErrorAssert(x, y) if(!(x)) { Log_Error(__FILE__, __LINE__, "" #x); return y; }

// Palette.cpp
bool Pal_Init();
D2Palette* Pal_GetPalette(int nIndex);

// Platform_*.cpp
void Sys_GetWorkingDirectory(char* szBuffer, size_t dwBufferLen);
void Sys_DefaultHomepath(char* szBuffer, size_t dwBufferLen);
void Sys_GetSystemInfo(D2SystemInfoStrc* pInfo);
void Sys_CreateDirectory(char* szPath);
D2ModuleExportStrc* Sys_OpenModule(OpenD2Modules nModule, D2ModuleImportStrc* pImports);
void Sys_CloseModule(OpenD2Modules nModule);
void Sys_CloseModules();

// FileSystem.cpp
void FS_Init(OpenD2ConfigStrc* pConfig);
void FS_Shutdown();
void FS_LogSearchPaths();
size_t FS_Open(char* filename, fs_handle* f, OpenD2FileModes mode, bool bBinary = false);
size_t FS_Read(fs_handle f, void* buffer, size_t dwBufferLen = 4, size_t dwCount = 1);
size_t FS_Write(fs_handle f, void* buffer, size_t dwBufferLen = 1, size_t dwCount = 1);
size_t FS_WritePlaintext(fs_handle f, char* text);
void FS_CloseFile(fs_handle f);
void FS_Seek(fs_handle f, size_t offset, int nSeekType);
size_t FS_Tell(fs_handle f);
bool FS_Find(char* szFileName, char* szBuffer, size_t dwBufferLen);

// FileSystem_MPQ.cpp
void FSMPQ_Init();
void FSMPQ_Shutdown();
D2MPQArchive* FSMPQ_AddSearchPath(char* szMPQName, char* szMPQPath);
fs_handle FSMPQ_FindFile(char* szFileName, char* szMPQName, D2MPQArchive** pArchiveOut);

// MPQ.cpp
void MPQ_OpenMPQ(char* szMPQPath, const char* szMPQName, D2MPQArchive* pMPQ);
void MPQ_CloseMPQ(D2MPQArchive* pMPQ);
fs_handle MPQ_FetchHandle(D2MPQArchive* pMPQ, char* szFileName);
size_t MPQ_FileSize(D2MPQArchive* pMPQ, fs_handle fFile);
size_t MPQ_ReadFile(D2MPQArchive* pMPQ, fs_handle fFile, BYTE* buffer, DWORD dwBufferLen);
void MPQ_Cleanup();

// Window.cpp
void D2Win_InitSDL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
void D2Win_ShutdownSDL();
void D2Win_ShowMessageBox(int nMessageBoxType, char* szTitle, char* szMessage);

// Renderer.cpp
void Render_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);