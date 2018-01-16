////////////////////////////////////////////////
//
//	Shared files between game and server
//	Although this file is technically compiled into game.exe,
//	we should not use this information there at all.

#pragma once
#include "D2Shared.hpp"

////////////////////////////////////////////////
//
//	Preprocessor Definitions

#define MAX_LEVEL_CONNECTIONS	8

#ifdef WIN32
#ifndef D2COMMON
#pragma comment(lib, "D2Common.lib")
#endif
#endif

#ifdef D2COMMON
#define D2COMMONAPI	D2EXPORT
#define D2GAMEAPI	D2IMPORT
#else
#define D2COMMONAPI D2IMPORT
#define D2GAMEAPI	D2EXPORT
#endif

////////////////////////////////////////////////
//
//	Data types

typedef handle tile_handle;

// Note: depending on the mod used, this may or may not be correct
enum D2Levels
{
	D2LEVEL_NULL,
	D2LEVEL_ACT1_TOWN,				// Rogue Encampment
	D2LEVEL_ACT1_WILD1,				// Blood Moor
	D2LEVEL_ACT1_WILD2,				// Cold Plains
	D2LEVEL_ACT1_WILD3,				// Stony Field
	D2LEVEL_ACT1_WILD4,				// Dark Wood
	D2LEVEL_ACT1_WILD5,				// Black Marsh
	D2LEVEL_ACT1_WILD6,				// Tamoe Highland
	D2LEVEL_ACT1_CAVE1,				// Den of Evil (Renamed to Cave Level 1 when quest is complete)
	D2LEVEL_ACT1_CAVE2,				// Cave Level 1
	D2LEVEL_ACT1_CAVE3,				// Underground Passage Level 1
	D2LEVEL_ACT1_CAVE4,				// Hole Level 1
	D2LEVEL_ACT1_CAVE5,				// Pit Level 1
	D2LEVEL_ACT1_CAVE2_TREASURE,	// Cave Level 2
	D2LEVEL_ACT1_CAVE3_TREASURE,	// Underground Passage Level 2
	D2LEVEL_ACT1_CAVE4_TREASURE,	// Hole Level 2
	D2LEVEL_ACT1_CAVE5_TREASURE,	// Pit Level 2
	D2LEVEL_ACT1_GRAVEYARD,			// Burial Grounds
	D2LEVEL_ACT1_CRYPT1A,			// Crypt
	D2LEVEL_ACT1_CRYPT1B,			// Mausoleum
	D2LEVEL_ACT1_TOWER2,			// Forgotten Tower
	D2LEVEL_ACT1_CRYPT3A,			// Tower Cellar Level 1
	D2LEVEL_ACT1_CRYPT3B,			// Tower Cellar Level 2
	D2LEVEL_ACT1_CRYPT3C,			// Tower Cellar Level 3
	D2LEVEL_ACT1_CRYPT3D,			// Tower Cellar Level 4
	D2LEVEL_ACT1_CRYPT3E,			// Tower Cellar Level 5
	D2LEVEL_ACT1_MONASTERY,			// Monastery Gate
	D2LEVEL_ACT1_COURTYARD1,		// Outer Cloister
	D2LEVEL_ACT1_BARRACKS,			// Barracks
	D2LEVEL_ACT1_JAIL1,				// Jail Level 1
	D2LEVEL_ACT1_JAIL2,				// Jail Level 2
	D2LEVEL_ACT1_JAIL3,				// Jail Level 3
	D2LEVEL_ACT1_COURTYARD2,		// Inner Cloister
	D2LEVEL_ACT1_CATHEDRAL,			// Cathedral
	D2LEVEL_ACT1_CATACOMBS1,		// Catacombs Level 1
	D2LEVEL_ACT1_CATACOMBS2,		// Catacombs Level 2
	D2LEVEL_ACT1_CATACOMBS3,		// Catacombs Level 3
	D2LEVEL_ACT1_CATACOMBS4,		// Catacombs Level 4
	D2LEVEL_ACT1_TRISTRAM,			// Tristram
	D2LEVEL_ACT1_MOOMOOFARM,		// Moo Moo Farm
	D2LEVEL_ACT2_TOWN,				// Lut Gholein
	D2LEVEL_ACT2_DESERT1,			// Rocky Waste
	D2LEVEL_ACT2_DESERT2,			// Dry Hills
	D2LEVEL_ACT2_DESERT3,			// Far Oasis
	D2LEVEL_ACT2_DESERT4,			// Lost City
	D2LEVEL_ACT2_DESERT5,			// Valley of Snakes
	D2LEVEL_ACT2_VALLEY_OF_KINGS,	// Canyon of the Magi
	D2LEVEL_ACT2_SEWER1A,			// Sewers Level 1
	D2LEVEL_ACT2_SEWER1B,			// Sewers Level 2
	D2LEVEL_ACT2_SEWER1C,			// Sewers Level 3
	D2LEVEL_ACT2_HAREM,				// Harem Level 1
	D2LEVEL_ACT2_CORRUPTHAREM1,		// Harem Level 2
	D2LEVEL_ACT2_BASEMENT1,			// Palace Cellar Level 1
	D2LEVEL_ACT2_BASEMENT2,			// Palace Cellar Level 2
	D2LEVEL_ACT2_BASEMENT3,			// Palace Cellar Level 3
	D2LEVEL_ACT2_TOMB1A,			// Stony Tomb Level 1
	D2LEVEL_ACT2_TOMB2A,			// Halls of the Dead Level 1
	D2LEVEL_ACT2_TOMB2B,			// Halls of the Dead Level 2
	D2LEVEL_ACT2_TOMB3A,			// Claw Viper Temple Level 1
	D2LEVEL_ACT2_TOMB1_TREASURE,	// Stony Tomb Level 2
	D2LEVEL_ACT2_TOMB2_TREASURE,	// Halls of the Dead Level 3
	D2LEVEL_ACT2_TOMB3_TREASURE,	// Claw Viper Temple Level 2
	D2LEVEL_ACT2_LAIR1A,			// Maggot Lair Level 1
	D2LEVEL_ACT2_LAIR1B,			// Maggot Lair Level 2
	D2LEVEL_ACT2_LAIR1_TREASURE,	// Maggot Lair Level 3
	D2LEVEL_ACT2_SEWER2A,			// Ancient Tunnels
	D2LEVEL_ACT2_TOMB_TAL1,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_TOMB_TAL2,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_TOMB_TAL3,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_TOMB_TAL4,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_TOMB_TAL5,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_TOMB_TAL6,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_TOMB_TAL7,			// Tal Rasha's Tomb
	D2LEVEL_ACT2_DURIELS_LAIR,		// Duriel's Lair
	D2LEVEL_ACT2_ARCANE,			// Arcane Sanctuary
	D2LEVEL_ACT3_TOWN,				// Kurast Docks
	D2LEVEL_ACT3_JUNGLE1,			// Spider Forest
	D2LEVEL_ACT3_JUNGLE2,			// Great Marsh
	D2LEVEL_ACT3_JUNGLE3,			// Flayer Jungle
	D2LEVEL_ACT3_KURAST1,			// Lower Kurast
	D2LEVEL_ACT3_KURAST2,			// Kurast Bazaar
	D2LEVEL_ACT3_KURAST3,			// Upper Kurast
	D2LEVEL_ACT3_KURAST4,			// Kurast Causeway
	D2LEVEL_ACT3_TRAVINCAL,			// Travincal
	D2LEVEL_ACT3_SPIDER1,			// Arachnid Lair
	D2LEVEL_ACT3_SPIDER2,			// Spider Cavern
	D2LEVEL_ACT3_DUNGEON1A,			// Swampy Pit Level 1
	D2LEVEL_ACT3_DUNGEON1B,			// Swampy Pit Level 2
	D2LEVEL_ACT3_DUNGEON2A,			// Flayer Dungeon Level 1
	D2LEVEL_ACT3_DUNGEON2B,			// Flayer Dungeon Level 2
	D2LEVEL_ACT3_DUNGEON1_TREASURE,	// Swampy Pit Level 1
	D2LEVEL_ACT3_DUNGEON2_TREASURE,	// Flayer Dungeon Level 3
	D2LEVEL_ACT3_SEWER1,			// Sewers Level 1
	D2LEVEL_ACT3_SEWER2,			// Sewers Level 2
	D2LEVEL_ACT3_TEMPLE1,			// Ruined Temple
	D2LEVEL_ACT3_TEMPLE2,			// Disused Fane
	D2LEVEL_ACT3_TEMPLE3,			// Forgotten Reliquary
	D2LEVEL_ACT3_TEMPLE4,			// Forgotten Temple
	D2LEVEL_ACT3_TEMPLE5,			// Ruined Fane
	D2LEVEL_ACT3_TEMPLE6,			// Disused Reliquary
	D2LEVEL_ACT3_MEPHISTO1,			// Durance of Hate Level 1
	D2LEVEL_ACT3_MEPHISTO2,			// Durance of Hate Level 2
	D2LEVEL_ACT3_MEPHISTO3,			// Durance of Hate Level 3
	D2LEVEL_ACT4_TOWN,				// Pandemonium Fortress
	D2LEVEL_ACT4_MESA1,				// Outer Steppes
	D2LEVEL_ACT4_MESA2,				// Plains of Despair
	D2LEVEL_ACT4_MESA3,				// City of the Damned
	D2LEVEL_ACT4_LAVA1,				// River of Flame
	D2LEVEL_ACT4_DIABLO1,			// Chaos Sanctum
	D2LEVEL_ACT5_TOWN,				// Harrogath
	D2LEVEL_ACT5_SIEGE1,			// Bloody Foothills
	D2LEVEL_ACT5_BARRICADE1,		// Frigid Highlands
	D2LEVEL_ACT5_BARRICADE2,		// Arreat Plateau
	D2LEVEL_ACT5_ICECAVE1,			// Crystalline Passage Level 1
	D2LEVEL_ACT5_ICECAVE1A,			// Frozen River
	D2LEVEL_ACT5_ICECAVE2,			// Glacial Trail
	D2LEVEL_ACT5_ICECAVE2A,			// Drifter Cavern
	D2LEVEL_ACT5_BARRICADE_SNOW,	// Frozen Tundra
	D2LEVEL_ACT5_ICECAVE3,			// The Ancients' Way
	D2LEVEL_ACT5_ICECAVE3A,			// Icy Cellar
	D2LEVEL_ACT5_MOUNTAIN_TOP,		// Arreat Summit
	D2LEVEL_ACT5_TEMPLE_ENTRANCE,	// Nihlathak's Temple
	D2LEVEL_ACT5_TEMPLE1,			// Halls of Anguish
	D2LEVEL_ACT5_TEMPLE2,			// Halls of Pain
	D2LEVEL_ACT5_TEMPLE_BOSS,		// Halls of Vaught
	D2LEVEL_ACT5_HELL1,				// Abaddon
	D2LEVEL_ACT5_HELL2,				// Pit of Acheron
	D2LEVEL_ACT5_HELL3,				// Infernal Pit
	D2LEVEL_ACT5_BAALTEMPLE1,		// Worldstone Keep Level 1
	D2LEVEL_ACT5_BAALTEMPLE2,		// Worldstone Keep Level 2
	D2LEVEL_ACT5_BAALTEMPLE3,		// Worldstone Keep Level 3
	D2LEVEL_ACT5_THRONEROOM,		// Throne of Destruction
	D2LEVEL_ACT5_WORLDSTONE,		// The Worldstone Chamber
};

