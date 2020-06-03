#pragma once

#include <memory>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>

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
#define GAME_FULL_UTF16		u"Diablo II 1.10f"

#define EXPANSION			// If not present, we are compiling D2 Classic!

#define GAME_LOG_PATH		"Debug"
#define GAME_LOG_HEADER		"D2"
#define GAME_HOMEPATH		"Diablo II"
#define	GAME_CONFIG_PATH	"D2.ini"
#define GAME_SAVE_PATH		"Save"
#define GAME_PORT			4000

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
#define MAX_PLAYERS_VIRTUAL	128		// how many players we can simulate in the treasure system
#define MAX_PLAYERS_REAL	8		// how many players can actually exist in the game at once

//////////////////////////////////////////////////
//
// Platform-specific Definitions

#ifdef WIN32
#define NL "\r\n"
#define D2EXPORT	__declspec(dllexport)
#define D2IMPORT	__declspec(dllimport)
#else
#define NL "\n"
#define D2EXPORT
#define D2IMPORT
#define __cdecl
#endif

//////////////////////////////////////////////////
//
// Standard Types

#ifndef _WIN32
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

#define LOWORD(_dw)     ((WORD)(((DWORD*)(_dw)) & 0xffffu))
#define HIWORD(_dw)     ((WORD)((((DWORD*)(_dw)) >> 16u) & 0xffffu))
#define LODWORD(_qw)    ((DWORD)(_qw))
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32u) & 0xffffffffu))

#define INVALID_HANDLE (handle)-1
#define SEED_MAGIC		666	// Little known fact...Diablo II encodes the top 4 bytes of a seed with 666. Clearly an evil game.

#define MAX_DIRECTIONS	32	// 32 directions per DCC
#define MAX_FRAMES		256	// 256 frames per direction

#define INVALID_COMPONENT	"xxx"

typedef DWORD handle;
typedef handle fs_handle;
typedef handle tex_handle;
typedef handle anim_handle;
typedef handle tbl_handle;
typedef handle font_handle;
typedef handle cof_handle;
typedef handle token_handle;
typedef handle thread_handle;
typedef handle sfx_handle;
typedef handle mus_handle;

typedef BYTE pixel[3];
typedef pixel D2Palette[256];

typedef void	(*D2AsyncTask)(void* pData);

struct D2MPQArchive;
struct D2Packet;

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
	IN_WINDOW
};

