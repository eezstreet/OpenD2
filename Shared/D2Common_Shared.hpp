////////////////////////////////////////////////
//
//	Shared files between game and server
//	Although this file is technically compiled into game.exe,
//	we should not use this information there at all.

#pragma once
#include "D2Shared.hpp"

// Link to D2Common
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

enum D2CharacterStatus
{
	D2STATUS_NEWBIE = 0,
	D2STATUS_HARDCORE = 2,
	D2STATUS_DEAD = 3,
	D2STATUS_EXPANSION = 5,
};

enum D2Difficulties
{
	D2DIFF_NORMAL,
	D2DIFF_NIGHTMARE,
	D2DIFF_HELL,
	D2DIFF_MAX,
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
	BYTE					nCharType;
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

#define Log_WarnAssert(x, y)	if(!x) { trap->Warning(__FILE__, __LINE__, #x); return y; }
#define Log_ErrorAssert(x, y)	if(!x) { trap->Error(__FILE__, __LINE__, #x); return y; }