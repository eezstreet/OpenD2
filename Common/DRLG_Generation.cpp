#include "DRLG.hpp"

static DRLGLevel LevelCache[MAX_LEVELS]{ 0 };

/*
 *	Assigns outdoor level generation data to a room
 *	@author	eezstreet
 */
static void DRLGRoom_AssignOutdoorData(DRLGRoomEx* pRoom)
{
	pRoom->pOutdoor = (DRLGOutdoorRoom*)malloc(sizeof(DRLGOutdoorRoom));
	memset(pRoom->pOutdoor, 0, sizeof(DRLGOutdoorRoom));
}

/*
 *	Assigns preset level generation data to a room
 *	@author	eezstreet
 */
static void DRLGRoom_AssignMazePresetData(DRLGRoomEx* pRoom)
{
	pRoom->pPreset = (DRLGPresetRoom*)malloc(sizeof(DRLGPresetRoom));
	memset(pRoom->pPreset, 0, sizeof(DRLGPresetRoom));
}

/*
 *	Allocates a RoomEx
 *	@author	eezstreet
 */
DRLGRoomEx* DRLG_AllocateRoomEx(DRLGLevel* pLevel, int nPresetType)
{
	DRLGRoomEx* pRoom = (DRLGRoomEx*)malloc(sizeof(DRLGRoomEx));
	memset(pRoom, 0, sizeof(DRLGRoomEx));

	pRoom->pLevel = pLevel;
	pRoom->ePresetType = nPresetType;
	pRoom->eRoomStatus = STATUS_INITED_UNTILED;
	D2_seedcopy(&pRoom->RoomExSeed, &pLevel->LevelSeed);
	pRoom->dwInitSeed = pRoom->RoomExSeed.dwLoSeed;

	// Reveal it on the automap, if it needs to be revealed
	if (pLevel->fLevelFlags & DRLGLVL_AUTOMAP_REVEAL)
	{
		pRoom->fRoomExFlags |= ROOMEXFLAG_AUTOMAP_REVEAL;
	}

	// Allocate the roomtype-specific data
	if (nPresetType == ROOMTYPE_OUT)
	{
		DRLGRoom_AssignOutdoorData(pRoom);
	}
	else if (nPresetType == ROOMTYPE_MAZE_PRESET)
	{
		DRLGRoom_AssignMazePresetData(pRoom);
	}

	return pRoom;
}

/*
 *	Binds a RoomEx to a level by incrementing the room count and adding it to the linked list.
 *	Not always called after creating a room!
 *	@author	eezstreet
 */
void DRLG_BindRoomEx(DRLGLevel* pLevel, DRLGRoomEx* pRoomEx)
{
	pRoomEx->pNext = pLevel->pFirstRoomEx;
	pLevel->nRooms++;
	pLevel->pFirstRoomEx = pRoomEx;
}

/*
 *	Builds a grid room.
 *	Note that there is a separate outdoor version of this function.
 *	@author	eezstreet
 */
static void DRLG_BuildGridRoom(DRLGLevel* pLevel, DRLGMap* pMap, int nFlags, bool bMaze)
{
	int dt1Mask = pMap->pLvlPrest->dwDt1Mask;
	DRLGGrid grid;
	DWORD gridSectors[4096]{ 0 };
	DWORD gridFlags[4096]{ 0 };
	int nGridFlags;

	// Create a grid with some slack space
	DRLG_CreateGrid_NoAllocations(&grid, 
		pMap->MapBox.nW / DRLG_GRID_SIZE + 1, pMap->MapBox.nH / DRLG_GRID_SIZE + 1, 
		gridFlags, gridSectors);


	if (pMap->pLvlPrest->dwOutdoors)
	{
		nFlags |= 0x80000; // ?
	}

	if (bMaze)
	{
		nGridFlags = DRLG_GetGridFlags(&grid, 0, 0);
		DRLG_InsertPresetRoom(pLevel, pMap, &pMap->MapBox, dt1Mask, nGridFlags, 1, nullptr);
		return;
	}
}

/*
 *	Creates a DRLGGrid with the appropriate size. Allocates the grid sectors also
 *	@author	eezstreet
 */
