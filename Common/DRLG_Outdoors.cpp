#include "DRLG.hpp"

/*
 *	Creates (phase 1) an outdoor level
 *	@author	eezstreet
 */
void DRLG_OutdoorLevelAllocate(DRLGLevel* pLevel)
{
	// Create the DRLGOutdoorInfo structure
	pLevel->pOutdoors = (DRLGOutdoorInfo*)malloc(sizeof(DRLGOutdoorInfo));
	memset(pLevel->pOutdoors, 0, sizeof(DRLGOutdoorInfo));
}

/*
 *	Picks random subelements and applies them to a DRLGRoomEx
 *	@author	eezstreet
 */
static DWORD DRLG_MaskSubtypes(DRLGRoomEx* pRoomEx, int nSubtype, int nSubTheme)
{
	DWORD dwResult = 0;
	D2LvlSubTxt* pStart;
	int nLvlSubMax = sgptDataTables->nLvlSubTxtRecordCount;
	int nIterator = 0;
	int nMiniIterator = 0;

	if (nSubtype == -1 || nSubTheme == -1)
	{
		return dwResult;
	}

	// Loop through LvlSub.bin until we can find the start of the subtypes
	pStart = sgptDataTables->pLvlSubTxt;
	while (pStart && nIterator < nLvlSubMax && pStart->dwType != nSubtype)
	{
		pStart++;
		nIterator++;
	}

	// OK cool we found it. Keep iterating through the file, applying dt1mask and picked types until we run out of types
	while (pStart && nIterator < nLvlSubMax && pStart->dwType == nSubtype)
	{
		if (D2Lib::smrand(&pRoomEx->RoomExSeed, 100) < pStart->dwProb[nSubTheme])
		{
			dwResult |= (1 << nMiniIterator);
			pRoomEx->dwDT1Mask |= pStart->dwDt1Mask;
		}
		pStart++;
		nIterator++;
		nMiniIterator++;
	}

	return dwResult;	// this will be a mask of all the subtypes we picked
}

/*
 *	Inserts a room, aligned along the outdoor level grid.
 *	@author	eezstreet
 */
void DRLG_InsertOutdoorGridRoom(DRLGLevel* pLevel, int nX, int nY, int nW, int nH, int nRoomFlags, 
	int nOutdoorMapFlags, int nUnused, int dt1Mask)
{
	DRLGRoomEx* pRoom = DRLG_AllocateRoomEx(pLevel, ROOMTYPE_OUT);
	D2LevelDefBin* pLevelDef;

	// Set basic room data
	pRoom->RoomExBox.nX = nX;
	pRoom->RoomExBox.nY = nY;
	pRoom->RoomExBox.nW = nW;
	pRoom->RoomExBox.nH = nH;
	pRoom->dwDT1Mask = dt1Mask;
	pRoom->fRoomExFlags |= ROOMEXFLAG_NO_LOS_DRAW; // don't use line of sight in outdoor levels
	pRoom->fRoomExFlags |= nRoomFlags;
	pRoom->pOutdoor->fOutMapFlags = nOutdoorMapFlags;
	pRoom->pOutdoor->fOutUnusedFlags = nUnused;

	// Apply subtheme, subtype, etc
	pLevelDef = &sgptDataTables->pLevelDefBin[pLevel->nLevel];
	pRoom->pOutdoor->nSubtypeNo = pLevelDef->dwSubType;
	pRoom->pOutdoor->nSubthemeNo = pLevelDef->dwSubTheme;
	pRoom->pOutdoor->dwSubRowsPicked = DRLG_MaskSubtypes(pRoom, pRoom->pOutdoor->nSubtypeNo, pRoom->pOutdoor->nSubthemeNo);
}

/*
 *	Creates a blank DRLGVer with a direction assigned to it.
 *	@author	eezstreet
 */
DRLGVer* DRLG_BlankDRLGVer(char nDirection)
{
	DRLGVer* pOut = (DRLGVer*)malloc(sizeof(DRLGVer));
	memset(pOut, 0, sizeof(DRLGVer));
	pOut->eDir = nDirection;
	return pOut;
}

