#include "DRLG.hpp"

/*
 *	Creates (phase 1) a maze level
 *	@author	eezstreet
 */
void DRLG_MazeLevelAllocate(DRLGLevel* pLevel)
{
	// TODO: create maze definitions
	DRLG_SetLevelSize(pLevel->pMisc, pLevel);
}

/*
 *	Generates (phase 2) a maze level
 *	@author	eezstreet
 */
void DRLG_MazeLevelGenerate(DRLGLevel* pLevel)
{
	DRLGMisc* pMisc = pLevel->pMisc;
	D2LevelDefBin* pLevelDef = &sgptDataTables->pLevelDefBin[pLevel->nLevel];
	D2LvlMazeTxt* pMazeBin = DRLG_GetMazeRecord(pLevel->nLevel);

	AssertWithDescription((pMazeBin != nullptr), "Couldn't find LvlMaze.bin entry for level");

	pLevel->pMazeInfo = pMazeBin;	// We just push the LvlMaze.bin info into the level itself for safekeeping

									// If we're a special level, we need to make sure to increase the room count
	if (pMisc->nBossTombLvl + D2LEVEL_ACT2_TOMB_TAL1 == pLevel->nLevel ||
		pMisc->nStaffTombLvl + D2LEVEL_ACT2_TOMB_TAL1 == pLevel->nLevel)
	{
		pLevel->pMazeInfo->dwRooms[0] *= 2;
		pLevel->pMazeInfo->dwRooms[1] *= 2;
		pLevel->pMazeInfo->dwRooms[2] *= 2;
	}

	// Create the rooms themselves (...todo...)
}