static void DRLG_CreateGrid(DRLGGrid* pGrid, int nWidth, int nHeight)
{
	size_t dwTotalSectorSize = sizeof(DWORD) * nHeight * (nWidth + 1);
	int i, j;

	pGrid->nWidth = nWidth;
	pGrid->nHeight = nHeight;
	
	// allocate the grid sectors and initialize them
	pGrid->pGridSectors = (DWORD*)malloc(dwTotalSectorSize);
	memset(pGrid->pGridSectors, 0, dwTotalSectorSize);
	pGrid->pGridFlags = &pGrid->pGridSectors[nHeight];

	for (i = 0, j = 0; i < nHeight; i++)
	{
		pGrid->pGridSectors[i] = j;
		j += nWidth;
	}

	pGrid->bInited = false;	// ?
}

/*
 *	Creates a DRLGGrid from static memory (no allocations)
 *	@author	eezstreet
 */
static void DRLG_CreateGrid_NoAllocations(DRLGGrid* pGrid, int nWidth, int nHeight, DWORD* pGridFlags, DWORD* pGridSectors)
{
	size_t dwTotalSectorSize = sizeof(DWORD) * nWidth * nHeight;

	pGrid->nWidth = nWidth;
	pGrid->nHeight = nHeight;
	pGrid->pGridFlags = pGridFlags;
	pGrid->pGridSectors = pGridSectors;

	for (int i = 0, j = 0; i < nHeight; i++, j += nWidth)
	{
		pGrid->pGridSectors[i] = j;
	}

	pGrid->bInited = false; // ?
}

/*
 *	Gets the grid flags at a specific x/y coordinate
 *	@author	eezstreet
 */
DWORD DRLG_GetGridFlags(DRLGGrid* pGrid, int x, int y)
{
	return pGrid->pGridFlags[x + pGrid->pGridSectors[y]];
}

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
static void DRLG_MazeLevelAllocate(DRLGLevel* pLevel)
{

}

/*
 *	Creates (phase 1) a preset level
 *	@author	eezstreet
 */
