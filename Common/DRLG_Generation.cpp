#include "DRLG.hpp"

#define MAX_LEVELS	255

static DRLGLevel LevelCache[MAX_LEVELS]{ 0 };

/*
 *	Injects a level into the DRLGMisc structure
 *	@author	eezstreet
 */
static void DRLG_SetLevelSize(DRLGMisc* pMisc, DRLGLevel* pLevel)
{
	D2LevelDefBin* pLevelDef = &sgptDataTables->pLevelDefBin[pLevel->nLevel];

	// The 'depend' field in levels.txt aligns one level along another one.
	if (pLevelDef->dwDepend)
	{
		// Find the depend level first
		DRLGLevel* pDependLevel = pMisc->pFirst;
		while (pDependLevel != nullptr)
		{
			if (pDependLevel->nLevel == pLevelDef->dwDepend)
			{
				// Found it! Match our coordinates.
				pLevel->LvlBox.nX = pDependLevel->LvlBox.nX + pLevelDef->dwOffsetX;
				pLevel->LvlBox.nY = pDependLevel->LvlBox.nY + pLevelDef->dwOffsetY;
				return;
			}

			pDependLevel = pDependLevel->pNext;
		}

		// Didn't find the dependent level, create it!
		pDependLevel = DRLG_GenerateLevel(pMisc, pLevelDef->dwDepend);
		pLevel->LvlBox.nX = pDependLevel->LvlBox.nX + pLevelDef->dwOffsetX;
		pLevel->LvlBox.nY = pDependLevel->LvlBox.nY + pLevelDef->dwOffsetY;
		return;
	}

	pLevel->LvlBox.nX = pLevelDef->dwOffsetX;
	pLevel->LvlBox.nY = pLevelDef->dwOffsetY;
}

/*
 *	Creates (phase 1) a maze level
 *	@author	eezstreet
 */
static void DRLG_MazeLevelCreate(DRLGLevel* pLevel)
{

}

/*
 *	Creates (phase 1) a preset level
 *	@author	eezstreet
 */
static void DRLG_PresetLevelCreate(DRLGLevel* pLevel)
{
	D2LvlPrestTxt* pPrest = DRLG_GetPrestRecord(pLevel->nLevel);

	AssertWithDescription((pPrest != nullptr), "Level labeled as Preset, but no preset claims the level");

	// Create the preset level data - this will initialize the pMap structure within pLevel!
	pLevel->pPreset = (DRLGPresetInfo*)malloc(sizeof(DRLGPresetInfo));
	
	// If the lvlprest entry has its files field set, randomize from those.
	// Otherwise, use the last direction field set in the miscdata.
	if (pPrest->dwFiles)
	{
		pLevel->pPreset->nFile = D2_smrand(&pLevel->LevelSeed, pPrest->dwFiles);
	}
	else
	{
		pLevel->pPreset->nFile = pLevel->pMisc->pRoomEx[0].fRoomAltFlags;
	}

	DRLG_SetLevelSize(pLevel->pMisc, pLevel);
}

/*
 *	Creates (phase 1) an outdoor level
 *	@author	eezstreet
 */
static void DRLG_OutdoorLevelCreate(DRLGLevel* pLevel)
{

}

/*
 *	Generates (phase 2) a maze level
 *	@author	eezstreet
 */
static void DRLG_MazeLevelGenerate(DRLGLevel* pLevel)
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

/*
 *	Generates (phase 2) a preset level
 *	@author	eezstreet
 */
static void DRLG_PresetLevelGenerate(DRLGLevel* pLevel)
{
	// Create the DRLGMap structure.
	// For some reason this resanitizes the file picked...no clue why...Blizzard logic I guess
	pLevel->pPreset->pMap = DRLG_CreateDRLGMap(pLevel, &pLevel->LvlBox);
	pLevel->pPreset->nFile = pLevel->pPreset->pMap->nFilePicked;
}

/*
 *	Generates (phase 2) an outdoor level
 *	@author	eezstreet
 */
static void DRLG_OutdoorLevelGenerate(DRLGLevel* pLevel)
{

}

/*
 *	Creates a new DRLGLevel, supplied with a DRLGMiscData and a level ID.
 *	This is the main point where a level gets generated!
 *	@author	eezstreet
 */
DRLGLevel* DRLG_GenerateLevel(DRLGMisc* pMisc, int nLevel)
{
	// Sanity check the level entered
	Log_ErrorAssert((nLevel >= 0 && nLevel <= MAX_LEVELS), nullptr);

	DRLGLevel* pLevel = &LevelCache[nLevel];
	D2LevelDefBin* pLevelDef = &sgptDataTables->pLevelDefBin[nLevel];

	// Fill in data about the level
	pLevel->dwStartSeed = D2_srand(&pMisc->MiscSeed);
	pLevel->LevelSeed.dwLoSeed = pLevel->dwStartSeed;
	pLevel->LevelSeed.dwHiSeed = SEED_MAGIC;
	pLevel->nLevel = nLevel;
	pLevel->nLevelType = pLevelDef->dwLevelType;
	pLevel->pMisc = pMisc;
	pLevel->eDRLGType = pLevelDef->dwDrlgType;
	pLevel->pNext = pMisc->pFirst;
	pMisc->pFirst = pLevel;

	// If the refresh flag has been set (ie, we are recreating a map that already got spawned),
	// the automap has been revealed and the player is allowed to see it
	if (pMisc->fMiscFlags & DRLGMISC_REFRESHFLAG)
	{
		pLevel->fLevelFlags |= DRLGLVL_AUTOMAP_REVEAL;
	}

	// Each type of generation has two phases - creation and generation (lame names, just roll with it)
	switch (pLevel->eDRLGType)
	{
		case DRLGTYPE_MAZE:
			DRLG_MazeLevelCreate(pLevel);
			DRLG_MazeLevelGenerate(pLevel);
			break;
		case DRLGTYPE_PRESET:
			DRLG_PresetLevelCreate(pLevel);
			DRLG_PresetLevelGenerate(pLevel);
			break;
		case DRLGTYPE_OUTDOORS:
			DRLG_OutdoorLevelCreate(pLevel);
			DRLG_OutdoorLevelGenerate(pLevel);
			break;
	}

	return pLevel;
}

/*
 *	Kills all of the generated levels
 *	@author	eezstreet
 */
void DRLG_FreeLevels()
{
	DRLGLevel* pLevel;
	DRLGMap* pMap;

	for (int i = 0; i < MAX_LEVELS; i++)
	{
		pLevel = &LevelCache[i];

		// Free the DRLG-specific data for this level
		switch (pLevel->eDRLGType)
		{
			case DRLGTYPE_PRESET:
				if (pLevel->pPreset)
				{
					free(pLevel->pPreset);
				}
				break;
			case DRLGTYPE_OUTDOORS:
				if (pLevel->pOutdoors)
				{
					free(pLevel->pOutdoors);
				}
				break;
		}

		// Walk through the DRLGMaps and free them all
		pMap = pLevel->pCurrent;
		while (pMap != nullptr)
		{
			DRLGMap* pPrev = pMap;
			pMap = pMap->pNext;
			free(pPrev);
		}
	}
}