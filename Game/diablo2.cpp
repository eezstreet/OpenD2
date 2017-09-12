#include "Diablo2.hpp"
#include <cstdlib>
#include <string>

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define co(x)	offsetof(D2GameConfigStrc, x)
D2CmdArgStrc CommandArguments[] = {
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
	{"VIDEO",		"FRAMERATE",	"fr",			CMD_DWORD,		co(dwFramerate),	0x7D},
	{"NETWORK",		"SERVERIP",		"s",			CMD_STRING,		co(szServerIP),		0x00},
	{"NETWORK",		"GAMETYPE",		"gametype",		CMD_DWORD,		co(dwGameType),		0x00},
	{"NETWORK",		"ARENA",		"arena",		CMD_DWORD,		co(dwArena),		0x00},
	{"NETWORK",		"JOINID",		"joinid",		CMD_WORD,		co(wJoinID),		0x00},
	{"NETWORK",		"GAMENAME",		"gamename",		CMD_STRING,		co(szGameName),		0x00},
	{"NETWORK",		"BATTLENETIP",	"bn",			CMD_STRING,		co(szBNetIP),		0x00},
	{"NETWORK",		"MCPIP",		"mcpip",		CMD_STRING,		co(szMCPIP),		0x00},
	{"CHARACTER",	"AMAZON",		"ama",			CMD_BOOLEAN,	co(bAmazon),		0x00},
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
	{"INTERFACE",	"DIFF",			"diff",			CMD_BYTE,		co(nDifficulty),	0x00},
	{"DEBUG",		"LOG",			"log",			CMD_BOOLEAN,	co(bLog),			0x01},
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
	{"NETWORK",		"COMINT",		"comint",		CMD_DWORD,		co(pInterface),		0x00},
	{"NETWORK",		"SKIPTOBNET",	"skiptobnet",	CMD_BOOLEAN,	co(bSkipToBNet),	0x00},
	{"NETWORK",		"OPENC",		"openc",		CMD_BOOLEAN,	co(bOpenC),			0x00},
	{"FILEIO",		"NOCOMPRESS",	"nocompress",	CMD_BOOLEAN,	co(bNoCompress),	0x00},
	{"TXT",			"TXT",			"txt",			CMD_BOOLEAN,	co(bTXT),			0x00},
	{"BUILD",		"BUILD",		"build",		CMD_BOOLEAN,	co(bBuild),			0x00},
	{"",			"",				"",				CMD_BOOLEAN,	0x0000,				0x00},
};
#undef co

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define co(x)	offsetof(OpenD2ConfigStrc, x)
D2CmdArgStrc OpenD2CommandArguments[] = {
	{"FILEIO",		"BASEPATH",		"basepath",		CMD_STRING,		co(szBasePath),		MAX_D2PATH_ABSOLUTE},
	{"FILEIO",		"HOMEPATH",		"homepath",		CMD_STRING,		co(szHomePath),		MAX_D2PATH_ABSOLUTE},
	{"FILEIO",		"MODPATH",		"modpath",		CMD_STRING,		co(szModPath),		MAX_D2PATH_ABSOLUTE},
	{"VIDEO",		"SDLNOACCEL",	"sdlnoaccel",	CMD_BOOLEAN,	co(bNoSDLAccel),	0x00},
	{"VIDEO",		"BORDERLESS",	"borderless",	CMD_BOOLEAN,	co(bBorderless),	0x00},
	{"VIDEO",		"NORENDERTEXT",	"norendertext",	CMD_BOOLEAN,	co(bNoRenderText),	0x00},
	{"FILEIO",		"LOGFLAGS",		"logflags",		CMD_DWORD,		co(dwLogFlags),		PRIORITY_ALL},
	{"",			"",				"",				0,				0x0000,				0x00},
};
#undef co

/////////////////////////////////////////////////////////////////////////////////////////////////////

static D2ModuleImportStrc exports = {
	D2CLIENTAPI_VERSION,

	Log_Print,
	Log_Warning,
	Log_Error,
	GetMilliseconds,

	FS_Open,
	FS_Read,
	FS_Write,
	FS_WritePlaintext,
	FS_CloseFile,
	FS_Seek,
	FS_Tell,
	FS_ListFilesInDirectory,
	FS_FreeFileList,
	FS_CreateSubdirectory,

	In_PumpEvents,
	SDL_StartTextInput,
	SDL_StopTextInput,

	FSMPQ_FindFile,
	MPQ_FileSize,
	MPQ_ReadFile,

	TBL_Register,
	TBL_FindStringFromIndex,
	TBL_FindStringIndexFromKey,
	TBL_FindStringText,
};

static D2ModuleExportStrc* imports[MODULE_MAX]{ 0 };

/*
 *	Get the current number of milliseconds.
 *	Wrapper for SDL_GetTicks
 */
