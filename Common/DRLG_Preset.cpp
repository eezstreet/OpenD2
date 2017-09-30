#include "DRLG.hpp"

/*
 *	Creates a preset room
 *	@author	eezstreet
 */
void DRLG_InsertPresetRoom(DRLGLevel* pLevel, DRLGMap* pMap, DRLGCoordBox* pCoords, 
	int dt1Mask, int nRoomFlags, int nPresetFlags, DRLGGrid* pGrid)
{
	DRLGRoomEx* pRoom = DRLG_AllocateRoomEx(pLevel, ROOMTYPE_MAZE_PRESET);

	pRoom->dwDT1Mask = dt1Mask;
	pRoom->fRoomExFlags |= nRoomFlags;
	memcpy(&pRoom->RoomExBox, pCoords, sizeof(DRLGCoordBox));
	pRoom->pPreset->pMap = pMap;
	pRoom->pPreset->nLevelPrestId = pMap->pLvlPrest->dwDef;
	pRoom->pPreset->fFlags = nPresetFlags;
	pRoom->pPreset->pMapMazeGrid = pGrid;

	if (pGrid)
	{
		// Add 1 to the height (?)
		pGrid->nHeight |= 1;
	}

	// If the lvlPrest is not populated, neither is the room!
	if (!pMap->pLvlPrest->dwPopulate)
	{
		pRoom->fRoomExFlags |= ROOMEXFLAG_POPULATION_ZERO;
	}

	DRLG_BindRoomEx(pLevel, pRoom);
}

/*
 *	Creates a DRLGMap associated with a DRLGLevel
 *	@author	eezstreet
 */
DRLGMap* DRLG_CreateDRLGMap(DRLGLevel* pLevel, DRLGCoordBox* pCoords)
{
	DRLGMap* pMap = (DRLGMap*)malloc(sizeof(DRLGMap*));
	D2LvlPrestTxt* pLvlPrest = DRLG_GetPrestRecord(pLevel->nLevel);

	AssertWithDescription((pLvlPrest != nullptr), "Level labeled as Preset, but no preset claims the level", nullptr);

	memset(pMap, 0, sizeof(DRLGMap));
	
	// Link up the LvlPrest.txt record to this DRLGMap
	pMap->pLvlPrest = pLvlPrest;
	pMap->nLevelPrestNo = pLvlPrest - sgptDataTables->pLvlPrestTxt;

	// Select the file used
	if (pLevel->pPreset->nFile == -1)
	{
		pMap->nFilePicked = D2_smrand(&pLevel->LevelSeed, pLvlPrest->dwFiles);
	}
	else
	{
		pMap->nFilePicked = pLevel->pPreset->nFile;
	}

	// Place it at the coordinates
	pMap->MapBox.nX = pCoords->nX;
	pMap->MapBox.nY = pCoords->nY;

	// If the LvlPrest.txt record is non-zero, use that as the width. Otherwise, use the supplied width.
	if (pLvlPrest->dwSizeX && pLvlPrest->dwSizeY)
	{
		pMap->MapBox.nW = pLvlPrest->dwSizeX;
		pMap->MapBox.nH = pLvlPrest->dwSizeY;
	}
	else
	{
		pMap->MapBox.nW = pCoords->nW;
		pMap->MapBox.nH = pCoords->nH;
	}

	pMap->bInit = true;	// We're initialized now!

	// Link the DRLGMap to the DRLGLevel
	pMap->pNext = pLevel->pCurrent;
	pLevel->pCurrent = pMap;

	return pMap;
}