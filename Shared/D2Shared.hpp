#pragma once

#include <memory>
#include <inttypes.h>

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
#define	GAME_CONFIG_PATH	"D2.ini"
#define GAME_SAVE_PATH		"Save"

/*
 *	The following languages are known to exist:
 *	CHI	DEU	ENG	ESP
 *	FRA	ITA	JPN	KOR
 *	POL	POR	RUS
 *
 *	The following charsets are known to exist:
 *	LATIN (base game)
 *	CHINESE (requires d2delta.mpq)
 *	JAPAN (requires d2delta.mpq)
 *	RUSSIAN	(requires d2delta.mpq)
 *	KOREAN (requires d2delta.mpq and d2kfixup.mpq)
 */
#define GAME_LANGUAGE		"ENG"
#define GAME_CHARSET		"LATIN"

//////////////////////////////////////////////////
//
// Hardcoded limitations

#define MAX_TOKEN_CHARS		1024
#define	MAX_D2PATH_ABSOLUTE	1024
#define	MAX_D2PATH			64
#define MAX_FILE_LIST_SIZE	128

//////////////////////////////////////////////////
//
// Platform-specific Definitions

#ifdef WIN32
#define NL "\r\n"
#define D2EXPORT	__declspec(dllexport)
#define D2IMPORT	__declspec(dllimport)
#else
#define NL "\n"
#define D2EXPORT	__attribute__((visibility("default")))
#define D2IMPORT
#endif

//////////////////////////////////////////////////
//
// Standard Types

#ifndef WIN32
typedef uint64_t QWORD;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;
#else
typedef unsigned long long	QWORD;
typedef unsigned long		DWORD;
typedef unsigned short		WORD;
typedef unsigned char		BYTE;
#endif

#define LOWORD(_dw)     ((WORD)(((DWORD*)(_dw)) & 0xffff))
#define HIWORD(_dw)     ((WORD)((((DWORD*)(_dw)) >> 16) & 0xffff))
#define LODWORD(_qw)    ((DWORD)(_qw))
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))

#define INVALID_HANDLE (handle)-1
#define SEED_MAGIC		666	// Little known fact...Diablo II encodes the top 4 bytes of a seed with 666. Clearly an evil game.

typedef DWORD handle;
typedef handle fs_handle;
typedef handle tex_handle;
typedef handle anim_handle;
typedef handle tbl_handle;
typedef handle font_handle;
typedef handle cof_handle;

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

enum D2ColorBlending
{
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_MOD,
};

enum D2TextAlignment
{
	ALIGN_LEFT = 0,		// text draws from left at x all the way to w
	ALIGN_TOP = 0,
	ALIGN_CENTER = 1,	// text draws centered between x and w
	ALIGN_RIGHT = 2,	// text draws from right at w going to x
	ALIGN_BOTTOM = 2,
};

enum D2InputCommand
{
	IN_MOUSEMOVE,
	IN_MOUSEDOWN,
	IN_MOUSEUP,
	IN_MOUSEWHEEL,
	IN_KEYDOWN,
	IN_KEYUP,
	IN_TEXTINPUT,
	IN_TEXTEDITING,
	IN_QUIT,
};

// This is a direct mapping of the SDL scancodes, with a few extra things thrown in
enum D2InputButton
{
	B_MOUSE1 = 0,	// left mouse
	B_MOUSE2 = 1,	// right mouse
	B_MOUSE3 = 2,	// middle mouse
	B_MOUSE4 = 3,	// extra mouse
	B_MOUSE5 = 7,	// extra mouse

	B_BACKSPACE = '\b',
	B_RIGHTARROW = 1073741903,
	B_LEFTARROW = 1073741904,
	B_DOWNARROW = 1073741905,
	B_UPARROW = 1073741906,
	B_HOME = 1073741898,
	B_END = 1073741901,
	B_INSERT = 1073741897,
	B_DELETE = '\177',

};

enum D2InputModifiers
{
	KEYMOD_SHIFT	= 0x01,
	KEYMOD_CTRL		= 0x02,
	KEYMOD_ALT		= 0x04
};

enum D2CharacterStatus
{
	D2STATUS_NEWBIE = 0,
	D2STATUS_HARDCORE = 2,
	D2STATUS_DEAD = 3,
	D2STATUS_EXPANSION = 5,
};

enum D2CharacterClass
{
	D2CLASS_AMAZON,
	D2CLASS_SORCERESS,
	D2CLASS_NECROMANCER,
	D2CLASS_PALADIN,
	D2CLASS_BARBARIAN,
	D2CLASS_DRUID,
	D2CLASS_ASSASSIN,
	D2CLASS_MAX,
};