enum D2WindowEventType
{
	WINDOWEVENT_NONE,           /**< Never used */
	WINDOWEVENT_SHOWN,          /**< Window has been shown */
	WINDOWEVENT_HIDDEN,         /**< Window has been hidden */
	WINDOWEVENT_EXPOSED,        /**< Window has been exposed and should be redrawn */
	WINDOWEVENT_MOVED,          /**< Window has been moved to data1, data2 */
	WINDOWEVENT_RESIZED,        /**< Window has been resized to data1xdata2 */
	WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as a result of an API call or through the system or user changing the window size. */
	WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
	WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
	WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size and position */
	WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
	WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
	WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
	WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
	WINDOWEVENT_CLOSE,          /**< The window manager requests that the window be closed */
	WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
	WINDOWEVENT_HIT_TEST
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
	D2STATUS_NEWBIE = 0u,
	D2STATUS_HARDCORE = 2u,
	D2STATUS_DEAD = 3u,
	D2STATUS_EXPANSION = 5u,
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

enum D2CompositeItem
{
	COMP_HEAD,
	COMP_TORSO,
	COMP_LEGS,
	COMP_RIGHTARM,
	COMP_LEFTARM,
	COMP_RIGHTHAND,
	COMP_LEFTHAND,
	COMP_SHIELD,
	COMP_SPECIAL1,
	COMP_SPECIAL2,
	COMP_SPECIAL3,
	COMP_SPECIAL4,
	COMP_SPECIAL5,
	COMP_SPECIAL6,
	COMP_SPECIAL7,
	COMP_SPECIAL8,
	COMP_MAX,
	COMP_INVALID = 0xFF,
};

// Can be loaded from WeaponClass.txt later on
enum D2WeaponClass
{
	WC_NONE,	// nothing
	WC_HTH,		// "HTH" - fists
	WC_BOW,		// "BOW" - bows
	WC_1HS,		// "1HS" - 'one handed swing' - maces, swords, hammers, axes
	WC_1HT,		// "1HT" - 'one handed thrust' - daggers, javelins, but also throwing potions
	WC_STF,		// "STF" - staves, but also polearms, mauls, and two-handed axes
	WC_2HS,		// "2HS" - 'two handed swing' - bastard swords
	WC_2HT,		// "2HT" - 'two handed thrust' - spears
	WC_XBW,		// "XBW" - crossbows
	WC_1JS,		// "1JS" - 'left jab right swing' - only used for Barbarians
	WC_1JT,		// "1JT" - 'left jab right thrust' - only used for Barbarians
	WC_1SS,		// "1SS" - 'left swing right swing' - only used for Barbarians
	WC_1ST,		// "1ST" - 'left swing right thrust' - only used for Barbarians
	WC_HT1,		// "HT1" - 'one hand-to-hand' - used for Assassins when claw is in one hand
	WC_HT2,		// "HT2" - 'two hand-to-hand' - used for Assassins when claw is in two hands
	WC_MAX,
};

// Different token types
enum D2TokenType
{
	TOKEN_CHAR,
	TOKEN_MONSTER,
	TOKEN_OBJECT,
	TOKEN_MAX,
};

// Different animation types (DCC)
enum D2AnimatedType
{
	ATYPE_CHAR,
	ATYPE_MONSTER,
	ATYPE_OBJECT,
	ATYPE_MISSILE,
	ATYPE_OVERLAY,
	ATYPE_MAX,
};

// Can be loaded from PlrMode.txt later on
enum D2PlayerMode
{
	PLRMODE_DT,	// death
	PLRMODE_NU,	// "neutral", when the player is standing still outside of town
	PLRMODE_WL,	// walk
	PLRMODE_RN,	// run
	PLRMODE_GH,	// gethit
	PLRMODE_TN,	// "town neutral", when the player is standing still inside of town
	PLRMODE_TW,	// town walk
	PLRMODE_A1,	// attack 1
	PLRMODE_A2,	// attack 2
	PLRMODE_BL,	// block
	PLRMODE_SC,	// cast
	PLRMODE_TH,	// throw
	PLRMODE_KK,	// kick
	PLRMODE_S1,	// skill 1
	PLRMODE_S2,	//
	PLRMODE_S3,
	PLRMODE_S4,
	PLRMODE_DD,	// dead
	PLRMODE_SQ,	// special sequences. These are handled in monseq.txt
	PLRMODE_KB,	// knockback. not sure if used?
	PLRMODE_MAX
};

// Can be loaded from MonMode.txt later on
enum D2MonsterMode
{
	MONMODE_DT,	// death
	MONMODE_NU,	// neutral
	MONMODE_WL,	// walk
	MONMODE_GH,	// gethit
	MONMODE_A1,	// attack 1
	MONMODE_A2,	// attack 2
	MONMODE_BL,	// block
	MONMODE_SC,	// cast
	MONMODE_S1,	// skill 1
	MONMODE_S2,
	MONMODE_S3,
	MONMODE_S4,
	MONMODE_DD,	// dead
	MONMODE_KB,	// knockback. not sure if used?
	MONMODE_XX,	// special sequences. These are handled in monseq.txt
	MONMODE_RN,	// run
	MONMODE_MAX
};

// Can be loaded from ObjMode.txt later on
enum D2ObjectMode
{
	OBJMODE_NU,	// neutral
	OBJMODE_OP,	// operating
	OBJMODE_ON,	// opened
	OBJMODE_S1,	// special
	OBJMODE_S2,
	OBJMODE_S3,
	OBJMODE_S4,
	OBJMODE_S5,
	OBJMODE_MAX
};

// max(MONMODE_MAX, PLRMODE_MAX, OBJMODE_MAX)
#define XXXMODE_MAX	PLRMODE_MAX

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

struct WindowEvent
{
	D2WindowEventType event;
};

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
		WindowEvent			window;
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
	DWORD			dwAudioDevice;
	DWORD			dwAudioChannels;
};

class IRenderer
{
public:
	//
    virtual ~IRenderer() {};
	virtual void Present() = 0;