DWORD GetMilliseconds()
{
	return SDL_GetTicks();
}

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
			*(BYTE*)((BYTE*)config + pArg->nOffset) = 1;
			break;
		case CMD_DWORD:
			*(DWORD*)((BYTE*)config + pArg->nOffset) = (DWORD)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_WORD:
			*(WORD*)((BYTE*)config + pArg->nOffset) = (WORD)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_BYTE:
			*(BYTE*)((BYTE*)config + pArg->nOffset) = (BYTE)atoi(arg + strlen(pArg->szCmdName));
			break;
		case CMD_STRING:
			if (*(arg + strlen(pArg->szCmdName)) == '=')
			{
				D2_strncpyz(((char*)config + pArg->nOffset), arg + strlen(pArg->szCmdName) + 1, 32);
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
	DWORD dwArgLen = 0;

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

	dwArgLen = strlen(pArg->szCmdName);

	switch (pArg->dwType)
	{
		case CMD_BOOLEAN:
		default:
			*(BYTE*)((BYTE*)config + pArg->nOffset) = 1;
			break;
		case CMD_DWORD:
			if (*(arg + dwArgLen) == '=')
			{
				*(DWORD*)((BYTE*)config + pArg->nOffset) = (DWORD)atoi(arg + dwArgLen + 1);
			}
			break;
		case CMD_WORD:
			if (*(arg + dwArgLen) == '=')
			{
				*(WORD*)((BYTE*)config + pArg->nOffset) = (WORD)atoi(arg + dwArgLen + 1);
			}
			break;
		case CMD_BYTE:
			if (*(arg + dwArgLen) == '=')
			{
				*(BYTE*)((BYTE*)config + pArg->nOffset) = (BYTE)atoi(arg + dwArgLen + 1);
			}
			break;
		case CMD_STRING:
			// in OpenD2 we take the default argument type as meaning the size of the string to copy into
			if (*(arg + dwArgLen) == '=')
			{
				D2_strncpyz(((char*)config + pArg->nOffset), arg + dwArgLen + 1, pArg->dwDefault);
			}
			break;
	}
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
		BYTE* address = (BYTE*)pvInput + pArg->nOffset;
		switch (pArg->dwType)
		{
			case CMD_BOOLEAN:
			case CMD_BYTE:
				*address = (BYTE)pArg->dwDefault;
				break;
			case CMD_WORD:
				*(WORD*)address = (WORD)pArg->dwDefault;
				break;
			case CMD_DWORD:
				*(DWORD*)address = pArg->dwDefault;
				break;
		}
		++pArg;
	}
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
	PopulateDefaultValues(CommandArguments, pConfig);
	PopulateDefaultValues(OpenD2CommandArguments, pOpenConfig);
}

/*
 *	Kills a game module and cleans up its memory
 */
static void CleanupModule(OpenD2Modules module)
{
	if (imports[module] != nullptr)
	{
		imports[module]->CleanupModule();
	}
	Sys_CloseModule(module);
}

/*
 *	Cleans up all of the game modules
 */
static void CleanupAllModules()
{
	for (int i = MODULE_CLIENT; i < MODULE_MAX; i++)
	{
		CleanupModule((OpenD2Modules)i);
	}
}

/*
 *	Write all game config options to D2.ini
 */
static void WriteGameConfig(D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
{
	fs_handle f;

	FS_Open(GAME_CONFIG_PATH, &f, FS_WRITE);
	Log_ErrorAssert(f != INVALID_HANDLE);

	INI_WriteConfig(&f, pGameConfig, pOpenConfig);

	FS_CloseFile(f);
}

/*
 *	Read all game config options from D2.ini
 */
static void ReadGameConfig(D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
{
	fs_handle f;

	FS_Open(GAME_CONFIG_PATH, &f, FS_READ);
	if (f == INVALID_HANDLE)
	{	// the config file doesn't exist. WE NEED TO COPY FROM THE REGISTRY!
		return;
	}

	INI_ReadConfig(&f, pGameConfig, pOpenConfig);

	FS_CloseFile(f);
}

/*
 *	Initialize the game (from main entrypoint)
 */
OpenD2Modules currentModule = MODULE_CLIENT;

int InitGame(int argc, char** argv, DWORD pid)
{
	D2GameConfigStrc config{ 0 };
	OpenD2ConfigStrc openD2Config{ 0 };

	PopulateConfiguration(&config, &openD2Config);
	ParseCommandline(argc, argv, &config, &openD2Config);

	FS_Init(&openD2Config);
	Log_InitSystem(GAME_LOG_HEADER, GAME_NAME, &openD2Config);
	FS_LogSearchPaths();
	ReadGameConfig(&config, &openD2Config);
	TBL_Init();

	D2Win_InitSDL(&config, &openD2Config); // renderer also gets initialized here
	Render_MapRenderTargetExports(&exports);
	
	// Main loop: execute modules until one of the modules has had enough
	while (currentModule != MODULE_NONE)
	{
		OpenD2Modules previousModule = currentModule;
		DWORD dwPreTick = SDL_GetTicks();
		DWORD dwPostTick, dwFrameMsec;

		// Open the desired module if it does not exist
		if (imports[currentModule] == nullptr)
		{
			imports[currentModule] = Sys_OpenModule(currentModule, &exports);
			if (imports[currentModule] == nullptr || 
				imports[currentModule]->nApiVersion != D2CLIENTAPI_VERSION)
			{
				break;
			}
		}

		// Run the module frame
		currentModule = imports[currentModule]->RunModuleFrame(&config, &openD2Config);

		if (currentModule == MODULE_CLEAN)
		{	// module requested to be cleaned
			CleanupModule(previousModule);
			imports[previousModule] = nullptr;
			currentModule = MODULE_CLIENT;
		}

		// Lock the framerate
		dwPostTick = SDL_GetTicks();
		dwFrameMsec = dwPostTick - dwPreTick;
		if (config.dwFramerate > 0 && dwFrameMsec < (1000 / config.dwFramerate))
		{
			SDL_Delay((1000 / config.dwFramerate) - dwFrameMsec);
		}
	}

	CleanupAllModules();

	D2Win_ShutdownSDL();	// renderer also gets shut down here

	WriteGameConfig(&config, &openD2Config);
	TBL_Cleanup();
	Log_Shutdown();
	FS_Shutdown();

	return 0;
}