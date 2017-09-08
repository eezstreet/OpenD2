#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Libraries/sdl/SDL.h"

/////////////////////////////////////////////////////////
//
//	Structures

enum D2CommandType
{
	CMD_BOOLEAN,
	CMD_DWORD,
	CMD_STRING,
	CMD_BYTE,
	CMD_WORD
};

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
 *	ALL OTHER FILE FORMATS
 */

/*
 *	String Archive TBLs
 *	@author kambala/eezstreet
 */

#define MAX_TBL_FILE_HANDLE	16
#define MAX_TBL_KEY_SIZE	64

#pragma pack(push,enter_include)
#pragma pack(1)

struct TBLHeader // header of string *.tbl file
{
	WORD  CRC;             // +0x00 - CRC value for string table
	WORD  NodesNumber;     // +0x02 - size of Indices array
	DWORD HashTableSize;   // +0x04 - size of TblHashNode array
	BYTE  Version;         // +0x08 - file version, either 0 or 1, doesn't matter
	DWORD DataStartOffset; // +0x09 - string table start offset
	DWORD HashMaxTries;    // +0x0D - max number of collisions for string key search based on its hash value
	DWORD FileSize;        // +0x11 - size of the file
};

struct TBLHashNode // node of the hash table in string *.tbl file
{
	BYTE  Active;          // +0x00 - shows if the entry is used. if 0, then it has been "deleted" from the table
	WORD  Index;           // +0x01 - index in Indices array
	DWORD HashValue;       // +0x03 - hash value of the current string key
	DWORD StringKeyOffset; // +0x07 - offset of the current string key
	DWORD StringValOffset; // +0x0B - offset of the current string value
	WORD  StringValLength; // +0x0F - length of the current string value
};

struct TBLDataNode
{
	BYTE		bActive;
	char16_t	key[MAX_TBL_KEY_SIZE];
	char16_t*	str;
};

struct TBLFile
{
	TBLHeader header;
	TBLHashNode* pHashNodes;
	TBLDataNode* pDataNodes;
	WORD* pIndices;

	char szHandle[MAX_TBL_FILE_HANDLE];
	D2MPQArchive* pArchive;	// the archive that this file was found in
	fs_handle archiveHandle;	// the file inside the archive

	size_t dwFileSize; // the size of the whole TBL file
	char16_t* szStringTable; // a table that contains all of the string data
};

#pragma pack(pop, enter_include)

/*
 *	Font TBLs
 *	@author	Necrolis
 */

#pragma pack(push,enter_include)
#pragma pack(1)

// FIXME: there's a lot of unknowns here, they need to be properly investigated with a debugger
struct TBLFontGlyph
{
	WORD			wChar;
	BYTE			nUnknown1;
	BYTE			nWidth;
	BYTE			nHeight;
	BYTE			nUnknown2;
	WORD			wUnknown3;
	BYTE			nImageIndex;
	BYTE			nChar;
	DWORD			dwUnknown4;	// NOTE: on SDL renderer this gets set to the X offset of the glyph on atlas
};

struct TBLFontFile
{
	BYTE			magic[4];	// always "Woo!", this exclamation appears in the save file format too
	WORD			wVersion;
	DWORD			dwLocale;	// 1 = english, 5 = russian, ...?
	BYTE			nHeight;	// interline spacing
	BYTE			nWidth;		// cap height (?)
	TBLFontGlyph	glyphs[256];
};

#pragma pack(pop, enter_include)

/*
 *	DC6 Files
 *	@author eezstreet (w. help from Paul Siramy)
 */
#define MAX_DC6_CELL_SIZE	256

#pragma pack(push,enter_include)
#pragma pack(1)
struct DC6Frame
{
	struct DC6FrameHeader
	{
		DWORD	dwFlip;				// If true, it's encoded top to bottom instead of bottom to top
		DWORD	dwWidth;			// Width of this frame
		DWORD	dwHeight;			// Height of this frame
		DWORD	dwOffsetX;			// X offset
		DWORD	dwOffsetY;			// Y offset
		DWORD	dwUnknown;
		DWORD	dwNextBlock;
		DWORD	dwLength;			// Number of blocks to decode
	};
	DC6FrameHeader fh;

	DWORD dwDeltaY; // delta from previous frame (0 on first frame in direction)
};

struct DC6ImageHeader
{
	DWORD	dwVersion;			// Version; always '6'
	DWORD	dwUnk1;				// Unknown; always '1'
	DWORD	dwUnk2;				// Unknown; always '0'
	DWORD	dwTermination;		// Termination code - usually 0xEEEEEEEE or 0xCDCDCDCD
	DWORD	dwDirections;		// Number of directions
	DWORD	dwFrames;			// Number of frames per direction
};
#pragma pack(pop, enter_include)

