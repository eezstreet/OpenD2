#pragma once

#ifdef WIN32
#include "IgnoreWarnings.h"
#endif

//////////////////////////////////////////////////
//
// Project definitions

#define GAME_NAME			"Diablo II"
#define	GAME_VERSION		"1.10f"
#define GAME_MAJOR_VERSION	1
#define	GAME_MINOR_VERSION	10
#define	GAME_TRAILER		"f"

#define EXPANSION			// If not present, we are compiling D2 Classic!

#define GAME_LOG_PATH		"Debug"
#define GAME_LOG_HEADER		"D2"
#define GAME_HOMEPATH		"Diablo II"

//////////////////////////////////////////////////
//
// Hardcoded limitations

#define MAX_TOKEN_CHARS		1024
#define	MAX_D2PATH_ABSOLUTE	1024
#define	MAX_D2PATH			64

//////////////////////////////////////////////////
//
// Platform-specific Definitions

#ifdef WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif

//////////////////////////////////////////////////
//
// Standard Types

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

#define INVALID_HANDLE (handle)-1

typedef DWORD handle;
typedef handle fs_handle;
typedef handle tex_handle;

typedef BYTE pixel[3];
typedef pixel D2Palette[256];

struct D2MPQArchive;

enum OpenD2Paths
{
	FS_HOMEPATH,
	FS_BASEPATH,
	FS_MODPATH,
	FS_MAXPATH
};

enum OpenD2FileModes
{
	FS_READ,
	FS_WRITE,
	FS_READWRITE,
	FS_APPEND
};

enum OpenD2FileSeekModes
{
	FS_SEEK_SET,
	FS_SEEK_CUR,
	FS_SEEK_END,
};

enum OpenD2LogFlags
{
	PRIORITY_CRASH		= 0x01,
	PRIORITY_MESSAGE	= 0x02,
	PRIORITY_DEBUG		= 0x04,
	PRIORITY_SYSTEMINFO	= 0x08,
	PRIORITY_VISUALS	= 0x10,
};

#define PRIORITY_ALL (PRIORITY_CRASH | PRIORITY_MESSAGE | PRIORITY_DEBUG | PRIORITY_SYSTEMINFO | PRIORITY_VISUALS)

enum D2Palettes
{
	PAL_ACT1,
	PAL_ACT2,
	PAL_ACT3,
	PAL_ACT4,
	PAL_ACT5,
	PAL_ENDGAME,
	PAL_FECHAR,
	PAL_LOADING,
	PAL_MENU0,
	PAL_MENU1,
	PAL_MENU2,
	PAL_MENU3,
	PAL_MENU4,
	PAL_SKY,
	PAL_STATIC,
	PAL_TRADEMARK,
	PAL_UNITS,
	PAL_MAX_PALETTES,
};


//////////////////////////////////////////////////
//
// Diablo 2 Structures

/*
 *	The structure which contains data about commandline arguments (ie, the precise values of them)
 *	@author	Necrolis
 */
