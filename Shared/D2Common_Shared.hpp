////////////////////////////////////////////////
//
//	Shared files between game and server
//	Although this file is technically compiled into game.exe,
//	we should not use this information there at all.

#pragma once
#include "D2Shared.hpp"

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
	D2STATUS_HARDCORE = 2,
	D2STATUS_DEAD = 3,
	D2STATUS_EXPANSION = 5,
};