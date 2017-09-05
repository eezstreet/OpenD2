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

#ifndef WIN32
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;
#else
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
#endif

#define INVALID_HANDLE (handle)-1

typedef DWORD handle;
typedef handle fs_handle;
typedef handle tex_handle;
typedef handle anim_handle;
typedef handle tbl_handle;
typedef handle font_handle;

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
	IN_TEXT,
	IN_QUIT,
};

// This is a direct mapping of the SDL scancodes, with a few extra things thrown in
enum D2InputButton
{
	B_UNKNOWN = 0,
	B_A = 4,
	B_B = 5,
	B_C = 6,
	B_D = 7,
	B_E = 8,
	B_F = 9,
	B_G = 10,
	B_H = 11,
	B_I = 12,
	B_J = 13,
	B_K = 14,
	B_L = 15,
	B_M = 16,
	B_N = 17,
	B_O = 18,
	B_P = 19,
	B_Q = 20,
	B_R = 21,
	B_S = 22,
	B_T = 23,
	B_U = 24,
	B_V = 25,
	B_W = 26,
	B_X = 27,
	B_Y = 28,
	B_Z = 29,

	B_1 = 30,
	B_2 = 31,
	B_3 = 32,
	B_4 = 33,
	B_5 = 34,
	B_6 = 35,
	B_7 = 36,
	B_8 = 37,
	B_9 = 38,
	B_0 = 39,

	B_RETURN = 40,
	B_ESCAPE = 41,
	B_BACKSPACE = 42,
	B_TAB = 43,
	B_SPACE = 44,

	B_MINUS = 45,
	B_EQUALS = 46,
	B_LEFTBRACKET = 47,
	B_RIGHTBRACKET = 48,
	B_BACKSLASH = 49,
	B_NONUSHASH = 50,
	B_SEMICOLON = 51,
	B_APOSTROPHE = 52,
	B_GRAVE = 53,
	B_COMMA = 54,
	B_PERIOD = 55,
	B_SLASH = 56,

	B_CAPSLOCK = 57,

	B_F1 = 58,
	B_F2 = 59,
	B_F3 = 60,
	B_F4 = 61,
	B_F5 = 62,
	B_F6 = 63,
	B_F7 = 64,
	B_F8 = 65,
	B_F9 = 66,
	B_F10 = 67,
	B_F11 = 68,
	B_F12 = 69,

	B_PRINTSCREEN = 70,
	B_SCROLLLOCK = 71,
	B_PAUSE = 72,
	B_INSERT = 73,
	B_HOME = 74,
	B_PAGEUP = 75,
	B_DELETE = 76,
	B_END = 77,
	B_PAGEDOWN = 78,
	B_RIGHT = 79,
	B_LEFT = 80,
	B_DOWN = 81,
	B_UP = 82,
	B_NUMLOCKCLEAR = 83,

	B_KP_DIVIDE = 84,
	B_KP_MULTIPLY = 85,
	B_KP_MINUS = 86,
	B_KP_PLUS = 87,
	B_KP_ENTER = 88,
	B_KP_1 = 89,
	B_KP_2 = 90,
	B_KP_3 = 91,
	B_KP_4 = 92,
	B_KP_5 = 93,
	B_KP_6 = 94,
	B_KP_7 = 95,
	B_KP_8 = 96,
	B_KP_9 = 97,
	B_KP_0 = 98,
	B_KP_PERIOD = 99,

	B_NONUSBACKSLASH = 100,
	B_APPLICATION = 101,
	B_POWER = 102,

	B_KP_EQUALS = 103,
	B_F13 = 104,
	B_F14 = 105,
	B_F15 = 106,
	B_F16 = 107,
	B_F17 = 108,
	B_F18 = 109,
	B_F19 = 110,
	B_F20 = 111,
	B_F21 = 112,
	B_F22 = 113,
	B_F23 = 114,
	B_F24 = 115,
	B_EXECUTE = 116,
	B_HELP = 117,
	B_MENU = 118,
	B_SELECT = 119,
	B_STOP = 120,
	B_AGAIN = 121,
	B_UNDO = 122,
	B_CUT = 123,
	B_COPY = 124,
	B_PASTE = 125,
	B_FIND = 126,
	B_MUTE = 127,
	B_VOLUMEUP = 128,
	B_VOLUMEDOWN = 129,
	B_KP_COMMA = 133,
	B_KP_EQUALSAS400 = 134,
	B_INTERNATIONAL1 = 135,
	B_INTERNATIONAL2 = 136,
	B_INTERNATIONAL3 = 137, /**< Yen */
	B_INTERNATIONAL4 = 138,
	B_INTERNATIONAL5 = 139,
	B_INTERNATIONAL6 = 140,
	B_INTERNATIONAL7 = 141,
	B_INTERNATIONAL8 = 142,
	B_INTERNATIONAL9 = 143,
	B_LANG1 = 144, /**< Hangul/English toggle */
	B_LANG2 = 145, /**< Hanja conversion */
	B_LANG3 = 146, /**< Katakana */
	B_LANG4 = 147, /**< Hiragana */
	B_LANG5 = 148, /**< Zenkaku/Hankaku */
	B_LANG6 = 149, /**< reserved */
	B_LANG7 = 150, /**< reserved */
	B_LANG8 = 151, /**< reserved */
	B_LANG9 = 152, /**< reserved */

