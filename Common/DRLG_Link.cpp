#include "DRLG.hpp"

///////////////////////////////////////////////////
//
//	General functions

/*
*	Gets the connection type based on the coordinates of two levels (assumed to be outdoors)
*	@author	eezstreet
*/
int DRLG_GetConnectionType(DRLGCoordBox* pA, DRLGCoordBox* pB)
{
	if (pA->nX <= pB->nX)
	{
		if (pB->nX == pA->nX + pA->nW)
		{
			return DIRECTION_SOUTHEAST;
		}
	}
	else if (pA->nX == pB->nX + pB->nW)
	{
		return DIRECTION_SOUTHWEST;
	}

	if (pA->nY <= pB->nY)
	{
		if (pB->nY == pA->nY + pA->nH)
		{
			return DIRECTION_NORTHEAST;
		}
	}
	else if (pA->nY == pB->nY + pB->nH)
	{
		return DIRECTION_NORTHWEST;
	}
	return DIRECTION_INVALID;
}

/*
*	Ensures that this is a valid connection.
*	@author Necrolis
*/
bool DRLG_CheckDataCoords(DRLGLinkData* pData, DRLGLinkData* pDataEx)
{
	DRLGCoordBox* pCoords;
	DRLGCoordBox* pCoordsEx;

	if (pData == NULL || pDataEx == NULL)
	{
		return false;
	}

	if (pData->eDir > pDataEx->eDir)
	{
		return true;
	}

	switch (pDataEx->eDir)
	{
	case DIRECTION_SOUTHWEST:
		pCoordsEx = pDataEx->pCoordBox;
		pCoords = pData->pCoordBox;
		if (pCoords->nY == pCoordsEx->nY)
		{
			return true;
		}
		break;

	case DIRECTION_NORTHWEST:
		pCoordsEx = pDataEx->pCoordBox;
		pCoords = pData->pCoordBox;
		if (pCoords->nX == pCoordsEx->nX)
		{
			return true;
		}
		break;

	case DIRECTION_SOUTHEAST:
		pCoordsEx = pDataEx->pCoordBox;
		pCoords = pData->pCoordBox;
		if (pCoords->nY == pCoordsEx->nY)
		{
			return true;
		}
		break;

	case DIRECTION_NORTHEAST:
		pCoordsEx = pDataEx->pCoordBox;
		pCoords = pDataEx->pCoordBox;
		if (pCoords->nX == pCoordsEx->nX)
		{
			return true;
		}
		break;
	}

	return false;
}

/*
*	@author	Necrolis
*/
void DRLG_CreateRoomData(DRLGLinkData** ppData, DRLGLevel* pLevel, int nDirection, bool bPreset)
{
	if (ppData == nullptr || pLevel == nullptr)
	{
		return;
	}

	DRLGLinkData* pData = (DRLGLinkData*)malloc(sizeof(DRLGLinkData));
	memset(pData, 0, sizeof(DRLGLinkData));
	pData->pLevel = pLevel;
	pData->eDir = nDirection;
	pData->bVerFlag = bPreset;
	pData->pCoordBox = &pLevel->LvlBox;

	DRLGLinkData* pPrev = *ppData;
	if (pPrev == nullptr)
	{
		*ppData = pData;
		return;
	}

	DRLGLinkData* pPrevEx = pPrev->pPrev;
	if (pPrevEx == nullptr)
	{
		if (DRLG_CheckDataCoords(pPrev, pData))
		{
			pData->pPrev = pPrevEx;
			pPrev->pPrev = pData;
			return;
		}

		pData->pPrev = *ppData;
		*ppData = pData;
	}

	while (pPrevEx != nullptr)
	{
		if (DRLG_CheckDataCoords(pPrevEx, pData))
		{
			pData->pPrev = pPrevEx;
			pPrev->pPrev = pData;
			return;
		}

		pPrevEx = pPrevEx->pPrev;
	}
}

/*
*	Create a warp for a level
*	@author	Necrolis
*/
DRLGWarp* DRLG_CreateWarp(DRLGMisc* pMisc, int nLevel)
{
	if (pMisc == NULL || nLevel == D2LEVEL_NULL)
		return NULL;

	DRLGWarp* pWarpLinks = pMisc->pWarp;
	D2LevelDefBin* pRecord;

	while (pWarpLinks != NULL)
	{
		if (pWarpLinks->nLevel == nLevel)
			return pWarpLinks;

		pWarpLinks = pWarpLinks->pNext;
	}

	DRLGWarp* pWarpLink = (DRLGWarp*)malloc(sizeof(DRLGWarp));
	pWarpLink->nLevel = nLevel;
	pRecord = &sgptDataTables->pLevelDefBin[nLevel];
	for (int i = 0; i < 8; i++)
	{
		pWarpLink->nVis[i] = pRecord->nVis[i];
		pWarpLink->nWarp[i] = pRecord->nWarp[i];
	}

	pWarpLink->pNext = pMisc->pWarp;
	pMisc->pWarp = pWarpLink;
	return pWarpLink;
}