enum D2LevelTypes
{
	D2LVLTYPE_NONE,
	D2LVLTYPE_ACT1_TOWN,			// Rogue Encampment
	D2LVLTYPE_ACT1_WILDERNESS,		// Blood Moor, Cold Plain, Stony Field ... Moo Moo Farm
	D2LVLTYPE_ACT1_CAVE,			// Den of Evil, Caves, Underground Passage, Pit, Hole
	D2LVLTYPE_ACT1_CRYPT,			// Forgotten Tower, Crypt, Mausoleum
	D2LVLTYPE_ACT1_MONASTERY,		// Monastery Gate
	D2LVLTYPE_ACT1_COURTYARD,		// Outer Cloister, Inner Cloister
	D2LVLTYPE_ACT1_BARRACKS,		// Barracks
	D2LVLTYPE_ACT1_JAIL,			// Jail
	D2LVLTYPE_ACT1_CATHEDRAL,		// Cathedral
	D2LVLTYPE_ACT1_CATACOMBS,		// Catacombs
	D2LVLTYPE_ACT1_TRISTRAM,		// Tristram
	D2LVLTYPE_ACT2_TOWN,			// Lut Gholein
	D2LVLTYPE_ACT2_SEWER,			// Sewers, Ancient Tunnels
	D2LVLTYPE_ACT2_HAREM,			// Harem
	D2LVLTYPE_ACT2_BASEMENT,		// Palace Cellar
	D2LVLTYPE_ACT2_DESERT,			// Rocky Waste, Dry Hills, Far Oasis, Lost City, ...
	D2LVLTYPE_ACT2_TOMB,			// Stony Tomb, Halls of the Dead, Viper Temple, ...
	D2LVLTYPE_ACT2_LAIR,			// Maggot Lair
	D2LVLTYPE_ACT2_ARCANE,			// Arcane Sanctuary
	D2LVLTYPE_ACT3_TOWN,			// Kurast Docks
	D2LVLTYPE_ACT3_JUNGLE,			// Spider Forest, Flayer Jungle, Great Marsh
	D2LVLTYPE_ACT3_KURAST,			// All Kurast, Durance, and Temple levels
	D2LVLTYPE_ACT3_SPIDER,			// Spider Caverns, Arachnid Lair
	D2LVLTYPE_ACT3_DUNGEON,			// Flayer Dungeon, Swampy Pit
	D2LVLTYPE_ACT3_SEWER,			// Sewers
	D2LVLTYPE_ACT4_TOWN,			// Pandemonium Fortress
	D2LVLTYPE_ACT4_MESA,			// Outer Steppes, Plains of Despair, City of the Damned
	D2LVLTYPE_ACT4_LAVA,			// River of Flame, Chaos Sanctum
	D2LVLTYPE_ACT5_TOWN,			// Harrogath
	D2LVLTYPE_ACT5_SIEGE,			// Bloody Foothills
	D2LVLTYPE_ACT5_BARRICADE,		// Frigid Highlands, Nihlathak's Temple, Arreat Summit, ...
	D2LVLTYPE_ACT5_TEMPLE,			// Halls of Pain, Halls of Torment, Halls of Vaught
	D2LVLTYPE_ACT5_ICECAVES,		// Crystalline Passage, Frozen River, Glacial Trail, ...
	D2LVLTYPE_ACT5_BAAL,			// Worldstone Keep
	D2LVLTYPE_ACT5_LAVA,			// Abaddon, Pit of Archeron, Infernal Pit
	D2LVLTYPES_MAX,
};