enum D2Difficulties
{
	D2DIFF_NORMAL,
	D2DIFF_NIGHTMARE,
	D2DIFF_HELL,
	D2DIFF_MAX,
};

enum D2Acts
{
	D2_ACT_I,
	D2_ACT_II,
	D2_ACT_III,
	D2_ACT_IV,
	D2_ACT_V,
	MAX_ACTS,
};

typedef void	(*AnimKeyframeCallback)(anim_handle anim, int nExtraInt);

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
 *	The command queue is iterated through on the client, and filled in by the engine when requested.
 */
struct D2CommandQueue
{
	D2InputCommand cmdType;

	struct MouseMotionEvent
	{
		DWORD x;
		DWORD y;
	};

	struct ButtonEvent
	{
		DWORD				buttonID;
		DWORD				mod;
	};

	struct TextEvent
	{
		char				text[32];
		int					start;
		int					length;
	};

	union
	{
		MouseMotionEvent	motion;
		ButtonEvent			button;
		TextEvent			text;
	} cmdData;
};

/*
 *	Seeds are used almost everywhere for random number generation
 */
struct D2Seed
{
	DWORD dwLoSeed;
	DWORD dwHiSeed;
};

/*
 *	The structure which contains OpenD2-specific data
 *	@author	eezstreet
 */
struct OpenD2ConfigStrc
{
	// See documentation in FileSystem.cpp regarding these three settings
	char			szBasePath[MAX_D2PATH_ABSOLUTE];
	char			szHomePath[MAX_D2PATH_ABSOLUTE];
	char			szModPath[MAX_D2PATH_ABSOLUTE];

	BYTE			bNoSDLAccel;
	BYTE			bBorderless;
	BYTE			bNoRenderText;
	DWORD			dwLogFlags;
	D2CommandQueue*	pCmds;
	DWORD			dwNumPendingCommands;
};

//////////////////////////////////////////////////
//
//	Module Exports

#define D2CLIENTAPI_VERSION	1

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
	DWORD		(__cdecl *Milliseconds)();

	// Filesystem calls
	size_t		(*FS_Open)(char* szFileName, fs_handle* f, OpenD2FileModes mode, bool bBinary);
	size_t		(*FS_Read)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t		(*FS_Write)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t		(*FS_WritePlaintext)(fs_handle f, char* text);
	void		(*FS_CloseFile)(fs_handle f);
	void		(*FS_Seek)(fs_handle f, size_t dwOffset, int nSeekType);
	size_t		(*FS_Tell)(fs_handle f);
	char**		(*FS_ListFilesInDirectory)(char* szDirectory, char* szExtensionFilter, int *nFiles);
	void		(*FS_FreeFileList)(char** pszFileList, int nNumFiles);
	void		(*FS_CreateSubdirectory)(char* szSubdirectory);

	// Input calls
	void		(*In_PumpEvents)(OpenD2ConfigStrc* pOpenConfig);
	void		(*In_StartTextEditing)();
	void		(*In_StopTextEditing)();

	// MPQ calls
	fs_handle	(*MPQ_FindFile)(char* szFileName, char* szMPQName, D2MPQArchive** pArchiveOut);
	size_t		(*MPQ_FileSize)(D2MPQArchive* pMPQ, fs_handle file);
	size_t		(*MPQ_ReadFile)(D2MPQArchive* pMPQ, fs_handle file, BYTE* buffer, DWORD dwBufferLen);

	// TBL calls
	tbl_handle	(*TBL_Register)(char* szTBLFile);
	char16_t*	(*TBL_FindStringFromIndex)(tbl_handle dwIndex);
	tbl_handle	(*TBL_FindStringIndexFromKey)(tbl_handle tbl, char16_t* szReference);
	char16_t*	(*TBL_FindStringFromText)(char16_t* szReference);

	// COF calls
	cof_handle	(*COF_Register)(char* type, char* token, char* animation, char* hitclass);
	void		(*COF_Deregister)(cof_handle cof);
	void		(*COF_DeregisterType)(char* type);

	// Renderer calls (should always be last)
	tex_handle	(*R_RegisterDC6Texture)(char *szFileName, char* szHandleName, DWORD dwStart, DWORD dwEnd, int nPalette);
	tex_handle	(*R_RegisterAnimatedDC6)(char *szFileName, char* szHandleName, int nPalette);
	void		(*R_DrawTexture)(tex_handle texture, int x, int y, int w, int h, int u, int v);
	void		(*R_DrawTextureFrames)(tex_handle texture, int x, int y, DWORD dwStart, DWORD dwEnd);
	void		(*R_DrawTextureFrame)(tex_handle texture, int x, int y, DWORD dwFrame);
	void		(*R_SetTextureBlendMode)(tex_handle texture, D2ColorBlending blendMode);
	void		(*R_Present)();
	void		(*R_DeregisterTexture)(char* szTexName, tex_handle texture);
	void		(*R_PollTexture)(tex_handle texture, DWORD* dwWidth, DWORD* dwHeight);
	bool		(*R_PixelPerfectDetect)(anim_handle anim, int nSrcX, int nSrcY, int nDrawX, int nDrawY, bool bAllowAlpha);
	anim_handle	(*R_RegisterAnimation)(tex_handle texture, char* szHandle, DWORD dwStartingFrame);
	void		(*R_DeregisterAnimation)(anim_handle anim);
	void		(*R_Animate)(anim_handle anim, DWORD dwFramerate, int x, int y);
	void		(*R_SetAnimFrame)(anim_handle anim, DWORD dwFrame);
	DWORD		(*R_GetAnimFrame)(anim_handle anim);
	void		(*R_AddAnimKeyframe)(anim_handle anim, int nFrame, AnimKeyframeCallback callback, int nExtraInt);
	void		(*R_RemoveAnimKeyframe)(anim_handle anim);
	DWORD		(*R_GetAnimFrameCount)(anim_handle anim);
	font_handle	(*R_RegisterFont)(char* szFontName);
	void		(*R_DeregisterFont)(font_handle font);
	void		(*R_DrawText)(font_handle font, char16_t* text, int x, int y, int w, int h,
		D2TextAlignment alignHorz, D2TextAlignment alignVert);
	void		(*R_AlphaModTexture)(tex_handle texture, int nAlpha);
	void		(*R_ColorModTexture)(tex_handle texture, int nRed, int nGreen, int nBlue);
	void		(*R_AlphaModFont)(font_handle font, int nAlpha);
	void		(*R_ColorModFont)(font_handle font, int nRed, int nGreen, int nBlue);
	void		(*R_DrawRectangle)(int x, int y, int w, int h, int r, int g, int b, int a);
};