/*
*	Link two levels together in their warp data
*	@author	Necrolis
*/
void DRLG_AddWarpLink(DRLGWarp* pWarp, int nLevel, int nWarp, int nWarpEx)
{
	if (pWarp == NULL)
		return;

	int* pWarps = pWarp->nWarp;
	int* pVis = pWarp->nVis;
	int i, j;

	for (i = 0; i < 8; i++)
	{
		if (pVis[i] == nLevel)
		{
			pWarps[i] = nWarp;
			return;
		}
	}

	nWarp = nWarpEx;
	if (nWarp == -1)
	{
		for (j = 0; j < 8; j++)
		{
			if (pWarps[i] != -1)
				break;

			if (pVis[i] == D2LEVEL_NULL && pWarps[i] == nWarp)
				nWarp = i;
		}
	}

	pVis[nWarpEx] = nLevel;
	pWarps[nWarpEx] = nWarpEx;
}

/*
*	Creates linkage between levels
*	@author	Necrolis
*/
static void DRLG_CreateLevelLinkage(DRLGMisc* pMisc, DRLGLink* pLink, DRLGSPACE pfSpaceCheck, DRLGLINKEREX pfLinkerEx)
{
	DRLGLevelLinkData linkData{ 0 };
	DRLGCoordBox* pCoord = &linkData.pLevelCoord[0];
	DRLGCoordBox* pCoordFrom = nullptr;
	DRLGLink* pCurrent = pLink;
	D2LevelDefBin* pLevelRecord;
	DRLGLevel* pLevel;
	int nIteration = 0;
	int nLevelLinked;
	int nLevelLinkedEx;

	// Set initial link data
	D2Lib::seedcopy(&linkData.pSeed, &pMisc->MiscSeed);
	memset(&linkData, 0, sizeof(linkData));
	linkData.pLink = pLink;

	for (int i = 0; i < 15; i++)
	{
		linkData.nRand[0][i] = -1;
		linkData.nRand[1][i] = -1;
		linkData.nRand[2][i] = -1;
		linkData.nRand[3][i] = -1;
	}

	// set size for each level in the chain
	while (pCurrent->nLevel != 0)
	{
		pLevelRecord = &sgptDataTables->pLevelDefBin[pCurrent->nLevel];
		pCoord->nW = pLevelRecord->dwSizeX[pMisc->nDifficulty];
		pCoord->nH = pLevelRecord->dwSizeY[pMisc->nDifficulty];
		pCoord++;
		pCurrent++;
	}

	// Run each linker on the batch chain.
	// Increment iteration count if the space checking function succeeded.
	pCurrent = pLink;
	while (pCurrent->nLevel != 0)
	{
		linkData.nCurrentLevel = pCurrent->nLevel;
		linkData.nIteration = nIteration;

		if (pCurrent->pfLinker(&linkData) && (pfSpaceCheck == nullptr || pfSpaceCheck(&linkData, nIteration)))
		{
			nIteration++;
			pCurrent++;
		}
		else
		{
			linkData.nRand[0][nIteration] = -1;
			linkData.nRand[1][nIteration] = -1;
			linkData.nRand[2][nIteration] = -1;
			linkData.nRand[3][nIteration] = -1;
			nIteration--;
			pCurrent--;
		}
	}

	pCurrent = pLink;
	nIteration = 0;

	// Now that we've linked all of the levels up, we need to make sure that the (outdoor) levels line up next to each other.
	while (pCurrent->nLevel != 0)
	{
		nLevelLinked = pCurrent->nLevelLink == -1 ? 0 : pCurrent->nLevelLink;
		nLevelLinkedEx = pCurrent->nLevelLinkEx == -1 ? 0 : pCurrent->nLevelLinkEx;
		pLevel = DRLG_GetLevelFromID(pMisc, pCurrent->nLevel);	// this will create the level if it doesn't exist

		pCoord = &pLevel->LvlBox;
		pCoordFrom = &linkData.pLevelCoord[nIteration];
		pCoord->nX = pCoordFrom->nX;
		pCoord->nY = pCoordFrom->nY;
		pCoord->nW = pCoordFrom->nW;
		pCoord->nH = pCoordFrom->nH;

		if (pLevel->eDRLGType == DRLGTYPE_PRESET)
		{	// If a town connects to this level randomly, set the preset file to match it
			if (pLevel->nLevel == D2LEVEL_ACT1_TOWN)
			{
				pLevel->pPreset->nFile = linkData.nRand[0][nIteration];
			}
			else if (pLevel->nLevel == D2LEVEL_ACT2_TOWN)
			{
				pLevel->pPreset->nFile = linkData.nRand[0][nIteration + 1];
			}
		}

		// Since the Tamoe Highland always connects to the Monastery Gate/Outer Cloister going from west to east,
		// the direction that the Barracks connects to the Outer Cloister therefore also depends upon the direction
		// that the Black Marsh connects to the Tamoe Highland.
		// If the Tamoe Highland connects to the Black Marsh along the SOUTH, the Outer Cloister must always connect to
		// the Barracks from the NORTH or the EAST. 
		// If the Tamoe Highland connects to the Black Marsh along the NORTH, the Outer Cloister must always connect to
		// the Barracks from the SOUTH or the EAST.
		// If the Tamoe Highland connects to the Black Marsh along the WEST, the Outer Cloister can spawn in any possible
		// direction - NORTH, EAST or SOUTH (but not west, obviously)
		// (This is a massive hack though, and I think Blizzard North knew this)
		if (pLevel->nLevel == D2LEVEL_ACT1_WILD5)
		{
			DRLGLevel* pCloisterLevel = DRLG_GetLevelFromID(pMisc, D2LEVEL_ACT1_COURTYARD1);
			switch (linkData.nRand[0][nIteration])
			{
			case DIRECTION_NORTHWEST:
				// Black Marsh links to Tamoe Highland from the WEST
				// Barracks can only link to Outer Cloister from EAST
				pCloisterLevel->pPreset->nFile = D2Lib::sbrand(&pMisc->MiscSeed) ? DIRECTION_SOUTHEAST : DIRECTION_NORTHEAST;
				break;
			case DIRECTION_NORTHEAST:
				// Black Marsh links to Tamoe Highland from the EAST
				// Barracks can only link to Outer Cloiser from WEST
				pCloisterLevel->pPreset->nFile = D2Lib::sbrand(&pMisc->MiscSeed) ? DIRECTION_SOUTHWEST : DIRECTION_NORTHWEST;
				break;
			}
		}

		// Run the linker post processing function (pfLinkerEx)
		if (pfLinkerEx != nullptr)
		{
			pfLinkerEx(pLevel, nIteration, &linkData.nRand[0][0]);
		}

		if (pMisc->nAct != D2_ACT_V)
		{
			if (nLevelLinked != D2LEVEL_NULL)
			{
				DRLGWarp* pWarp = DRLG_CreateWarp(pMisc, pLink->nLevel);
				DRLGWarp* pWarpEx = DRLG_CreateWarp(pMisc, nLevelLinked);
				DRLG_AddWarpLink(pWarp, pLink->nLevel, -1, -1);
				DRLG_AddWarpLink(pWarpEx, nLevelLinked, -1, -1);
			}

			if (nLevelLinkedEx != D2LEVEL_NULL)
			{
				DRLGWarp* pWarp = DRLG_CreateWarp(pMisc, pLink->nLevel);
				DRLGWarp* pWarpEx = DRLG_CreateWarp(pMisc, nLevelLinkedEx);
				DRLG_AddWarpLink(pWarp, pLink->nLevel, -1, -1);
				DRLG_AddWarpLink(pWarpEx, nLevelLinkedEx, -1, -1);
			}
		}

		nIteration++;
		pCurrent++;
	}
}