enum eD2CompositeItem
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
	COMP_COUNT,
	COMP_INVALID = 0xFF,
};

////////////////////////////////////////////////
//
//	Savegame Structs
//

#define D2SAVE_MAGIC	0xAA55AA55

#if GAME_MINOR_VERSION >= 10
#define D2SAVE_VERSION	96
#elif GAME_MINOR_VERSION >= 9
#define D2SAVE_VERSION	92
#else
#define	D2SAVE_VERSION	89
#endif

#pragma pack(1)
struct D2SaveHeaderMercData
{
	BYTE	bMercDead;
	BYTE	nMercReviveCount;
	DWORD	dwMercControl;
	WORD	wMercName;
	WORD	wMercType;
	DWORD	dwMercExperience;
};

struct D2SaveHeader
{
	DWORD					dwMagic;
	DWORD					dwVersion;
	DWORD					dwFileSize;
	DWORD					dwCRC;
	DWORD					dwWeaponSet;
	char					szCharacterName[16];
	BYTE					nCharStatus;
	BYTE					nCharTitle;
	WORD					unk1;
	BYTE					nCharClass;
	WORD					unk2;
	BYTE					nCharLevel;
	DWORD					unk3;
	DWORD					dwCreationTime;
	DWORD					dwModificationTime;
	DWORD					dwSkillKey[16];
	DWORD					dwLeftSkill1;	// left skill for weapon set 1
	DWORD					dwRightSkill1;	// right skill for weapon set 2
	DWORD					dwLeftSkill2;
	DWORD					dwRightSkill2;
	BYTE					nAppearance[16];
	BYTE					nColor[16];
	BYTE					nTowns[D2DIFF_MAX];
	DWORD					dwSeed;
	WORD					unk5;
	D2SaveHeaderMercData	mercData;
	BYTE					nRealmData[0x90];
};

struct D2Savegame
{
	D2SaveHeader			header;
};
#pragma pack()

////////////////////////////////////////////////
//
//	Functions

// D2Common.cpp
D2COMMONAPI void D2Common_Init(D2ModuleImportStrc* pTrap, D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
D2COMMONAPI void D2Common_Shutdown();

// DRLG.cpp
D2COMMONAPI BYTE DRLG_GetLevelActNum(int nLevel);
D2COMMONAPI void DRLG_CreateAct(int nAct, bool bServer, DWORD dwInitSeed, int nDifficulty, int nTown);


////////////////////////////////////////////////
//
//	Preprocessor Macros

#define Log_WarnAssert(x, y)	if(!x) { trap->Warning(__FILE__, __LINE__, #x); return y; }
#define Log_ErrorAssert(x, y)	if(!x) { trap->Error(__FILE__, __LINE__, #x); return y; }
#define WarnWithDescription(x, y, z)	if(!x) { trap->Warning(__FILE__, __LINE__, y); return z; }
#define AssertWithDescription(x, y, z)	if(!x) { trap->Error(__FILE__, __LINE__, y); return z; }