#include "DRLG.hpp"

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
		if (D2_smrand(&pRoomEx->RoomExSeed, 100) < pStart->dwProb[nSubTheme])
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
		pPrevVer = pCurrentVer;
	}

	// apply it to the DRLGLinkData (if applicable)
	pCurrent = pLink;
	while (pCurrent)
	{
		int nNewX, nNewY, nNewW, nNewH;

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
void DRLG_CreateAct1OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 2
*	@author	eezstreet
*/
void DRLG_CreateAct2OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 3
*	@author	eezstreet
*/
void DRLG_CreateAct3OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 4
*	@author	eezstreet
*/
void DRLG_CreateAct4OutdoorsLevel(DRLGLevel* pLevel)
{

}

/*
*	Creates an outdoor level for Act 5
*	@author	eezstreet
*/
void DRLG_CreateAct5OutdoorsLevel(DRLGLevel* pLevel)
{

}