/*
*	Creates linkage between levels.
*	Only used on some acts.
*	@author	Necrolis
*/
static void DRLG_CreateLevelLinkageEx(DRLGMisc* pMisc, int nLevel, int nLevelEx)
{
	if (pMisc == nullptr || nLevel < 0 || nLevelEx < 0)
	{
		return;
	}

	while (nLevel <= nLevelEx)
	{
		DRLGLevel* pLevel = DRLG_GetLevelFromID(pMisc, nLevel);
		if (pLevel->eDRLGType == DRLGTYPE_OUTDOORS)
		{
			int* pWarps = DRLG_GetWarpLevels(pMisc, nLevel);
			int* pVis = DRLG_GetVisLevels(pMisc, nLevel);
			for (int i = 0; i < 8; i++)
			{
				if (pWarps != nullptr && pWarps[i] != D2LEVEL_NULL)
				{
					DRLGLevel* pLevelEx = DRLG_GetLevelFromID(pLevel->pMisc, nLevel);
					int nDirection = DRLG_GetConnectionType(&pLevel->LvlBox, &pLevelEx->LvlBox);
					DRLG_CreateRoomData(&pLevel->pOutdoors->pData, pLevelEx, nDirection, (pLevelEx->eDRLGType == DRLGTYPE_PRESET));
				}
			}
		}

		nLevel++;
	}
}