struct D2ModuleExportStrc
{	// These get exported to the engine
	int nApiVersion;

	OpenD2Modules	(*RunModuleFrame)(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
	void			(*CleanupModule)();
};

typedef D2EXPORT D2ModuleExportStrc* (*GetAPIType)(D2ModuleImportStrc* pImports);

//////////////////////////////////////////////////
//
//	Library Functions

// String Management - ASCII
int D2_stricmpn(char* s1, char* s2, int n);
int D2_stricmp(char* s1, char* s2);
void D2_strncpyz(char *dest, const char *src, int destsize);
DWORD D2_strhash(char* szString, size_t dwLen, size_t dwMaxHashSize);

// String Management - UTF-16
int D2_qstricmpn(char16_t* s1, char16_t* s2, int n);
int D2_qstricmp(char16_t* s1, char16_t* s2);
int D2_qstrcmpn(char16_t* s1, char16_t* s2, int n);
int D2_qstrcmp(char16_t* s1, char16_t* s2);
size_t D2_qstrncpyz(char16_t* dest, char16_t* src, size_t destLen);
size_t D2_qstrlen(char16_t* s1);
size_t D2_qmbtowc(char16_t* dest, size_t destLen, char* src);
size_t D2_qwctomb(char* dest, size_t destLen, char16_t* src);
char16_t* D2_qstrverse(char16_t* s);
char16_t* D2_qnitoa(int number, char16_t* buffer, size_t bufferLen, int base, size_t& written);
char16_t* D2_qstrchr(char16_t* str, char16_t chr);
int D2_qvsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, va_list args);
int D2_qsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, ...);
DWORD D2_qstrhash(char16_t* str, size_t dwLen, DWORD dwMaxHashSize);

// Random Numbers
DWORD D2_srand(D2Seed* pSeed);
DWORD D2_smrand(D2Seed* pSeed, DWORD dwMax);
DWORD D2_srrand(D2Seed* pSeed, DWORD dwMin, DWORD dwMax);
void D2_seedcopy(D2Seed* pDest, D2Seed* pSrc);
bool D2_sbrand(D2Seed* pSeed);