/*
 *	Creates the ver data for outdoor levels. It's perfectly acceptable to call this with a null pLink.
 *	@author	eezstreet
 */
void DRLG_CreateOutdoorVerData(DRLGVer** ppVer, DRLGCoordBox* pCoords, char nDirection, DRLGLinkData* pLink)
{
	DRLGLinkData* pCurrent = pLink;
	DRLGVer* pPrevVer = nullptr;
	DRLGVer* pCurrentVer;
	DRLGCoordBox* pAdjustBox;
	DRLGVer* pSouthWest, * pSouthEast, * pNorthWest, * pNorthEast;
	int i;

	// Strip a bit off the sides (temporary, just for this calculation)
	while (pCurrent != nullptr)
	{
		pCurrent->pCoordBox->nW--;
		pCurrent->pCoordBox->nH--;
		pCurrent = pCurrent->pPrev;
	}

	// Create four DRLGVer structures, one for each direction
	for (i = 0; i < 4; i++)
	{
		pCurrentVer = (DRLGVer*)malloc(sizeof(DRLGVer));
		memset(pCurrentVer, 0, sizeof(DRLGVer));

		pCurrentVer->eDir = nDirection;
		
		// Set each one's position based on its cardinal direction (each iteration)
		switch (i)
		{
			case DIRECTION_SOUTHWEST:
				*ppVer = pCurrentVer;
				pCurrentVer->nX = pCoords->nX;
				pCurrentVer->nY = pCoords->nY + pCoords->nH;
				pSouthWest = pCurrentVer;
				break;
			case DIRECTION_NORTHWEST:
				pCurrentVer->nX = pCoords->nX;
				pCurrentVer->nY = pCoords->nY;
				pNorthWest = pCurrentVer;
				break;
			case DIRECTION_SOUTHEAST:
				pCurrentVer->nX = pCoords->nX + pCoords->nW;
				pCurrentVer->nY = pCoords->nY;
				pSouthEast = pCurrentVer;
				break;
			case DIRECTION_NORTHEAST:
				pCurrentVer->nX = pCoords->nX + pCoords->nW;
				pCurrentVer->nY = pCoords->nY + pCoords->nH;
				pNorthEast = pCurrentVer;
				break;
		}

		pCurrentVer->pPrev = pPrevVer;
		if (i == DIRECTION_NORTHEAST)
		{
			(**ppVer).pPrev = pCurrentVer;
		}
		pPrevVer = pCurrentVer;
	}

	// apply it to the DRLGLinkData (if applicable)
	pCurrent = pLink;
	while (pCurrent)
	{
		pAdjustBox = pCurrent->pCoordBox;
		if (pAdjustBox == nullptr)
		{
			pAdjustBox = pCoords;
		}

		// BIG TODO HERE
		switch (pCurrent->eDir)
		{
			case DIRECTION_SOUTHWEST:
				break;
			case DIRECTION_NORTHWEST:
				break;
			case DIRECTION_SOUTHEAST:
				break;
			case DIRECTION_NORTHEAST:
				break;
		}
		pCurrent = pCurrent->pPrev;
	}

	// Add more to the width again
	pCurrent = pLink;
	while (pCurrent)
	{
		pCurrent->pCoordBox->nW++;
		pCurrent->pCoordBox->nH++;
		pCurrent = pCurrent->pPrev;
	}
}

/*
 *	Creates an outdoor level for Act 1
 *	@author	eezstreet
 */
static void DRLG_CreateAct1OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 2
*	@author	eezstreet
*/
static void DRLG_CreateAct2OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 3
*	@author	eezstreet
*/
static void DRLG_CreateAct3OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 4
*	@author	eezstreet
*/
static void DRLG_CreateAct4OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 5
*	@author	eezstreet
*/
static void DRLG_CreateAct5OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Generates (phase 2) an outdoor level
*	@author	eezstreet
*/
void DRLG_OutdoorLevelGenerate(DRLGLevel* pLevel)
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

				pPrev = pCurrent->pPrev;
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
					DRLG_BuildPresetRooms(pLevel, pMap, nRoomFlag, false);
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