///////////////////////////////////////////////////
//
//	Linker table functions

/*
 *	Places a level at the given coordinates.
 *	@author	Necrolis
 */
void DRLG_PlaceCoords(DRLGCoordBox* pCoords, DRLGCoordBox* pCoordsEx, int nDirection, int nAdjust)
{
	if (pCoords == nullptr ||
		pCoordsEx == nullptr ||
		nDirection == DIRECTION_INVALID ||
		nDirection >= DIRECTION_COUNT)
	{
		return;
	}

	switch (nDirection)
	{
		case DIRECTION_SOUTHWEST:
			pCoordsEx->nX = pCoords->nX;
			pCoordsEx->nY = pCoords->nY + pCoords->nH;
			if (nAdjust == 1)
			{
				pCoordsEx->nX -= 16;
			}
			break;

		case DIRECTION_NORTHWEST:
			pCoordsEx->nX = pCoords->nX - pCoordsEx->nW;
			pCoordsEx->nY = pCoords->nY;
			if (nAdjust == 1)
			{
				pCoordsEx->nY -= 16;
			}
			else if (nAdjust == 2)
			{
				pCoordsEx->nY += 8;
			}
			break;

		case DIRECTION_SOUTHEAST:
			pCoordsEx->nX = pCoords->nX + pCoords->nW - pCoordsEx->nW;
			pCoordsEx->nY = pCoords->nY - pCoordsEx->nH;
			if (nAdjust == 1)
			{
				pCoordsEx->nX += 16;
			}
			break;

		case DIRECTION_NORTHEAST:
			pCoordsEx->nX = pCoords->nX + pCoords->nW;
			pCoordsEx->nY = pCoords->nY + pCoords->nH - pCoordsEx->nH;
			if (nAdjust == 1)
			{
				pCoordsEx->nY += 16;
			}
			else if (nAdjust == 2)
			{
				pCoordsEx->nY -= 8;
			}
			else if (nAdjust == 3)
			{
				pCoordsEx->nY += 8;
			}
			break;
	}
}

/*
 *	Places a level at the specified coordinates.
 *	An alternate of the first one.
 *	Need to find out what the difference is.
 *	@author	eezstreet
 */
void DRLG_PlaceCoords2(DRLGCoordBox* pCoords, DRLGCoordBox* pCoordsEx, int nDirection, int nAdjust)
{
	if (pCoords == nullptr ||
		pCoordsEx == nullptr ||
		nDirection == DIRECTION_INVALID ||
		nDirection >= DIRECTION_COUNT)
	{
		return;
	}

	switch (nDirection)
	{
		case DIRECTION_SOUTHWEST:
			pCoordsEx->nX = pCoords->nX + pCoords->nW - pCoordsEx->nW;
			pCoordsEx->nY = pCoords->nY + pCoords->nH;
			if (nAdjust == 1)
			{
				pCoordsEx->nX += 16;
			}
			break;
		case DIRECTION_NORTHWEST:
			pCoordsEx->nX = pCoords->nX - pCoords->nW;
			pCoordsEx->nY = pCoords->nY + pCoords->nH - pCoordsEx->nH;
			if (nAdjust == 1)
			{
				pCoords->nY += 16;
			}
			else if (nAdjust == 2)
			{
				pCoords->nY -= 8;
			}
			break;
		case DIRECTION_SOUTHEAST:
			pCoordsEx->nX = pCoords->nX;
			pCoordsEx->nY = pCoords->nY - pCoordsEx->nH;
			if (nAdjust == 1)
			{
				pCoordsEx->nX -= 16;
			}
			break;
		case DIRECTION_NORTHEAST:
			pCoordsEx->nX = pCoords->nX + pCoords->nW;
			pCoordsEx->nY = pCoords->nY;
			switch (nAdjust)
			{
				case 1:
					pCoordsEx->nY -= 16;
					break;
				case 2:
					pCoordsEx->nY += 8;
					break;
				case 3:
					pCoordsEx->nY -= 8;
					break;
			}
			break;
	}
}

/*
 *	Fixed link between two levels.
 *	@author	Necrolis
 */
bool DRLGLink_Fixed(DRLGLevelLinkData* pLinkData)
{
	int nIteration;
	D2LevelDefBin* pLevelDef;

	if (pLinkData == nullptr)
	{
		return false;
	}

	nIteration = pLinkData->nIteration;

	if (pLinkData->nRand[1][nIteration] == -1)
	{
		pLinkData->nRand[0][nIteration] = -1;
	}

	pLevelDef = &sgptDataTables->pLevelDefBin[pLinkData->nCurrentLevel];
	pLinkData->pLevelCoord[nIteration].nX = pLevelDef->dwOffsetX;
	pLinkData->pLevelCoord[nIteration].nY = pLevelDef->dwOffsetY;
	return true;
}

