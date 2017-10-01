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
 *	Builds an area. Only should be called from DRLG_BuildPresetRooms
 *	@author	eezstreet
 */
static void DRLG_BuildArea(DRLGLevel* pLevel, DRLGGrid* pGrid, int nFlags, DRLGMap* pMap, bool bMaze)
{
	int* vis = DRLG_GetVisLevels(pLevel->pMisc, pLevel->nLevel);
	int* warp = DRLG_GetWarpLevels(pLevel->pMisc, pLevel->nLevel);
	D2LvlPrestTxt* pPrest = pMap->pLvlPrest;
	int i;
	DWORD dwPops;

	// Flag the areas off that we don't have a connection to, on the grid itself
	for (i = 0; i < MAX_LEVEL_CONNECTIONS; i++)
	{
		if (vis[i])
		{
			if (warp[i] != DRLGLINK_INVALID)
			{
				nFlags |= (16 * (i + 1));
			}
		}
	}
	DRLG_DoGridFlagsMath(pGrid, nFlags, D2MATH_OR);

	dwPops = pPrest->dwPops;
	if (!pPrest->dwScan && !dwPops)
	{
		return;
	}

	// build the pops data
	if (dwPops)
	{
		pMap->pPopsIndex = malloc(sizeof(DWORD) * dwPops);
		pMap->pPopsSubIndex = malloc(sizeof(DWORD) * dwPops);
		pMap->pPopsTicks = malloc(sizeof(DWORD) * dwPops);
		pMap->pPopsCoords = malloc(sizeof(DRLGCoordBox) * dwPops);

		memset(pMap->pPopsIndex, 0, sizeof(DWORD) * dwPops);
		memset(pMap->pPopsSubIndex, 0, sizeof(DWORD) * dwPops);
		memset(pMap->pPopsTicks, 0, sizeof(DWORD) * dwPops);
		memset(pMap->pPopsCoords, 0, sizeof(DRLGCoordBox) * dwPops);
	}

	// TODO: load DS1, load tiles, load all that jazz
}

/*
 *	Builds a grid room.
 *	Note that there is a separate outdoor version of this function.
 *	@author	eezstreet
 */
void DRLG_BuildPresetRooms(DRLGLevel* pLevel, DRLGMap* pMap, int nFlags, bool bMaze)
{
	int dt1Mask = pMap->pLvlPrest->dwDt1Mask;
	DRLGGrid grid;
	DWORD gridSectors[4096]{ 0 };
	DWORD gridFlags[4096]{ 0 };
	int nGridFlags;
	int nRoomFlags;
	int nYReal, nXReal;
	int nYGrid, nXGrid;

	// Create a grid with some slack space
	DRLG_CreateGrid_NoAllocations(&grid, 
		pMap->MapBox.nW / DRLG_GRID_SIZE + 1, pMap->MapBox.nH / DRLG_GRID_SIZE + 1, 
		gridFlags, gridSectors);

	if (pMap->pLvlPrest->dwOutdoors)
	{
		nFlags |= 0x80000; // ?
	}

	DRLG_BuildArea(pLevel, &grid, nFlags, pMap, bMaze);

	// If it's a maze level, all we need to do is insert the room
	if (bMaze)
	{
		nGridFlags = DRLG_GetGridFlags(&grid, 0, 0);
		DRLG_InsertPresetRoom(pLevel, pMap, &pMap->MapBox, dt1Mask, nGridFlags, 1, nullptr);
		return;
	}

	// Walk through the grid and insert each of the rooms
	nYGrid = 0;
	for (nYReal = pMap->MapBox.nY; nYReal < pMap->MapBox.nY + pMap->MapBox.nH; nYReal += DRLG_GRID_SIZE)
	{
		nXGrid = 0;
		for (nXReal = pMap->MapBox.nX; nXReal < pMap->MapBox.nX + pMap->MapBox.nW; nXReal += DRLG_GRID_SIZE)
		{
			DRLGCoordBox coords{ nXReal, nYReal, pMap->MapBox.nW - nXReal, pMap->MapBox.nH - nYReal };

			nGridFlags = 0;
			if (pMap->bHasInfo)
			{
				nGridFlags = DRLG_GetGridFlags(&pMap->MapMazeGrid, nXGrid, nYGrid);
			}
			nRoomFlags = DRLG_GetGridFlags(&grid, nXGrid, nYGrid);

			DRLG_InsertPresetRoom(pLevel, pMap, &coords, dt1Mask, nRoomFlags, 0, &grid);
			nXGrid++;
		}
		nYGrid++;
	}
}

/*
 *	Creates a new DRLGLevel, supplied with a DRLGMiscData and a level ID.
 *	This is the main point where a level gets generated!
 *	@author	eezstreet
 */
DRLGLevel* DRLG_CreateLevel(DRLGMisc* pMisc, int nLevel)
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
			break;
		case DRLGTYPE_PRESET:
			DRLG_PresetLevelAllocate(pLevel);
			break;
		case DRLGTYPE_OUTDOORS:
			DRLG_OutdoorLevelAllocate(pLevel);
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