static void DRLG_PresetLevelAllocate(DRLGLevel* pLevel)
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
static void DRLG_OutdoorLevelAllocate(DRLGLevel* pLevel)
{
	// Create the DRLGOutdoorInfo structure
	pLevel->pOutdoors = (DRLGOutdoorInfo*)malloc(sizeof(DRLGOutdoorInfo));
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
	DRLGOutdoorInfo* pOutdoorInfo = pLevel->pOutdoors;
	DRLGVer* pCurrent;
	DRLGVer* pPrev;
	char nAct = DRLG_GetLevelActNum(pLevel->nLevel);
	int nDt1Mask = 0;
	int x, y;
	DRLGCoordBox placeCoords;

	pOutdoorInfo->nW = 0;
	pOutdoorInfo->nH = 0;
	pOutdoorInfo->wGrid = pLevel->LvlBox.nW / DRLG_GRID_SIZE;
	pOutdoorInfo->hGrid = pLevel->LvlBox.nH / DRLG_GRID_SIZE;

	DRLG_CreateGrid(&pOutdoorInfo->pGrid[0], pOutdoorInfo->wGrid, pOutdoorInfo->hGrid);
	DRLG_CreateGrid(&pOutdoorInfo->pGrid[1], pOutdoorInfo->wGrid, pOutdoorInfo->hGrid);
	DRLG_CreateGrid(&pOutdoorInfo->pGrid[2], pOutdoorInfo->wGrid, pOutdoorInfo->hGrid);
	DRLG_CreateGrid(&pOutdoorInfo->pGrid[3], pOutdoorInfo->wGrid, pOutdoorInfo->hGrid);

	DRLG_CreateOutdoorVerData(&pOutdoorInfo->pVer, &pLevel->LvlBox, 0, pOutdoorInfo->pData);

	// walk backwards and make sure everything snaps to the grid
	pCurrent = pOutdoorInfo->pVer;
	do
	{	// FIXME: maybe move this to DRLG_CreateOutdoorVerData?
		pCurrent->nX /= DRLG_GRID_SIZE;
		pCurrent->nY /= DRLG_GRID_SIZE;
		pCurrent = pCurrent->pPrev;
	} while (pCurrent != pOutdoorInfo->pVer);

	// walk backwards and make sure that we don't have any overlapping DRLGVer data
	do
	{	// FIXME: maybe move this to DRLG_CreateOutdoorVerData?
		pPrev = pCurrent->pPrev;
		if (pPrev->nX == pCurrent->nX)
		{
			if (pPrev->nY == pCurrent->nY)
			{
				// this one is bad, its coordinates match the previous one
				if (pPrev == pOutdoorInfo->pVer)
				{
					// it can be very bad if we are modifying the thing that the outdoor data points to!
					pOutdoorInfo->pVer = pCurrent;
				}

				// inherit everything from the previous...
				pCurrent->pPrev = pPrev->pPrev;
				pCurrent->dwVerFlag |= pPrev->dwVerFlag;
				pCurrent->eDir = pPrev->eDir;

				// ...and then kill it!
				free(pPrev);
			}
		}
		pCurrent = pPrev;
	} while (pCurrent != pOutdoorInfo->pVer);

	// Act-specific level generation details
	switch (nAct)
	{
		case D2_ACT_I:
			DRLG_CreateAct1OutdoorsLevel(pLevel);
			break;
		case D2_ACT_II:
			DRLG_CreateAct2OutdoorsLevel(pLevel);
			break;
		case D2_ACT_III:
			DRLG_CreateAct3OutdoorsLevel(pLevel);
			break;
		case D2_ACT_IV:
			DRLG_CreateAct4OutdoorsLevel(pLevel);
			break;
		case D2_ACT_V:
			DRLG_CreateAct5OutdoorsLevel(pLevel);
			break;
	}

	// Leveltype-specific generation details
	// FIXME: figure out what these are masked together
	switch (pLevel->nLevelType)
	{
		case D2LVLTYPE_ACT1_WILDERNESS:
			nDt1Mask = 278787;
			break;
		case D2LVLTYPE_ACT3_JUNGLE:
			nDt1Mask = 4;
			break;
		case D2LVLTYPE_ACT2_DESERT:
		case D2LVLTYPE_ACT3_KURAST:
		case D2LVLTYPE_ACT4_MESA:
		case D2LVLTYPE_ACT4_LAVA:
			nDt1Mask = 1;
			break;
		case D2LVLTYPE_ACT5_SIEGE:
		case D2LVLTYPE_ACT5_BARRICADE:
			nDt1Mask = 17;
			break;
	}

	// Build along the grid
	for (y = 0; y < pOutdoorInfo->hGrid; y++)
	{
		for (x = 0; x < pOutdoorInfo->wGrid; x++)
		{
			int nRoomFlag = DRLG_GetGridFlags(&pOutdoorInfo->pGrid[DRLGGRID_ROOMFLAG], x, y);
			int nOutdoorFlag = DRLG_GetGridFlags(&pOutdoorInfo->pGrid[DRLGGRID_OUTLVLFLAG], x, y);
			int nOtherFlag;

			if (nOutdoorFlag & OUTDOOR_FLAG2)	// no idea what this flag does
			{
				nOtherFlag = DRLG_GetGridFlags(&pOutdoorInfo->pGrid[DRLGGRID_LVLPREST], x, y);
				if (nOtherFlag != 0)
				{	// if the preset hasn't been spawned, we need to place it
					DRLGMap* pMap;

					placeCoords.nX = pLevel->LvlBox.nX + (x * DRLG_GRID_SIZE);
					placeCoords.nY = pLevel->LvlBox.nY + (y * DRLG_GRID_SIZE);
					placeCoords.nW = placeCoords.nH = 0;

					pMap = DRLG_CreateDRLGMap(pLevel, &placeCoords);
					pMap->nFilePicked = (nOutdoorFlag >> 16) & 0xF;	// align it along the direction
				}
			}
			else if (!(nOutdoorFlag & OUTDOOR_FLAG1))	// or this one either
			{
				nOtherFlag = DRLG_GetGridFlags(&pOutdoorInfo->pGrid[DRLGGRID_UNUSED], x, y);
				DRLG_InsertOutdoorGridRoom(pLevel, x, y, DRLG_GRID_SIZE, DRLG_GRID_SIZE, 
					nRoomFlag, nOutdoorFlag, nOtherFlag, nDt1Mask);
			}
		}
	}
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

	if (pLevel->pNext != nullptr)
	{
		return pLevel;	// There's no need to regenerate if this level already exists.
	}

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

	// Each type of generation has two phases - allocation and generation
	switch (pLevel->eDRLGType)
	{
		case DRLGTYPE_MAZE:
			DRLG_MazeLevelAllocate(pLevel);
			DRLG_MazeLevelGenerate(pLevel);
			break;
		case DRLGTYPE_PRESET:
			DRLG_PresetLevelAllocate(pLevel);
			DRLG_PresetLevelGenerate(pLevel);
			break;
		case DRLGTYPE_OUTDOORS:
			DRLG_OutdoorLevelAllocate(pLevel);
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