	//
	virtual tex_handle TextureFromStitchedDC6(const char* dc6Path, const char* handle, DWORD start, DWORD end, int palette) = 0;
	virtual tex_handle TextureFromAnimatedDC6(const char* dc6Path, const char* handle, int palette) = 0;
	virtual void DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v) = 0;
	virtual void DrawTextureFrames(tex_handle texture, int x, int y, DWORD startFrame, DWORD endFrame) = 0;
	virtual void DrawTextureFrame(tex_handle texture, int x, int y, DWORD frame) = 0;
	virtual void DeregisterTexture(const char* handleName, tex_handle texture) = 0;
	virtual void SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode) = 0;
	virtual void PollTexture(tex_handle texture, DWORD* width, DWORD* height) = 0;
	virtual bool PixelPerfectDetect(anim_handle anim, int srcX, int srcY, int drawX, int drawY, bool bAllowAlpha) = 0;

	//
	virtual anim_handle RegisterDC6Animation(tex_handle texture, const char* szHandlename, DWORD startingFrame) = 0;
	virtual void DeregisterAnimation(anim_handle anim) = 0;
	virtual void Animate(anim_handle anim, DWORD framerate, int x, int y) = 0;
	virtual void SetAnimFrame(anim_handle anim, DWORD frame) = 0;
	virtual DWORD GetAnimFrame(anim_handle anim) = 0;
	virtual void AddAnimKeyframe(anim_handle anim, int frame, AnimKeyframeCallback callback, int extraInt) = 0;
	virtual void RemoveAnimKeyframe(anim_handle anim) = 0;
	virtual DWORD GetAnimFrameCount(anim_handle anim) = 0;

	//
	virtual font_handle RegisterFont(const char* fontName) = 0;
	virtual void DeregisterFont(font_handle font) = 0;
	virtual void DrawText(font_handle font, const char16_t* text, int x, int y, int w, int h,
	D2TextAlignment alignHorz, D2TextAlignment alignVert) = 0;

	//
	virtual void AlphaModTexture(tex_handle texture, int alpha) = 0;
	virtual void ColorModTexture(tex_handle texture, int red, int green, int blue) = 0;
	virtual void AlphaModFont(font_handle font, int alpha) = 0;
	virtual void ColorModFont(font_handle font, int red, int green, int blue) = 0;

	//
	virtual void DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a) = 0;

	virtual void DrawTokenInstance(anim_handle instance, int x, int y, int translvl, int palette) = 0;

	virtual void Clear() = 0;
};

//////////////////////////////////////////////////
//
//	Module Exports

#define D2CLIENTAPI_VERSION	1
#define D2SERVERAPI_VERSION	1

enum OpenD2Modules
{
	MODULE_CLIENT,		// Submodule: D2Client
	MODULE_SERVER,		// Submodule: D2Game
	MODULE_MAX,
	MODULE_NONE,
	MODULE_CLEAN,
};

struct D2ModuleImportStrc
{	// These get imported from the engine
	int nApiVersion;
	IRenderer* renderer;

	// Basic functions
	void			(*Print)(OpenD2LogFlags nPriority, const char* szFormat, ...);
	void			(*Warning)(const char* szFile, const int nLine, const char* szCondition);
	void			(*Error)(const char* szFile, const int nLine, const char* szCondition);
	DWORD			(__cdecl *Milliseconds)();

	// Filesystem calls
	size_t			(*FS_Open)(const char* szFileName, fs_handle* f, OpenD2FileModes mode, bool bBinary);
	size_t			(*FS_Read)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t			(*FS_Write)(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount);
	size_t			(*FS_WritePlaintext)(fs_handle f, const char* text);
	void			(*FS_CloseFile)(fs_handle f);
	void			(*FS_Seek)(fs_handle f, size_t dwOffset, int nSeekType);
	size_t			(*FS_Tell)(fs_handle f);
	char**			(*FS_ListFilesInDirectory)(const char* szDirectory, const char* szExtensionFilter, int *nFiles);
	void			(*FS_FreeFileList)(char** pszFileList, int nNumFiles);
	void			(*FS_CreateSubdirectory)(const char* szSubdirectory);

	// Network subsystem calls
	void			(*NET_SendServerPacket)(int nClientMask, D2Packet* pPacket);
	void			(*NET_SendClientPacket)(D2Packet* pPacket);
	void			(*NET_SetPlayerCount)(DWORD dwNewPlayerCount);
	bool			(*NET_Connect)(char* szServerAddress, DWORD dwPort);
	void			(*NET_Disconnect)();
	void			(*NET_Listen)(DWORD dwPort);
	void			(*NET_StopListen)();
	char16_t*		(*NET_GetLocalIP)();

	// Input calls
	void			(*In_PumpEvents)(OpenD2ConfigStrc* pOpenConfig);
	void			(*In_StartTextEditing)();
	void			(*In_StopTextEditing)();

	// TBL calls
	tbl_handle		(*TBL_Register)(const char* szTBLFile);
	char16_t*		(*TBL_FindStringFromIndex)(tbl_handle dwIndex);
	tbl_handle		(*TBL_FindStringIndexFromKey)(tbl_handle tbl, char16_t* szReference);
	char16_t*		(*TBL_FindStringFromText)(char16_t* szReference);