/*
 *	Floating link between two levels.
 *	@author	Necrolis
 */
bool DRLGLink_Floating(DRLGLevelLinkData* pLinkData)
{
	int nIteration;
	int nRand;
	int nLink;
	DRLGCoordBox* pCoords;
	DRLGCoordBox* pCoordsEx;

	if (pLinkData == nullptr)
	{
		return false;
	}

	nIteration = pLinkData->nIteration;
	if (pLinkData->nRand[1][nIteration] == -1)
	{	// Pick a random direction
		nRand = D2Lib::smrand(&pLinkData->pSeed, DIRECTION_COUNT);
		pLinkData->nRand[0][nIteration] = nRand;
		pLinkData->nRand[1][nIteration] = nRand;
	}
	else
	{	// Pick the same direction that was already established
		nRand = pLinkData->nRand[1][nIteration] % DIRECTION_COUNT;
		if (nRand == pLinkData->nRand[1][nIteration])
		{
			return false;
		}

		pLinkData->nRand[0][nIteration] = nRand;
	}

	nLink = pLinkData->pLink[nIteration].nLevelLink;
	pCoords = &pLinkData->pLevelCoord[nIteration];
	pCoordsEx = &pLinkData->pLevelCoord[nLink];
	DRLG_PlaceCoords(pCoordsEx, pCoords, pLinkData->nRand[0][nIteration], 1);
	return true;
}

/*
 *	Linker: Link to Act 1 town
 *	@author	eezstreet
 */
bool DRLGLink_LinkToAct1Town(DRLGLevelLinkData* pLinkData)
{
	int nIteration = pLinkData->nIteration;
	DRLGCoordBox* pCoords;
	DRLGCoordBox* pCoordsEx;
	int nNewDir;
	int nRand;
	int nLink;
	
	if (pLinkData->nRand[1][nIteration] == -1)
	{
		nNewDir = D2Lib::srand(&pLinkData->pSeed);
		pLinkData->nRand[1][nIteration] = nNewDir & 3;
		pLinkData->nRand[0][nIteration] = pLinkData->nRand[1][nIteration];
		nRand = D2Lib::srand(&pLinkData->pSeed);
		pLinkData->nRand[3][nIteration] = nRand & 1;
		nRand = pLinkData->nRand[3][nIteration];
	}
	else
	{
		nNewDir = pLinkData->nRand[2][nIteration] + pLinkData->nRand[0][nIteration];
		nNewDir %= DIRECTION_COUNT;
		nRand = pLinkData->nRand[2][nIteration] + 1;
		nRand %= 2;

		if (nNewDir == pLinkData->nRand[0][nIteration] && nRand == pLinkData->nRand[3][nIteration])
		{
			return false;
		}

		pLinkData->nRand[0][nIteration] = nNewDir;
	}

	pLinkData->nRand[2][nIteration] = nRand;

	nLink = pLinkData->pLink[nIteration].nLevelLink;
	pCoords = &pLinkData->pLevelCoord[nIteration];
	pCoordsEx = &pLinkData->pLevelCoord[nLink];

	if (pLinkData->nRand[1][nIteration] == 1)
	{	// What spawns in the northwest ?
		DRLG_PlaceCoords(pCoordsEx, pCoords, pLinkData->nRand[0][nIteration], 1);
	}
	else
	{
		DRLG_PlaceCoords2(pCoordsEx, pCoords, pLinkData->nRand[0][nIteration], 1);
	}
	return true;
}

/*
 *	Linker: link from Act 1 town
 *	@author	eezstreet
 */
bool DRLGLink_LinkFromAct1Town(DRLGLevelLinkData* pLinkData)
{
	int nIteration = pLinkData->nIteration;
	int nRand;
	int nLink;
	DRLGCoordBox* pCoords;
	DRLGCoordBox* pCoordsEx;

	if (pLinkData->nRand[1][nIteration] == -1)
	{
		nRand = D2Lib::srand(&pLinkData->pSeed);
		pLinkData->nRand[1][nIteration] = nRand & 3;
		pLinkData->nRand[0][nIteration] = pLinkData->nRand[1][nIteration];
		nRand = D2Lib::srand(&pLinkData->pSeed);
		pLinkData->nRand[3][nIteration] = nRand & 1;
		nLink = pLinkData->nRand[3][nIteration];
	}
	else
	{
		nRand = pLinkData->nRand[2][nIteration] + pLinkData->nRand[0][nIteration];
		nLink = nRand % 2;
		nRand %= DIRECTION_COUNT;
		if (nRand == pLinkData->nRand[1][nIteration] && nLink == pLinkData->nRand[3][nIteration])
		{
			return false;
		}
		pLinkData->nRand[0][nIteration] = nRand;
	}

	pLinkData->nRand[2][nIteration] = nLink;
	nLink = pLinkData->pLink[nIteration].nLevelLink;
	pCoords = &pLinkData->pLevelCoord[nIteration];
	pCoordsEx = &pLinkData->pLevelCoord[nLink];

	if (pLinkData->nRand[2][nIteration] == 1)
	{
		DRLG_PlaceCoords(pCoords, pCoordsEx, pLinkData->nRand[0][nIteration], 2);
	}
	else
	{
		DRLG_PlaceCoords2(pCoords, pCoordsEx, pLinkData->nRand[0][nIteration], 2);
	}

	return true;
}