#pragma pack(push,enter_include)
#pragma pack(1)
struct D2GameConfigStrc     // size 0x3C7
{
	DWORD dwExpansion;		// +00
	BYTE  bWindowed;		// +04 ("w" option)
	BYTE  b3DFX;			// +05 ("3dfx" option)
	BYTE  bOpenGL;			// +06 ("opengl" option)
	BYTE  bRave;			// +07 ("rave" option)
	BYTE  bD3D;				// +08 ("d3d" option)
	BYTE  bPerspective;		// +09 ("per" option)
	BYTE  bQuality;			// +0A ("lq" option)
	DWORD dwGamma;			// +0B ("gamma" option)
	BYTE  bVSync;			// +0F ("vsync" option)
	DWORD dwFramerate;		// +10 ("fr" option)
	DWORD dwGameType;		// +14 ("gametype" option)
	WORD  wJoinID;			// +18 ("joinid" option) ??? dword overlapps next addy !
	char  szGameName[24];	// +1A ("gamename" option),
	char  szServerIP[24];	// {"NETWORK"  ,"SERVERIP"    ,"s"        , 0x02, 0x0032, 0x00},
	char  szBNetIP[24];		// {"NETWORK"  ,"BATTLENETIP" ,"bn"       , 0x02, 0x004A, 0x00},
	char  szMCPIP[24];		// {"NETWORK"  ,"MCPIP"       ,"mcpip"    , 0x02, 0x0062, 0x00},
	BYTE  bUnk07A;			//
	BYTE  bUnk07B;			//
	BYTE  bUnk07C;			//
	BYTE  bUnk07D;			//
	BYTE  bNoPK;			// {"NETWORK"     ,"NOPK"        ,"nopk"      , 0x00, 0x007E, 0x00},
	BYTE  bOpenC;			// {"NETWORK"     ,"OPENC"       ,"openc"     , 0x00, 0x007F, 0x00},
	BYTE  bAmazon;			// {"CHARACTER"   ,"AMAZON"      ,"ama"      , 0x00, 0x80, 0x01},
	BYTE  bPaladin;			// {"CHARACTER"   ,"PALADIN"     ,"pal"      , 0x00, 0x81, 0x00},
	BYTE  bSorceress;		// {"CHARACTER"   ,"SORCERESS"   ,"sor"      , 0x00, 0x82, 0x00},
	BYTE  bNecromancer;		// {"CHARACTER"   ,"NECROMANCER" ,"nec"      , 0x00, 0x83, 0x00},
	BYTE  bBarbarian;		// {"CHARACTER"   ,"BARBARIAN"   ,"bar"      , 0x00, 0x84, 0x00},
	BYTE  bDruid;			// {"CHARACTER"   ,"DRUID"       ,"dru"      , 0x00, 0x85, 0x00},
	BYTE  bAssassin;		// {"CHARACTER"   ,"ASSASSIN"    ,"sas"      , 0x00, 0x86, 0x00},
	BYTE  bInvincible;		// {"CHARACTER"   ,"INVINCIBLE"  ,"i"        , 0x00, 0x87, 0x00},
	BYTE  bUnk088[0x30];	// ??
	char  szCharName[24];	// {"CHARACTER"   ,"NAME"        ,"name"     , 0x02, 0x00B8, 0x00},
	char  szRealm[24];		// {"CHARACTER"   ,"REALM"       ,"realm"    , 0x02, 0x00D0, 0x00},
	BYTE  bUnk0E8[0x100];	// ??
	DWORD dwCTemp;			// {"CHARACTER"   ,"CTEMP"       ,"ctemp"    , 0x01, 0x01E8, 0x00},
	BYTE  bNoMonster;		// {"MONSTER"     ,"NOMONSTER"   ,"nm"       , 0x00, 0x01EC, 0x00},
	DWORD dwMonClass;		// {"MONSTER"     ,"MONSTERCLASS","m"        , 0x01, 0x01ED, 0x00},
	BYTE  bMonInfo;			// {"MONSTER"     ,"MONSTERINFO" ,"minfo"    , 0x00, 0x01F1, 0x00},
	DWORD dwMonDebug;		// {"MONSTER"     ,"MONSTERDEBUG","md"       , 0x01, 0x01F2, 0x00},
	BYTE  bRare;			// {"ITEM"        ,"RARE"        ,"rare"     , 0x00, 0x01F6, 0x00},
	BYTE  bUnique;			// {"ITEM"        ,"UNIQUE"      ,"unique"   , 0x00, 0x01F7, 0x00},
	BYTE  bUnk1F8;			//
	BYTE  bUnk1F9;			//
	DWORD dwAct;			// {"INTERFACE"   ,"ACT"         ,"act"      , 0x01, 0x01FA, 0x01},
	BYTE  bNoPreload;		// {"FILEIO"      ,"NOPREDLOAD"  ,"npl"       , 0x00, 0x01FE, 0x00},
	BYTE  bDirect;			// {"FILEIO"      ,"DIRECT"      ,"direct"    , 0x00, 0x01FF, 0x00},
	BYTE  bLowEnd;			// {"FILEIO"      ,"LOWEND"      ,"lem"       , 0x00, 0x0200, 0x00},
	BYTE  bNoCompress;		// {"FILEIO"      ,"NOCOMPRESS"  ,"nocompress", 0x00, 0x0201, 0x00},
	DWORD dwArena;			// {"NETWORK"     ,"ARENA"       ,"arena"    , 0x01, 0x0202, 0x00},
	WORD  wUnk206;			// 0x10 - related to arena
	BYTE  bUnk208;
	BYTE  bUnk209;
	BYTE  bUnk20A;
	BYTE  nDifficulty;		// +20B
	void* pfMPQFunc;		// +20C function ptr; returns 1
	BYTE  bTXT;				// {"TXT"         ,"TXT"         ,"txt"      , 0x00, 0x0210, 0x00},
	BYTE  bLog;				// {"DEBUG"       ,"LOG"         ,"log"      , 0x00, 0x0211, 0x00},
	BYTE  bMsgLog;			// {"DEBUG"       ,"MSGLOG"      ,"msglog"   , 0x00, 0x0212, 0x00},
	BYTE  bSafeMode;		// {"DEBUG"       ,"SAFEMODE"    ,"safe"     , 0x00, 0x0213, 0x00},
	BYTE  bNoSave;			// {"DEBUG"       ,"NOSAVE"      ,"nosave"   , 0x00, 0x0214, 0x00},
	DWORD dwSeed;			// {"DEBUG"       ,"SEED"        ,"seed"     , 0x01, 0x0215, 0x00},
	BYTE  bCheats;			// {"DEBUG"       ,"CHEATS"      ,"cheats"    , 0x00, 0x0219, 0x00},
	BYTE  bTeen;			// {"DEBUG"       ,"TEEN"        ,"teen"      , 0x00, 0x021A, 0x00},
	BYTE  bNoSound;			// {"DEBUG"       ,"NOSOUND"     ,"ns"        , 0x00, 0x021B, 0x00},
	BYTE  bQuests;			// {"DEBUG"       ,"QuEsTs"      ,"questall"  , 0x00, 0x021C, 0x00},
	BYTE  bSkipInterface;	// skips loading the BNClient QueryInterface
	BYTE  bBuild;			// {"BUILD"       ,"BUILD"       ,"build"     , 0x00, 0x021E, 0x00},
	void* pInterface;		// {"NETWORK"     ,"COMINT"      ,"comint"    , 0x01, 0x021F, 0x00},
	BYTE  bUnk223[0x134];	// +223 ?
	BYTE  bSkipToBNet;		// {"NETWORK"     ,"SKIPTOBNET"  ,"W"         , 0x00, 0x0357, 0x00}, //check at d2launch.6FA1D660 CMP BYTE PTR DS:[EAX+357],BL
	BYTE  nScreenSize;		// {"VIDEO"       ,"SCREENSIZE"  ,"sz"     , 0x00, 0x358, 0x00},
	BYTE  bShowLogo;		// {"VIDEO"       ,"SHOWLOGO"    ,"sl"     , 0x00, 0x359, 0x00},
	BYTE  nUnk[0x6E];		// ?