	// Token calls (TODO: Make part of the modcode, not part of the engine)
	token_handle	(*TOK_Register)(D2TokenType type, const char* tokenName, const char* szWeaponClass);
	void			(*TOK_Deregister)(token_handle token);

	// Token instance calls (TODO: Make part of the modcode, not part of the engine)
	anim_handle		(*TOK_CreateTokenAnimInstance)(token_handle token);
	void			(*TOK_SwapTokenAnimToken)(anim_handle handle, token_handle newhandle);
	void			(*TOK_DestroyTokenInstance)(anim_handle handle);
	void			(*TOK_SetTokenInstanceComponent)(anim_handle handle, int componentNum, const char* componentName);
	char*			(*TOK_GetTokenInstanceComponent)(anim_handle handle, int component);
	void			(*TOK_SetTokenInstanceFrame)(anim_handle handle, int frameNum);
	int				(*TOK_GetTokenInstanceFrame)(anim_handle handle);
	char*			(*TOK_GetTokenInstanceWeaponClass)(anim_handle handle);
	void			(*TOK_SetInstanceActive)(anim_handle handle, bool bNewActive);
	void			(*TOK_SetTokenInstanceMode)(anim_handle handle, int modeNum);
	void			(*TOK_SetTokenInstanceDirection)(anim_handle handle, int dirNum);

	// Audio calls
	sfx_handle		(*S_RegisterSound)(const char* szAudioFile);
	mus_handle		(*S_RegisterMusic)(const char* szAudioFile);
	void			(*S_PlaySound)(sfx_handle handle, int loops);
	void			(*S_PlayMusic)(mus_handle handle, int loops);
	void			(*S_PauseAudio)();
	void			(*S_ResumeAudio)();
	void			(*S_SetMasterVolume)(float volume);
	void			(*S_SetMusicVolume)(float volume);
	void			(*S_SetSoundVolume)(float volume);
};

struct D2ModuleExportStrc
{	// These get exported to the engine
	int nApiVersion;

	OpenD2Modules	(*RunModuleFrame)(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
	void			(*CleanupModule)();
	bool			(*HandlePacket)(D2Packet* pPacket);
};

typedef D2EXPORT D2ModuleExportStrc* (*GetAPIType)(D2ModuleImportStrc* pImports);

//////////////////////////////////////////////////
//
//	Library Functions

namespace D2Lib
{
	// String Management - ASCII
	int stricmpn(const char* s1, const char* s2, size_t n);
	int stricmp(const char* s1, const char* s2);
	void strncpyz(char *dest, const char *src, size_t destsize);
	DWORD strhash(const char* szString, size_t dwLen, size_t dwMaxHashSize);

	// File name management
	char* fnbld(char* szFileName);
	char* fnbldb(char* szFileName);
	char* fnext(char* szFileName);
	char* fnextstr(char* szFileName);
	char* fnextstrb(char* szFileName);

	// String Management - UTF-16
	int qstricmpn(const char16_t* s1, const char16_t* s2, int n);
	int qstricmp(const char16_t* s1, const char16_t* s2);
	int qstrcmpn(const char16_t* s1, const char16_t* s2, int n);
	int qstrcmp(const char16_t* s1, const char16_t* s2);
	size_t qstrncpyz(char16_t* dest, const char16_t* src, size_t destLen);
	size_t qstrlen(const char16_t* s1);
	size_t qmbtowc(char16_t* dest, size_t destLen, const char* src);
	size_t qwctomb(char* dest, size_t destLen, const char16_t* src);
	void qstrverse(char16_t* s, int start, size_t len);
	char16_t* qnitoa(int number, char16_t* buffer, size_t bufferLen, int base, size_t& written);
	char16_t* qstrchr(char16_t* str, char16_t chr);
	size_t qvsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, va_list args);
	size_t qsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, ...);
	DWORD qstrhash(const char16_t *str, size_t dwLen, DWORD dwMaxHashSize);

	// Random Numbers
	DWORD rand();
	DWORD mrand(DWORD dwMax);
	DWORD rrand(DWORD dwMin, DWORD dwMax);
	DWORD srand(D2Seed* pSeed);
	DWORD smrand(D2Seed* pSeed, DWORD dwMax);
	DWORD srrand(D2Seed* pSeed, DWORD dwMin, DWORD dwMax);
	void seedcopy(D2Seed* pDest, D2Seed* pSrc);
	bool sbrand(D2Seed* pSeed);

	// Math
	template <typename T>
	T min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template <typename T>
	T max(T a, T b)
	{
		return (a > b) ? a : b;
	}
};