/*
 *	Links two levels with zeroing (?)
 *	@author	eezstreet
 */
bool DRLGLink_Zero(DRLGLevelLinkData* pLinkData)
{
	int nIteration = pLinkData->nIteration;

	pLinkData->nRand[1][nIteration] = 0;
	pLinkData->nRand[0][nIteration] = 0;

	DRLG_PlaceCoords(&pLinkData->pLevelCoord[pLinkData->pLink[nIteration].nLevelLink],
		&pLinkData->pLevelCoord[nIteration],
		0,
		0);

	return true;
}

/*
 *	Linker tables for each of the acts
 *	TODO: finish this
 *	@author	eezstreet
 */
static DRLGLink Act1_LinkTable_Pass1[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT1_WILD3, -1, -1 },
	{ DRLGLink_Floating, D2LEVEL_ACT1_WILD2, 0, -1},
	{ DRLGLink_LinkToAct1Town, D2LEVEL_ACT1_WILD1, 1, -1 },
	{ DRLGLink_LinkFromAct1Town, D2LEVEL_ACT1_TOWN, 2, -1},
	{ DRLGLink_Floating, D2LEVEL_ACT1_GRAVEYARD, 1, -1},
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

static DRLGLink Act1_LinkTable_Pass2[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT1_MOOMOOFARM, -1, -1},
	{ DRLGLink_Fixed, D2LEVEL_ACT1_MONASTERY, -1, -1},
	{ DRLGLink_Zero, D2LEVEL_ACT1_WILD6, 1, -1 },
	{ DRLGLink_Floating, D2LEVEL_ACT1_WILD5, 2, -1},
	{ DRLGLink_Floating, D2LEVEL_ACT1_WILD4, 3, -1},
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

static DRLGLink Act2_LinkTable_Pass1[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT2_TOWN, -1, -1},
	// there is some more stuff here, but it's not known at present
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

static DRLGLink Act2_LinkTable_Pass2[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT2_VALLEY_OF_KINGS, -1, -1},
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

// Act 3 doesn't have a link table, it's handled specially

static DRLGLink Act4_LinkTable_Pass1[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT4_TOWN, -1, -1},
	{ /* */},
	{ DRLGLink_Floating, D2LEVEL_ACT4_MESA2, 1, -1},
	{ DRLGLink_Floating, D2LEVEL_ACT4_MESA3, 2, -1},
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

static DRLGLink Act4_LinkTable_Pass2[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT4_DIABLO1, -1, -1},
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