	BYTE nPadding;         // account for the extra option in 1.13c+
};
#pragma pack(pop,enter_include)

/*
 *	The structure which contains OpenD2-specific data
 *	@author	eezstreet
 */
struct OpenD2ConfigStrc
{
	// See documentation in FileSystem.cpp regarding these three settings
	char	szBasePath[MAX_D2PATH_ABSOLUTE];
	char	szHomePath[MAX_D2PATH_ABSOLUTE];
	char	szModPath[MAX_D2PATH_ABSOLUTE];
	BYTE	bNoSDLAccel;
	BYTE	bBorderless;
	BYTE	bNoRenderText;
	DWORD	dwLogFlags;
};

/*
*	DC6 Files
*/
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
	BYTE* pFramePixels;
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
};

/*
*	DCC Files
*/

//////////////////////////////////////////////////
//
//	Module Exports

#define D2CLIENTAPI_VERSION	1

#ifdef WIN32
#define D2EXPORT	__declspec(dllexport)
#else
#define D2EXPORT	__attribute__((visibility("default")))
#endif

enum OpenD2Modules
{
	MODULE_CLIENT,	// Submodule: D2Client
	MODULE_SERVER,	// Submodule: D2Game
	MODULE_MAX,
	MODULE_NONE,
	MODULE_CLEAN,
};

struct D2ModuleImportStrc
{	// These get imported from the engine
	int nApiVersion;

	// Basic functions
	void		(*Print)(OpenD2LogFlags nPriority, char* szFormat, ...);
	void		(*Warning)(char* szFile, int nLine, char* szCondition);
	void		(*Error)(char* szFile, int nLine, char* szCondition);

	// Filesystem calls
	size_t		(*FS_Open)(char* szFileName, fs_handle* f, OpenD2FileModes mode, bool bBinary);
	size_t		(*FS_Read)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t		(*FS_Write)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t		(*FS_WritePlaintext)(fs_handle f, char* text);
	void		(*FS_CloseFile)(fs_handle f);
	void		(*FS_Seek)(fs_handle f, size_t dwOffset, int nSeekType);
	size_t		(*FS_Tell)(fs_handle f);

	// MPQ calls
	fs_handle	(*MPQ_FindFile)(char* szFileName, char* szMPQName, D2MPQArchive** pArchiveOut);
	size_t		(*MPQ_FileSize)(D2MPQArchive* pMPQ, fs_handle file);
	size_t		(*MPQ_ReadFile)(D2MPQArchive* pMPQ, fs_handle file, BYTE* buffer, DWORD dwBufferLen);
};

struct D2ModuleExportStrc
{	// These get exported to the engine
	int nApiVersion;

	OpenD2Modules	(*RunModuleFrame)(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
};

typedef D2EXPORT D2ModuleExportStrc* (*GetAPIType)(D2ModuleImportStrc* pImports);


//////////////////////////////////////////////////
//
//	Library Functions

int D2_stricmpn(char* s1, char* s2, int n);
int D2_stricmp(char* s1, char* s2);
void D2_strncpyz(char *dest, const char *src, int destsize);
DWORD D2_strhash(char* szString, size_t dwLen, size_t dwMaxHashSize);