	B_ALTERASE = 153, /**< Erase-Eaze */
	B_SYSREQ = 154,
	B_CANCEL = 155,
	B_CLEAR = 156,
	B_PRIOR = 157,
	B_RETURN2 = 158,
	B_SEPARATOR = 159,
	B_OUT = 160,
	B_OPER = 161,
	B_CLEARAGAIN = 162,
	B_CRSEL = 163,
	B_EXSEL = 164,

	B_KP_00 = 176,
	B_KP_000 = 177,
	B_THOUSANDSSEPARATOR = 178,
	B_DECIMALSEPARATOR = 179,
	B_CURRENCYUNIT = 180,
	B_CURRENCYSUBUNIT = 181,
	B_KP_LEFTPAREN = 182,
	B_KP_RIGHTPAREN = 183,
	B_KP_LEFTBRACE = 184,
	B_KP_RIGHTBRACE = 185,
	B_KP_TAB = 186,
	B_KP_BACKSPACE = 187,
	B_KP_A = 188,
	B_KP_B = 189,
	B_KP_C = 190,
	B_KP_D = 191,
	B_KP_E = 192,
	B_KP_F = 193,
	B_KP_XOR = 194,
	B_KP_POWER = 195,
	B_KP_PERCENT = 196,
	B_KP_LESS = 197,
	B_KP_GREATER = 198,
	B_KP_AMPERSAND = 199,
	B_KP_DBLAMPERSAND = 200,
	B_KP_VERTICALBAR = 201,
	B_KP_DBLVERTICALBAR = 202,
	B_KP_COLON = 203,
	B_KP_HASH = 204,
	B_KP_SPACE = 205,
	B_KP_AT = 206,
	B_KP_EXCLAM = 207,
	B_KP_MEMSTORE = 208,
	B_KP_MEMRECALL = 209,
	B_KP_MEMCLEAR = 210,
	B_KP_MEMADD = 211,
	B_KP_MEMSUBTRACT = 212,
	B_KP_MEMMULTIPLY = 213,
	B_KP_MEMDIVIDE = 214,
	B_KP_PLUSMINUS = 215,
	B_KP_CLEAR = 216,
	B_KP_CLEARENTRY = 217,
	B_KP_BINARY = 218,
	B_KP_OCTAL = 219,
	B_KP_DECIMAL = 220,
	B_KP_HEXADECIMAL = 221,

	B_LCTRL = 224,
	B_LSHIFT = 225,
	B_LALT = 226, /**< alt, option */
	B_LGUI = 227, /**< windows, command (apple), meta */
	B_RCTRL = 228,
	B_RSHIFT = 229,
	B_RALT = 230, /**< alt gr, option */
	B_RGUI = 231, /**< windows, command (apple), meta */

	B_MODE = 257,
		
	B_AUDIONEXT = 258,
	B_AUDIOPREV = 259,
	B_AUDIOSTOP = 260,
	B_AUDIOPLAY = 261,
	B_AUDIOMUTE = 262,
	B_MEDIASELECT = 263,
	B_WWW = 264,
	B_MAIL = 265,
	B_CALCULATOR = 266,
	B_COMPUTER = 267,
	B_AC_SEARCH = 268,
	B_AC_HOME = 269,
	B_AC_BACK = 270,
	B_AC_FORWARD = 271,
	B_AC_STOP = 272,
	B_AC_REFRESH = 273,
	B_AC_BOOKMARKS = 274,

	B_BRIGHTNESSDOWN = 275,
	B_BRIGHTNESSUP = 276,
	B_DISPLAYSWITCH = 277,

	B_KBDILLUMTOGGLE = 278,
	B_KBDILLUMDOWN = 279,
	B_KBDILLUMUP = 280,
	B_EJECT = 281,
	B_SLEEP = 282,

	B_APP1 = 283,
	B_APP2 = 284,