static DRLGLink Act5_LinkTable_Pass1[] =
{
	{ DRLGLink_Fixed, D2LEVEL_ACT5_TOWN, -1, -1},
	{ DRLGLink_Fixed, D2LEVEL_ACT5_SIEGE1, 0, -1},
	{ /* */ },
	{ /* */ },
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

static DRLGLink Act5_LinkTable_Pass2[] =
{
	{ /* */ },
	{ nullptr, D2LEVEL_NULL, -1, -1},
};

const bool DRLG_Act1_TownOverlap[] =
{
	true, true, false, false, false, false, false, false, false, true, false, false, false,
	false, false, false, false, false, false, true, false, true, false, false, true, false,
	false, false, false, false, true, true, false, true, false, false, false, false, false,
	true, false, true, false, false, false, false, false, false, false, false, false, false,
	false, true, true, false, true, false, false, false, false, false, true, true, false
};

/*
 *	Space-Checker for Act 1 (pass 1)
 *	@author	eezstreet
 */
static DWORD DRLG_Act1P1_SpaceChecker(DRLGLevelLinkData* pLinkData, int nIteration)
{
	int nLevel;
	int nLink = Act1_LinkTable_Pass1[nIteration].nLevelLink;
	int i = 0;

	// Check for levels overlapping
	for (i = 0; i < nIteration; i++)
	{
		if (nLink != i)
		{
			if (!DRLG_NotOverlapping(&pLinkData->pLevelCoord[nIteration], &pLinkData->pLevelCoord[i], 0))
			{
				return 0;
			}
		}
	}

	i = 0;
	nLevel = Act1_LinkTable_Pass1[nIteration].nLevel - 1;
	if (nLevel != 0)
	{
		if (nLevel == 16)
		{	// Graveyard level (16 + 1 = 17)
			DRLGLink* pCurrent = Act1_LinkTable_Pass1;
			int* pRand = &pLinkData->nRand[0][0];

			while (i == nIteration ||
				Act1_LinkTable_Pass1[i].nLevelLink != Act1_LinkTable_Pass1[nIteration].nLevelLink ||
				pLinkData->nRand[0][nIteration] == *pRand)
			{
				pRand++;
				i++;

				// NOTE: this checks the level link of pass one against the level link of PASS TWO
				if (Act1_LinkTable_Pass1[i].nLevelLink >= Act1_LinkTable_Pass2[nIteration].nLevelLink)
				{
					return false;
				}
			}
		}

		return true;
	}
	else
	{
		// funny logic here
		nLevel = (pLinkData->nRand[2][nLink] << 2) + pLinkData->nRand[0][nLink];
		nLevel += pLinkData->nRand[2][nIteration];
		nLevel *= 4;
		nLevel += pLinkData->nRand[0][nIteration];
		return DRLG_Act1_TownOverlap[nLevel];
	}
}

/*
 *	Space-Checker for Act 1 (pass 2)
 *	@author	eezstreet
 */
static DWORD DRLG_Act1P2_SpaceChecker(DRLGLevelLinkData* pLinkData, int nIteration)
{
	int i = 0;
	DRLGCoordBox* pCurrentBox;
	bool bSuccess = false;

	if (nIteration <= 0)
	{
		return true;
	}
	
	pCurrentBox = pLinkData->pLevelCoord;
	while (i == Act1_LinkTable_Pass2[nIteration].nLevelLink ||
		DRLG_NotOverlapping(&pLinkData->pLevelCoord[nIteration], pCurrentBox, 0))
	{
		i++;
		pCurrentBox++;
		if (i >= nIteration)
		{
			// ?? this seems bizarre as hell...
			pLinkData->pLevelCoord[0].nH += 200;
			pLinkData->pLevelCoord[0].nY -= 200;
			bSuccess = DRLG_NotOverlapping(pLinkData->pLevelCoord, &pLinkData->pLevelCoord[nIteration], 0);
			pLinkData->pLevelCoord[0].nH -= 200;
			pLinkData->pLevelCoord[0].nY += 200;
			return bSuccess;
		}
	}
	return false;
}

/*
 *	Space-Checker for Act 2 (pass 1)
 *	@author	eezstreet
 *	@todo write this!
 */
static DWORD DRLG_Act2P1_SpaceChecker(DRLGLevelLinkData* pLinkData, int nIteration)
{
	return 0;
}

/*
 *	Space-Checker for Act 2 (pass 2)
 *	@author	eezstreet
 *	@todo write this!
 */
static DWORD DRLG_Act2P2_SpaceChecker(DRLGLevelLinkData* pLinkData, int nIteration)
{
	return 0;
}

/*
 *	Space-Checker for Act 4 (pass 1)
 *	@author	eezstreet
 *	@todo write this!
 */
static DWORD DRLG_Act4P1_SpaceChecker(DRLGLevelLinkData* pLinkData, int nIteration)
{
	return 0;
}

/*
 *	Space-Checker for Act 4 (pass 2)
 *	@author	eezstreet
 *	@todo write this!
 */
static DWORD DRLG_Act4P2_SpaceChecker(DRLGLevelLinkData* pLinkData, int nIteration)
{
	return 0;
}

/*
 *	Linker function for Act 1 (both passes)
 *	@author	Necrolis
 */
const DRLGLinkerParams DRLG_Act1_LinkerParams[] =
{
	{ 0x00000000,0x00000002,0x00000003,0x00000001,0x00000000,0x00000004 },
	{ 0x00000000,0x00000002,0x00000003,0x00000002,0x00000003,0x00000004 },
	{ 0x00000000,0x00000003,0x00000011,0x00000002,0x00000001,0x00000008 },
	{ 0x00000000,0x00000003,0x00000011,0x00000003,0x00000000,0x00000008 },
	{ 0x00000000,0x00000003,0x00000011,0x00000001,0x00000001,0x00000010 },
	{ 0x00000000,0x00000003,0x00000011,0x00000003,0x00000003,0x00000010 },
	{ 0x00000002,0x00000000,0x00000000,0x00000000,0x00000000,0x00000008 },
	{ 0x00000002,0x00000000,0x00000000,0x00000002,0x00000002,0x00000008 },
	{ 0x00000002,0x00000000,0x00000000,0x00000003,0x00000000,0x00000008 },
	{ 0x00000002,0x00000000,0x00000000,0x00000003,0x00000002,0x00000008 },
	{ 0x00000002,0x00000000,0x00000000,0x00000000,0x00000001,0x00000400 },
	{ 0x00000002,0x00000000,0x00000000,0x00000001,0x00000001,0x00000400 },
	{ 0x00000002,0x00000000,0x00000000,0x00000002,0x00000001,0x00000200 },
	{ 0x00000002,0x00000000,0x00000000,0x00000002,0x00000002,0x00000080 },
	{ 0x00000002,0x00000000,0x00000000,0x00000003,0x00000002,0x00000100 }
};

const int DRLG_Act1_LinkerSize = sizeof(DRLG_Act1_LinkerParams) / sizeof(DRLGLinkerParams);

static void DRLG_Act1_Linker(DRLGLevel* pLevel, int nIteration, int* pRand)
{
	DRLGOutdoorInfo* pOutdoors;
	int nLevel;

	if (pLevel == nullptr || pRand == nullptr || pLevel->eDRLGType != DRLGTYPE_OUTDOORS)
	{
		return;
	}

	pOutdoors = pLevel->pOutdoors;
	nLevel = pLevel->nLevel;

	const DRLGLinkerParams* pStart = &DRLG_Act1_LinkerParams[0];
	const DRLGLinkerParams* pEnd = pStart + DRLG_Act1_LinkerSize;
	while (pStart < pEnd)
	{
		if (pStart->nLinkerLevels[0] != nLevel && pStart->nLinkerLevels[0] == D2LEVEL_NULL)
		{
			pStart++;
			continue;
		}

		if (pStart->nLinkerLevels[1] == nLevel || pStart->nLinkerLevels[2] == nLevel)
		{
			pStart++;
			continue;
		}

		if (pRand[nIteration] != pStart->nChance[0] || pRand[nIteration] != pStart->nChance[1])
		{
			pStart++;
			continue;
		}

		//pOutdoors->fOUTDOORFLAGS |= pStart->fFlags;	// FIXME
		pStart++;
	}
}

/*
 *	Links all of the overworld levels together
 *	@author	Necrolis
 */
void DRLG_MiscGenerateOverworld(DRLGMisc* pMisc)
{
	switch (pMisc->nAct)
	{
	case D2_ACT_I:
		DRLG_CreateLevelLinkage(pMisc, Act1_LinkTable_Pass1, DRLG_Act1P1_SpaceChecker, DRLG_Act1_Linker);
		DRLG_CreateLevelLinkage(pMisc, Act1_LinkTable_Pass2, DRLG_Act1P2_SpaceChecker, DRLG_Act1_Linker);
		DRLG_CreateLevelLinkageEx(pMisc, D2LEVEL_ACT1_TOWN, D2LEVEL_ACT1_GRAVEYARD);
		break;
	case D2_ACT_II:
		//DRLG_CreateLevelLinkage(pMisc, Act2_LinkTable_Pass1, DRLG_Act2P1_SpaceChecker, nullptr);
		//DRLG_CreateLevelLinkage(pMisc, Act2_LinkTable_Pass2, DRLG_Act2P2_SpaceChecker, nullptr);
		DRLG_CreateLevelLinkageEx(pMisc, D2LEVEL_ACT2_TOWN, D2LEVEL_ACT2_VALLEY_OF_KINGS);
		break;
	case D2_ACT_III:
		//DRLG_CreateJungleLinkage(pMisc, D2LEVEL_ACT3_TOWN, D2LEVEL_ACT3_TRAVINCAL);
		DRLG_CreateLevelLinkageEx(pMisc, D2LEVEL_ACT3_TOWN, D2LEVEL_ACT3_TRAVINCAL);
		break;
	case D2_ACT_IV:
		//DRLG_CreateLevelLinkage(pMisc, Act4_LinkTable_Pass1, DRLG_Act4P1_SpaceChecker, nullptr);
		//DRLG_CreateLevelLinkage(pMisc, Act4_LinkTable_Pass2, DRLG_Act4P2_SpaceChecker, nullptr);
		DRLG_CreateLevelLinkageEx(pMisc, D2LEVEL_ACT4_TOWN, D2LEVEL_ACT4_MESA3);
		break;
	case D2_ACT_V:
		// Act 5's linkage is...bizarre, to say the least.
		// It looks like some loops will need to be unrolled in order to make proper sense of what's going on.
		DRLG_CreateLevelLinkageEx(pMisc, D2LEVEL_ACT5_BARRICADE1, D2LEVEL_ACT5_BARRICADE2);
		break;
	}
}