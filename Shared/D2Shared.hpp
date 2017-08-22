#pragma once

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

//////////////////////////////////////////////////
//
// Hardcoded limitations

#define MAX_TOKEN_CHARS		1024
#define	MAX_D2PATH_ABSOLUTE	1024
#define	MAX_D2PATH_RELATIVE	128

//////////////////////////////////////////////////
//
// Standard Types

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;


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
	char szBasePath[MAX_D2PATH_ABSOLUTE];
	char szHomePath[MAX_D2PATH_ABSOLUTE];
	char szModPath[MAX_D2PATH_ABSOLUTE];
};

//////////////////////////////////////////////////
//
// Library Functions

int D2_stricmpn(char* s1, char* s2, int n);
int D2_stricmp(char* s1, char* s2);
void D2_strncpyz(char *dest, const char *src, int destsize);
char *D2_ParseExt(const char **data_p, bool allowLineBreaks);