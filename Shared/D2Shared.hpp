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
struct D2GameConfigStrc     // size 0x3C7
{
	DWORD dwExpansion;
	BYTE  bWindowed;		// {"VIDEO","WINDOW"       ,"w"     , 0x00, 0x04, 0x00},
	BYTE  b3DFX;			// {"VIDEO","3DFX"         ,"3dfx"  , 0x00, 0x05, 0x00},
	BYTE  bOpenGL;			// {"VIDEO","OPENGL"       ,"opengl", 0x00, 0x06, 0x00},
	BYTE  bRave;			// {"VIDEO","RAVE"         ,"rave"  , 0x00, 0x07, 0x00},
	BYTE  bD3D;				// {"VIDEO","D3D"          ,"d3d"   , 0x00, 0x08, 0x00},
	BYTE  bPerspective;		// {"VIDEO","PERSPECTIVE"  ,"per"   , 0x00, 0x09, 0x00},
	BYTE  bQuality;			// {"VIDEO","QUALITY"      ,"lq"    , 0x00, 0x0A, 0x00},
	DWORD dwGamma;			// {"VIDEO","GAMMA"        ,"gamma" , 0x00, 0x0B, 0x00},
	BYTE  bVSync;			// {"VIDEO","VSYNC"        ,"vsync" , 0x00, 0x0F, 0x00},
	DWORD dwFramerate;		// {"VIDEO","FRAMERATE"    ,"fr"    , 0x01, 0x10, 0x00},
	DWORD dwGameType;		// {"NETWORK"  ,"GAMETYPE"    ,"gametype" , 0x01, 0x0014, 0x00},
	WORD  wJoinID;			// {"NETWORK"  ,"JOINID"      ,"joinid"   , 0x01, 0x0018, 0x00}, ??? dword overlapps next addy !
	char  szGameName[24];	// {"NETWORK"  ,"GAMENAME"    ,"gamename" , 0x02, 0x001A, 0x00},
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
	BYTE  nDifficulty;
	void* pfMPQFunc;		// function ptr; returns 1
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
	BYTE  bUnk223[0x134];	// ?
	BYTE  bSkipToBNet;		// {"NETWORK"     ,"SKIPTOBNET"  ,"W"         , 0x00, 0x0357, 0x00}, //check at d2launch.6FA1D660 CMP BYTE PTR DS:[EAX+357],BL
	BYTE  nScreenSize;		// {"VIDEO"       ,"SCREENSIZE"  ,"sz"     , 0x00, 0x358, 0x00},
	BYTE  bShowLogo;		// {"VIDEO"       ,"SHOWLOGO"    ,"sl"     , 0x00, 0x359, 0x00},
	BYTE  nUnk[0x6E];		// ?

	BYTE nPadding;         // account for the extra option in 1.13c+
};

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