	B_MOUSE1 = 300,	// left mouse
	B_MOUSE2 = 301,	// right mouse
	B_MOUSE3 = 302,	// middle mouse
	B_MOUSE4 = 303,	// extra mouse
	B_MOUSE5 = 304,	// extra mouse
};

enum D2InputModifiers
{
	KEYMOD_SHIFT	= 0x01,
	KEYMOD_CTRL		= 0x02,
	KEYMOD_ALT		= 0x04
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
		D2InputButton		buttonID;
		DWORD				mod;
	};

	union
	{
		MouseMotionEvent	motion;
		ButtonEvent			button;
	} cmdData;
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
	DWORD		(__cdecl *Milliseconds)();

	// Filesystem calls
	size_t		(*FS_Open)(char* szFileName, fs_handle* f, OpenD2FileModes mode, bool bBinary);
	size_t		(*FS_Read)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t		(*FS_Write)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t		(*FS_WritePlaintext)(fs_handle f, char* text);
	void		(*FS_CloseFile)(fs_handle f);
	void		(*FS_Seek)(fs_handle f, size_t dwOffset, int nSeekType);
	size_t		(*FS_Tell)(fs_handle f);

	// Input calls
	void		(*In_PumpEvents)(OpenD2ConfigStrc* pOpenConfig);

	// MPQ calls
	fs_handle	(*MPQ_FindFile)(char* szFileName, char* szMPQName, D2MPQArchive** pArchiveOut);
	size_t		(*MPQ_FileSize)(D2MPQArchive* pMPQ, fs_handle file);
	size_t		(*MPQ_ReadFile)(D2MPQArchive* pMPQ, fs_handle file, BYTE* buffer, DWORD dwBufferLen);

	// TBL calls
	tbl_handle	(*TBL_Register)(char* szTBLFile);
	char16_t*	(*TBL_FindStringFromIndex)(tbl_handle dwIndex);
	tbl_handle	(*TBL_FindStringIndexFromKey)(tbl_handle tbl, char16_t* szReference);
	char16_t*	(*TBL_FindStringFromText)(char16_t* szReference);

	// Renderer calls (should always be last)
	tex_handle	(*R_RegisterDC6Texture)(char *szFileName, char* szHandleName, DWORD dwStart, DWORD dwEnd, int nPalette);
	tex_handle	(*R_RegisterAnimatedDC6)(char *szFileName, char* szHandleName, int nPalette);
	void		(*R_DrawTexture)(tex_handle texture, int x, int y, int w, int h, int u, int v);
	void		(*R_DrawTextureFrames)(tex_handle texture, int x, int y, DWORD dwStart, DWORD dwEnd);
	void		(*R_DrawTextureFrame)(tex_handle texture, int x, int y, DWORD dwFrame);
	void		(*R_SetTextureBlendMode)(tex_handle texture, D2ColorBlending blendMode);
	void		(*R_Present)();
	void		(*R_DeregisterTexture)(char* szTexName, tex_handle texture);
	anim_handle	(*R_RegisterAnimation)(tex_handle texture, char* szHandle, DWORD dwStartingFrame);
	void		(*R_DeregisterAnimation)(anim_handle anim);
	void		(*R_Animate)(anim_handle anim, DWORD dwFramerate, int x, int y);
	void		(*R_SetAnimFrame)(anim_handle anim, DWORD dwFrame);
	font_handle	(*R_RegisterFont)(char* szFontName);
	void		(*R_DeregisterFont)(font_handle font);
	void		(*R_DrawText)(font_handle font, char16_t* text, int x, int y, int w, int h,
		D2TextAlignment alignHorz, D2TextAlignment alignVert);
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

int D2_stricmpn(char* s1, char* s2, int n);
int D2_stricmp(char* s1, char* s2);
void D2_strncpyz(char *dest, const char *src, int destsize);
DWORD D2_strhash(char* szString, size_t dwLen, size_t dwMaxHashSize);
int D2_qstricmpn(char16_t* s1, char16_t* s2, int n);
int D2_qstricmp(char16_t* s1, char16_t* s2);
int D2_qstrcmpn(char16_t* s1, char16_t* s2, int n);
int D2_qstrcmp(char16_t* s1, char16_t* s2);
size_t D2_qstrncpyz(char16_t* dest, char16_t* src, size_t destLen);
size_t D2_qstrlen(char16_t* s1);
size_t D2_qmbtowc(char16_t* dest, size_t destLen, char* src);
size_t D2_qwctomb(char* dest, size_t destLen, char16_t* src);
DWORD D2_qstrhash(char16_t* str, size_t dwLen, DWORD dwMaxHashSize);