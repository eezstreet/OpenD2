#include "Diablo2.hpp"
#include <cstdlib>
#include <string>
#include "../Shared/Fog.h"
#include "../Shared/D2Win.h"
#include "../Shared/D2Gfx.h"
#include "../Shared/D2Sound.h"
#include "../Shared/D2MCPClient.h"


enum D2CommandType
{
	CMD_BOOLEAN,
	CMD_DWORD,
	CMD_STRING,
	CMD_BYTE,
	CMD_WORD
};

enum D2RenderMode
{
	RENDER_NONE,
	RENDER_GDI,
	RENDER_SOFTWARE,
	RENDER_DIRECTDRAW,
	RENDER_GLIDE,
	RENDER_OPENGL,
	RENDER_DIRECT3D,
	RENDER_RAVE,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define co(x)	offsetof(D2GameConfigStrc, x)
static D2CmdArgStrc CommandArguments[] = {
//	INI section		INI key			cmdline arg		type			offset				default value
	{"VIDEO",		"WINDOW",		"w",			CMD_BOOLEAN,	co(bWindowed),		0x00},
	{"VIDEO",		"3DFX",			"3dfx",			CMD_BOOLEAN,	co(b3DFX),			0x00},
	{"VIDEO",		"OPENGL",		"opengl",		CMD_BOOLEAN,	co(bOpenGL),		0x00},
	{"VIDEO",		"D3D",			"d3d",			CMD_BOOLEAN,	co(bD3D),			0x00},
	{"VIDEO",		"RAVE",			"rave",			CMD_BOOLEAN,	co(bRave),			0x00},
	{"VIDEO",		"PERSPECTIVE",	"per",			CMD_BOOLEAN,	co(bPerspective),	0x00},
	{"VIDEO",		"QUALITY",		"lq",			CMD_BOOLEAN,	co(bQuality),		0x00},
	{"VIDEO",		"GAMMA",		"gamma",		CMD_BOOLEAN,	co(dwGamma),		0x00},
	{"VIDEO",		"VSYNC",		"vsync",		CMD_BOOLEAN,	co(bVSync),			0x00},
	{"VIDEO",		"FRAMERATE",	"fr",			CMD_DWORD,		co(dwFramerate),	0x00},
	{"NETWORK",		"SERVERIP",		"s",			CMD_STRING,		co(szServerIP),		0x00},
	{"NETWORK",		"GAMETYPE",		"gametype",		CMD_DWORD,		co(dwGameType),		0x00},
	{"NETWORK",		"ARENA",		"arena",		CMD_DWORD,		co(dwArena),		0x00},
	{"NETWORK",		"JOINID",		"joinid",		CMD_WORD,		co(wJoinID),		0x00},
	{"NETWORK",		"GAMENAME",		"gamename",		CMD_STRING,		co(szGameName),		0x00},
	{"NETWORK",		"BATTLENETIP",	"bn",			CMD_STRING,		co(szBNetIP),		0x00},
	{"NETWORK",		"MCPIP",		"mcpip",		CMD_STRING,		co(szMCPIP),		0x00},
	{"CHARACTER",	"AMAZON",		"ama",			CMD_BOOLEAN,	co(bAmazon),		0x01},
	{"CHARACTER",	"PALADIN",		"pal",			CMD_BOOLEAN,	co(bPaladin),		0x00},
	{"CHARACTER",	"SORCERESS",	"sor",			CMD_BOOLEAN,	co(bSorceress),		0x00},
	{"CHARACTER",	"NECROMANCER",	"nec",			CMD_BOOLEAN,	co(bNecromancer),	0x00},
	{"CHARACTER",	"BARBARIAN",	"bar",			CMD_BOOLEAN,	co(bBarbarian),		0x00},
	{"CHARACTER",	"DRUID",		"dru",			CMD_BOOLEAN,	co(bDruid),			0x00},
	{"CHARACTER",	"ASSASSIN",		"sas",			CMD_BOOLEAN,	co(bAssassin),		0x00},
	{"CHARACTER",	"INVINCIBLE",	"i",			CMD_BOOLEAN,	co(bInvincible),	0x00},
	{"CHARACTER",	"NAME",			"name",			CMD_STRING,		co(szCharName),		0x00},
	{"CHARACTER",	"REALM",		"realm",		CMD_STRING,		co(szRealm),		0x00},
	{"CHARACTER",	"CTEMP",		"ctemp",		CMD_DWORD,		co(dwCTemp),		0x00},
	{"MONSTER",		"NOMONSTER",	"nm",			CMD_BOOLEAN,	co(bNoMonster),		0x00},
	{"MONSTER",		"MONSTERCLASS",	"m",			CMD_DWORD,		co(dwMonClass),		0x00},
	{"MONSTER",		"MONSTERINFO",	"minfo",		CMD_BOOLEAN,	co(bMonInfo),		0x00},
	{"MONSTER",		"MONSTERDEBUG",	"md",			CMD_DWORD,		co(dwMonDebug),		0x00},
	{"ITEM",		"RARE",			"rare",			CMD_BOOLEAN,	co(bRare),			0x00},
	{"ITEM",		"UNIQUE",		"unique",		CMD_BOOLEAN,	co(bUnique),		0x00},
	{"INTERFACE",	"ACT",			"act",			CMD_DWORD,		co(dwAct),			0x01},
	{"INTERFACE",	"DIFF",			"diff",			CMD_BYTE,		co(nDifficulty),	0x03},
	{"DEBUG",		"LOG",			"log",			CMD_BOOLEAN,	co(bLog),			0x00},
	{"DEBUG",		"MSGLOG",		"msglog",		CMD_BOOLEAN,	co(bMsgLog),		0x00},
	{"DEBUG",		"SAFEMODE",		"safe",			CMD_BOOLEAN,	co(bSafeMode),		0x00},
	{"DEBUG",		"NOSAVE",		"nosave",		CMD_BOOLEAN,	co(bNoSave),		0x00},
	{"DEBUG",		"SEED",			"seed",			CMD_DWORD,		co(dwSeed),			0x00},
	{"NETWORK",		"NOPK",			"nopk",			CMD_BOOLEAN,	co(bNoPK),			0x00},
	{"DEBUG",		"CHEATS",		"cheats",		CMD_BOOLEAN,	co(bCheats),		0x00},
	{"DEBUG",		"TEEN",			"teen",			CMD_BOOLEAN,	co(bTeen),			0x00},
	{"DEBUG",		"NOSOUND",		"ns",			CMD_BOOLEAN,	co(bNoSound),		0x00},
	{"FILEIO",		"NOPREDLOAD",	"npl",			CMD_BOOLEAN,	co(bNoPreload),		0x00},
	{"FILEIO",		"DIRECT",		"direct",		CMD_BOOLEAN,	co(bDirect),		0x00},
	{"FILEIO",		"LOWEND",		"lem",			CMD_BOOLEAN,	co(bLowEnd),		0x00},
	{"DEBUG",		"QuEsTs",		"questall",		CMD_BOOLEAN,	co(bQuests),		0x00},
	{"DEBUG",		"CHOICE",		"choice",		CMD_BOOLEAN,	0x03CC,				0x00},	// FIXME
	{"NETWORK",		"COMINT",		"comint",		CMD_DWORD,		co(pInterface),		0x00},
	{"NETWORK",		"SKIPTOBNET",	"skiptobnet",	CMD_BOOLEAN,	co(bSkipToBNet),	0x00},
	{"NETWORK",		"OEPNC",		"openc",		CMD_BOOLEAN,	co(bOpenC),			0x00},
	{"FILEIO",		"NOCOMPRESS",	"nocompress",	CMD_BOOLEAN,	co(bNoCompress),	0x00},
	{"TXT",			"TXT",			"txt",			CMD_BOOLEAN,	co(bTXT),			0x00},
	{"BUILD",		"BUILD",		"build",		CMD_BOOLEAN,	co(bBuild),			0x00},
	{"",			"",				"",				CMD_BOOLEAN,	0x0000,				0x00},
};
#undef co

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define co(x)	offsetof(OpenD2ConfigStrc, x)
static D2CmdArgStrc OpenD2CommandArguments[] = {
	{"FILEIO",		"BASEPATH",		"basepath",		CMD_STRING,		co(szBasePath),		MAX_D2PATH_ABSOLUTE},
	{"FILEIO",		"HOMEPATH",		"homepath",		CMD_STRING,		co(szHomePath),		MAX_D2PATH_ABSOLUTE},
	{"FILEIO",		"MODPATH",		"modpath",		CMD_STRING,		co(szModPath),		MAX_D2PATH_ABSOLUTE},
	{"",			"",				"",				0,				0x0000,				0x00},
};
#undef co

/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 *	Processes a single commandline argument
 */
void ProcessDiablo2Argument(char* arg, D2GameConfigStrc* config)
{
	D2CmdArgStrc* pArg;

	if (arg[0] == '\0')
	{	// some smart-alec decided to put a dash for the fun of it...
		return;
	}

	pArg = CommandArguments;
	while (pArg != nullptr && pArg->szCmdName[0] != '\0')
	{
		if (!D2_stricmpn(arg, pArg->szCmdName, strlen(pArg->szCmdName)))
		{
			// It's this one!
			break;
		}
		++pArg;
	}

	if (pArg == nullptr || pArg->szCmdName[0] == '\0')
	{	// not valid
		return;
	}

	switch (pArg->dwType)
	{
		case CMD_BOOLEAN:
		default:
			*(BYTE*)(config + pArg->nOffset) = true;
			break;
		case CMD_DWORD:
			*(DWORD*)(config + pArg->nOffset) = (DWORD)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_WORD:
			*(WORD*)(config + pArg->nOffset) = (WORD)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_BYTE:
			*(BYTE*)(config + pArg->nOffset) = (BYTE)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_STRING:
			if (*(arg + strlen(pArg->szCmdName)) == '=')
			{
				D2_strncpyz((char*)(config + pArg->nOffset), arg + strlen(pArg->szCmdName) + 1, 32);
			}
			break;
	}
}

/*
 *	Processes a single OpenD2 argument.
 */
void ProcessOpenD2Argument(char* arg, OpenD2ConfigStrc* config)
{
	D2CmdArgStrc* pArg;

	if (arg[0] == '\0')
	{	// some smart-alec decided to put just a regular old + here
		return;
	}

	pArg = OpenD2CommandArguments;
	while (pArg != nullptr && pArg->szCmdName[0] != '\0')
	{
		if (!D2_stricmpn(arg, pArg->szCmdName, strlen(pArg->szCmdName)))
		{	// it's this one
			break;
		}
		++pArg;
	}

	if (pArg == nullptr || pArg->szCmdName[0] == '\0')
	{	// not valid
		return;
	}

	switch (pArg->dwType)
	{
		case CMD_BOOLEAN:
		default:
			*(BYTE*)(config + pArg->nOffset) = true;
			break;
		case CMD_DWORD:
			*(DWORD*)(config + pArg->nOffset) = (DWORD)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_WORD:
			*(WORD*)(config + pArg->nOffset) = (WORD)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_BYTE:
			*(BYTE*)(config + pArg->nOffset) = (BYTE)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_STRING:
			// in OpenD2 we take the default argument type as meaning the size of the string to copy into
			if (*(arg + strlen(pArg->szCmdName)) == '=')
			{
				D2_strncpyz((char*)(config + pArg->nOffset), arg + strlen(pArg->szCmdName) + 1, pArg->dwDefault);
			}
			break;
	}
}

/*
 *	Get the rendering mode
 */
inline DWORD GetRenderingMode(D2GameConfigStrc* pConfig)
{
	DWORD dwRenderingMode = RENDER_NONE;
	if (pConfig->b3DFX)
	{
		dwRenderingMode = RENDER_GLIDE;
	}
	else if (pConfig->bWindowed)
	{
		dwRenderingMode = RENDER_GDI;
	}
	else if (pConfig->bD3D)
	{
		dwRenderingMode = RENDER_DIRECT3D;
	}
	else
	{
		dwRenderingMode = RENDER_DIRECTDRAW;
	}

	// NOTE: OpenGL and Software rendering modes cannot occur!!
	return dwRenderingMode;
}

/*
 *	Pump a single frame from the module
 */
D2InterfaceModules PumpModuleFrame(D2InterfaceModules dwInterface, D2GameConfigStrc* pConfig)
{
	if (gpfModules[dwInterface] != nullptr)
	{
		return gpfModules[dwInterface](pConfig);
	}
	return D2I_NONE;
}

/*
 *	Parse commandline arguments
 */
void ParseCommandline(int argc, char** argv, D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	char* arg;

	// Process the commandline arguments
	for (int i = 1; i < argc; i++)
	{
		arg = argv[i];
		if (arg[0] == '\0')
		{	// out of arguments
			break;
		}
		else if (arg[0] == '-')
		{	// Diablo II game setting
			ProcessDiablo2Argument(arg + 1, pConfig);
		}
		else if (arg[0] == '+')
		{	// OpenD2 game setting
			ProcessOpenD2Argument(arg + 1, pOpenConfig);
		}
	}
}

/*
 *	Pull default values from a D2CmdArgStrc array
 */
static void PopulateDefaultValues(D2CmdArgStrc* paCommandArguments, void* pvInput)
{
	D2CmdArgStrc* pArg = paCommandArguments;
	while (pArg != nullptr && pArg->szKeyName[0] != '\0')
	{
		switch (pArg->dwType)
		{
			case CMD_BOOLEAN:
			case CMD_BYTE:
				*((BYTE*)pvInput + pArg->nOffset) = (BYTE)pArg->dwDefault;
				break;
			case CMD_WORD:
				*((WORD*)pvInput + pArg->nOffset) = (WORD)pArg->dwDefault;
				break;
			case CMD_DWORD:
				*((DWORD*)pvInput + pArg->nOffset) = pArg->dwDefault;
				break;
		}
		++pArg;
	}
}

/*
 *	A (tiny) callback that is needed by the game for some reason
 */
int __stdcall LoadExpansionMPQ()
{
	return 1;
}

/*
 *	Populate the config with default values
 */
static void PopulateConfiguration(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
#ifdef EXPANSION
	pConfig->dwExpansion = 1;
#endif

	// Push the default values from the commandline settings for both OpenD2 and the retail game
	//PopulateDefaultValues(CommandArguments, pConfig);
	//PopulateDefaultValues(OpenD2CommandArguments, pOpenConfig);

	// Set the default MPQ callback
#if GAME_MINOR_VERSION >= 13	// In 1.13 we apply a +1 offset to keep it kosher
	*(void**)((BYTE*)&pConfig->pfMPQFunc + 1) = LoadExpansionMPQ;
#else
	pConfig->pfMPQFunc = LoadExpansionMPQ;
#endif
}

/*
 *	Initialize the game (from main entrypoint)
 */
int InitGame(int argc, char** argv, DWORD pid)
{
	D2GameConfigStrc config{ 0 };
	OpenD2ConfigStrc openD2Config{ 0 };
	bool bSuccess;
	DWORD dwRenderingMode;
	D2InterfaceModules dwCurrentModule;

	PopulateConfiguration(&config, &openD2Config);
	ParseCommandline(argc, argv, &config, &openD2Config);

	FS_Init(&openD2Config);
	FSMPQ_Init();

	// Testing
	/*D2MPQArchive* pMusicMPQ = FSMPQ_AddSearchPath("D2MUSIC", "d2music.mpq");
	D2MPQArchive* pVideoMPQ = FSMPQ_AddSearchPath("D2VIDEO", "d2video.mpq");
	D2MPQArchive* pSpeechMPQ = FSMPQ_AddSearchPath("D2SPEECH", "d2speech.mpq");
	D2MPQArchive* pSFXMPQ = FSMPQ_AddSearchPath("D2SFX", "d2sfx.mpq");
	D2MPQArchive* pDataMPQ = FSMPQ_AddSearchPath("D2DATA", "d2data.mpq");
	D2MPQArchive* pCharsMPQ = FSMPQ_AddSearchPath("D2CHAR", "d2char.mpq");
	D2MPQArchive* pExpTalkMPQ = FSMPQ_AddSearchPath("D2XTALK", "d2xtalk.mpq");
	D2MPQArchive* pExpMPQ = FSMPQ_AddSearchPath("D2EXP", "d2exp.mpq");
	D2MPQArchive* pPatchMPQ = FSMPQ_AddSearchPath("PATCH", "Patch_D2.mpq");*/

	dwRenderingMode = GetRenderingMode(&config);

	/*
	 *	BIG HUGE TODO LIST
	 *	- Rewrite all of Fog (in Shared/*)
	 *	- Rewrite all of Storm (in Shared/* and Game/*)
	 *	- Rewrite all of D2Win and make this function more platform-agnostic
	 */

	FOG_10021(GAME_LOG_PATH);	// Init log manager
	FOG_10019(GAME_NAME, 0, "v" GAME_VERSION, GAME_MAJOR_VERSION);	// Init system

#if GAME_MINOR_VERSION >= 13
	// set up the ignore list
#endif

	FOG_10101(config.bDirect, 0);	// Set working directory
	FOG_10089(1, 0);	// Init async data
	FOG_10218();	// Init mask table

	bSuccess = D2WIN_10037();	// load archives
	if (!bSuccess)
	{
		D2WIN_10036();	// unload archives
		return -1;
	}

	bSuccess = D2WIN_10171(D2WIN_10174, D2WIN_10205, 0, &config);	// load expansion archives
	if (!bSuccess)
	{
		D2WIN_10036();	// unload archives
		return -1;
	}

	// Create the window
	bSuccess = D2WIN_10000((HINSTANCE)pid, dwRenderingMode, config.bWindowed, !config.bNoCompress);
	if (!bSuccess)
	{
		D2WIN_10036();	// unload archives
		return -1;
	}

	if (config.bPerspective && dwRenderingMode >= RENDER_GLIDE)
	{	// set perspective mode
		D2GFX_10011(true);
	}

	bSuccess = D2WIN_10001(config.bWindowed, config.bWindowed ? 2 : 0);	// init sprite cache
	if (!bSuccess)
	{
		D2GFX_10001();	// destroy window
		D2WIN_10036();	// unload archives
		return -1;
	}

	if (config.bLowEnd)
	{	// enable low-end graphics
		D2GFX_10015();
	}

	if (config.dwGamma != 0)
	{	// set gamma
		D2GFX_10018(config.dwGamma);
	}

	if (config.bVSync)
	{	// enable vsync
		D2GFX_10020();
	}

#if GAME_MINOR_VERSION >= 13
	// set the fixed aspect ratio
#endif

	if (!config.bNoSound)
	{	// initialize the sound system
		D2SOUND_10000(config.dwExpansion);
	}

	// Initialize the external modules.
	Sys_InitModules();

	// Starting with Launch, ping-pong between all of the different modules until one of them dies
	dwCurrentModule = D2I_LAUNCH;
	while (dwCurrentModule = PumpModuleFrame(dwCurrentModule, &config));

	if (!config.bNoSound)
	{	// kill the sound system
		D2SOUND_10001();
	}

	D2WIN_10002();			// destroy sprite cache
	D2GFX_10001();			// destroy window
	D2WIN_10036();			// destroy archives
	FOG_10090();			// destroy async data
	//D2MCPCLIENT_10001();	// destroy MCP client (FIXME: kind of a bad place for this..)
	FOG_10143(nullptr);		// kill fog memory

	FSMPQ_Shutdown();
	FS_Shutdown();

	return 0;
}