struct DC6Image
{
	void*			mpq;			// The MPQ we found it in
	fs_handle		f;				// The file handle from within the MPQ
	DC6ImageHeader	header;
	DC6Frame*		pFrames;
	BYTE*			pPixels;
	DWORD			dwDirectionHeights[32];
	DWORD			dwTotalWidth;
	DWORD			dwTotalHeight;
};

/*
*	DCC Files
*/

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
	tex_handle	(*RF_TextureFromStitchedDC6)(char* szDc6Path, char* szHandle, DWORD dwStart, DWORD dwEnd, int nPalette);
	tex_handle	(*RF_TextureFromAnimatedDC6)(char* szDc6Path, char* szHandle, int nPalette);
	void		(*RF_DrawTexture)(tex_handle texture, int x, int y, int w, int h, int u, int v);
	void		(*RF_DrawTextureFrames)(tex_handle texture, int x, int y, DWORD dwStartFrame, DWORD dwEndFrame);
	void		(*RF_DrawTextureFrame)(tex_handle texture, int x, int y, DWORD dwFrame);
	void		(*RF_DeregisterTexture)(char* handleName, tex_handle texture);
	void		(*RF_SetTextureBlendMode)(tex_handle texture, D2ColorBlending blendMode);
	void		(*RF_PollTexture)(tex_handle texture, DWORD* dwWidth, DWORD* dwHeight);

	anim_handle	(*RF_RegisterAnimation)(tex_handle texture, char* szHandleName, DWORD dwStartingFrame);
	void		(*RF_DeregisterAnimation)(anim_handle anim);
	void		(*RF_Animate)(anim_handle anim, DWORD dwFramerate, int x, int y);
	void		(*RF_SetAnimFrame)(anim_handle anim, DWORD dwFrame);

	font_handle	(*RF_RegisterFont)(char* szFontName);
	void		(*RF_DeregisterFont)(font_handle font);
	void		(*RF_DrawText)(font_handle font, char16_t* text, int x, int y, int w, int h,
		D2TextAlignment alignHorz, D2TextAlignment alignVert);

	void		(*RF_AlphaModTexture)(tex_handle texture, int nAlpha);
	void		(*RF_ColorModTexture)(tex_handle texture, int nRed, int nGreen, int nBlue);
	void		(*RF_AlphaModFont)(font_handle font, int nAlpha);
	void		(*RF_ColorModFont)(font_handle font, int nRed, int nGreen, int nBlue);
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
DWORD GetMilliseconds();

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

// INI.cpp
void INI_WriteConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig);
void INI_ReadConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig);

// Input.cpp
void In_PumpEvents(OpenD2ConfigStrc* pOpenConfig);

// Logging.cpp
void Log_InitSystem(const char* szLogHeader, const char* szGameName, OpenD2ConfigStrc* pOpenConfig);
void Log_Shutdown();
void Log_Print(OpenD2LogFlags nPriority, char* szFormat, ...);
void Log_Warning(char* szFile, int nLine, char* szCondition);
void Log_Error(char* szFile, int nLine, char* szCondition);
#define Log_WarnAssert(x, y) if(!(x)) { Log_Warning(__FILE__, __LINE__, "" #x); return y; }
#define Log_ErrorAssert(x, y) if(!(x)) { Log_Error(__FILE__, __LINE__, "" #x); return y; }

// MPQ.cpp
void MPQ_OpenMPQ(char* szMPQPath, const char* szMPQName, D2MPQArchive* pMPQ);
void MPQ_CloseMPQ(D2MPQArchive* pMPQ);
fs_handle MPQ_FetchHandle(D2MPQArchive* pMPQ, char* szFileName);
size_t MPQ_FileSize(D2MPQArchive* pMPQ, fs_handle fFile);
size_t MPQ_ReadFile(D2MPQArchive* pMPQ, fs_handle fFile, BYTE* buffer, DWORD dwBufferLen);
void MPQ_Cleanup();

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

// Renderer.cpp
void Render_Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
void Render_MapRenderTargetExports(D2ModuleImportStrc* pExport);

// TBL_Font.cpp
tbl_handle TBLFont_RegisterFont(char* szFontName);
TBLFontFile* TBLFont_GetPointerFromHandle(tbl_handle handle);

// TBL_Text.cpp
tbl_handle TBL_Register(char* szTblFile);
char16_t* TBL_FindStringFromIndex(DWORD dwIndex);
tbl_handle TBL_FindStringIndexFromKey(tbl_handle tbl, char16_t* szReference);
char16_t* TBL_FindStringText(char16_t* szReference);
void TBL_Init();
void TBL_Cleanup();

// Window.cpp
void D2Win_InitSDL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
void D2Win_ShutdownSDL();
void D2Win_ShowMessageBox(int nMessageBoxType, char* szTitle, char* szMessage);