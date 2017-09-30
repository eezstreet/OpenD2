#include "DRLG.hpp"

static DRLGAct sgServerActs[MAX_ACTS]{ 0 };
static DRLGAct sgClientAct{ 0 };

/*
 *	Find a LvlMaze.txt record, given a level id. Returns nullptr if it couldn't be found.
 *	Don't call this very often, because it's somewhat expensive.
 *	@author	eezstreet
 */
D2LvlMazeTxt* DRLG_GetMazeRecord(int nLevelID)
{
	D2LvlMazeTxt* pRecord;
	for (int i = 0; i < sgptDataTables->nLvlMazeTxtRecordCount; i++)
	{
		pRecord = &sgptDataTables->pLvlMazeTxt[i];
		if (pRecord->dwLevelId == nLevelID)
		{
			return pRecord;
		}
	}
	return nullptr;
}

/*
 *	Find a LvlPrest.txt record, given a level id. Returns nullptr if it couldn't be found.
 *	Don't call this very often, because it's somewhat expensive.
 *	@author	eezstreet
 */
D2LvlPrestTxt* DRLG_GetPrestRecord(int nLevelID)
{
	D2LvlPrestTxt* pRecord;
	for (int i = 0; i < sgptDataTables->nLvlPrestTxtRecordCount; i++)
	{
		pRecord = &sgptDataTables->pLvlPrestTxt[i];
		if (pRecord->dwLevelId == nLevelID)
		{
			return pRecord;
		}
	}
	return nullptr;
}

/*
 *	Find a DRLGLevel from a DRLGMisc with specified level ID
 *	@author	eezstreet
 */
DRLGLevel* DRLG_GetLevelFromID(DRLGMisc* pMisc, int nLevelID)
{
	// Search through the misc data until we find it
	DRLGLevel* pCurrent = pMisc->pFirst;
	while (pCurrent != nullptr && pCurrent->nLevel != nLevelID)
	{
		pCurrent = pCurrent->pNext;
	}

	if (pCurrent == nullptr)
	{
		// Not found, so we will need to generate it
		pCurrent = DRLG_GenerateLevel(pMisc, nLevelID);
		Log_ErrorAssert((pCurrent != nullptr), nullptr);
	}

	return pCurrent;
}

/*
 *	Gets a warp for a level
 *	@author	Necrolis
 */
int* DRLG_GetWarpLevels(DRLGMisc* pMisc, int nLevel)
{
	if (pMisc == nullptr || nLevel < 0)
	{
		return nullptr;
	}

	DRLGWarp* pWarp = pMisc->pWarp;
	while (pWarp != nullptr)
	{
		if (pWarp->nLevel == nLevel)
		{
			return pWarp->nWarp;
		}

		pWarp = pWarp->pNext;
	}

	return sgptDataTables->pLevelDefBin[nLevel].nWarp;
}

/*
 *	Gets a vis for a level
 *	@author	Necrolis
 */
int* DRLG_GetVisLevels(DRLGMisc* pMisc, int nLevel)
{
	if (pMisc == nullptr || nLevel < 0)
	{
		return nullptr;
	}

	DRLGWarp* pWarp = pMisc->pWarp;
	while (pWarp != nullptr)
	{
		if (pWarp->nLevel == nLevel)
		{
			return pWarp->nVis;
		}

		pWarp = pWarp->pNext;
	}

	return sgptDataTables->pLevelDefBin[nLevel].nVis;
}

/*
 *	Check if two boxes overlap
 *	@author	eezstreet
 */
bool DRLG_NotOverlapping(DRLGCoordBox* pA, DRLGCoordBox* pB, int nThreshold)
{
	int nXDiff;
	int nYDiff;

	if (pA->nX >= pB->nX)
	{
		nXDiff = pA->nX - pB->nW - pB->nX;
	}
	else
	{
		nXDiff = pB->nX - pA->nW - pA->nX;
	}

	if (pA->nY >= pB->nY)
	{
		nYDiff = pA->nY - pB->nH - pB->nY;
	}
	else
	{
		nYDiff = pB->nY - pA->nH - pA->nY;
	}

	return nXDiff >= nThreshold || nYDiff >= nThreshold;
}

/*
 *	Get the Act number associated with a level index
 *	Equivalent in function to D2Common.#10001.
 *	@author	eezstreet
 */
D2COMMONAPI BYTE DRLG_GetLevelActNum(int nLevel)
{
	D2LevelsTxt* pRecord = &sgptDataTables->pLevelsTxt[nLevel];
	return pRecord->nAct;
}

/*
 *	Creates MiscData for a DRLGAct
 *	@author	Necrolis
 */
static DRLGMisc* DRLG_CreateMiscData(DRLGAct* pAct, bool bServer, int nDifficulty, int nLevelStart)
{
	DRLGMisc* pMisc = (DRLGMisc*)malloc(sizeof(DRLGMisc));
	memset(pMisc, 0, sizeof(DRLGMisc));

	pMisc->nAct = pAct->nAct;
	pMisc->dwStartSeed = pMisc->MiscSeed.dwLoSeed = pAct->dwInitSeed;
	pMisc->fMiscFlags = bServer;
	pMisc->nDifficulty = nDifficulty;

	if (pMisc->nAct == D2_ACT_II)
	{
		// Act II has two special values we need to randomize:
		//	* Staff Tomb (tomb that leads to Duriel)
		//	* Boss Tomb (tomb that leads to Ancient Kaa the Soulless)
		// These two values cannot be the same thing (just reroll until two distinct values are found)
		pMisc->nStaffTombLvl = D2_smrand(&pMisc->MiscSeed, 7);
		do
		{
			pMisc->nBossTombLvl = D2_smrand(&pMisc->MiscSeed, 7);
		} while (pMisc->nBossTombLvl == pMisc->nStaffTombLvl);
	}
	else if (pMisc->nAct == D2_ACT_III)
	{
		// Act III has a special value that we need to randomize:
		//	* Whether or not to use Great Marsh as an interlink between Spider Forest and Flayer Jungle
		pMisc->bJungleInterlink = D2_sbrand(&pMisc->MiscSeed);
	}

	// Initialize the room sectors
	for (int i = 0; i < 4; i++)
	{
		pMisc->pRoomEx[i].eRoomStatus = i;
		pMisc->pRoomEx[i].pStatusNext = &pMisc->pRoomEx[i];
		pMisc->pRoomEx[i].pStatusPrev = &pMisc->pRoomEx[i];
	}

	// Generate the overworld (if one exists)
	DRLG_MiscGenerateOverworld(pMisc);

	// Create the starting level (if one exists)
	if (nLevelStart)
	{
		DRLG_GenerateLevel(pMisc, nLevelStart);
	}

	return pMisc;
}

/*
 *	Deletes MiscData for a DRLGAct
 *	@author	eezstreet
 */
static void DRLG_FreeMiscData(DRLGMisc* pMiscData)
{
	free(pMiscData);
}

/*
 *	Create EnvData for a DRLGAct
 *	@author	Necrolis
 */
static DRLGEnv* DRLG_CreateEnvData(DRLGAct* pAct)
{
	DRLGEnv* pEnv = (DRLGEnv*)malloc(sizeof(DRLGEnv));
	pEnv->ticks = pEnv->cputicks = trap->Milliseconds();
	return pEnv;
}

/*
 *	Frees EnvData for a DRLGAct
 *	@author	eezstreet
 */
static void DRLG_FreeEnvData(DRLGEnv* pEnv)
{
	free(pEnv);
}

/*
*	Frees an act
*	@author	eezstreet
*/
static void DRLG_FreeAct(DRLGAct* pAct)
{
	if (pAct->pMisc)
	{
		DRLG_FreeMiscData(pAct->pMisc);
		pAct->pMisc = nullptr;
	}
	if (pAct->pEnv)
	{
		DRLG_FreeEnvData(pAct->pEnv);
		pAct->pEnv = nullptr;
	}
}

/*
 *	Creates an act
 *	@author	Necrolis, eezstreet
 */
D2COMMONAPI void DRLG_CreateAct(int nAct, bool bServer, DWORD dwInitSeed, int nDifficulty, int nTown)
{
	DRLGAct* pAct;
	if (bServer)
	{
		pAct = &sgServerActs[nAct];
	}
	else
	{	// If we're on the client, we need to clean up the act and start over
		DRLG_FreeAct(&sgClientAct);
		memset(&sgClientAct, 0, sizeof(DRLGAct));
		pAct = &sgClientAct;
	}

	pAct->dwInitSeed = dwInitSeed;
	pAct->nAct = nAct;
	pAct->nTown = nTown;
	pAct->pMisc = DRLG_CreateMiscData(pAct, bServer, nDifficulty, nTown);
	pAct->pEnv = DRLG_CreateEnvData(pAct);
}

/*
 *	Shuts down the DRLG subsystem and frees up whatever memory isn't in use
 *	@author	eezstreet
 */
void DRLG_Shutdown()
{
	for (int i = 0; i < MAX_ACTS; i++)
	{
		DRLGAct* pAct = &sgServerActs[i];
		DRLG_FreeAct(pAct);
	}
	DRLG_FreeAct(&sgClientAct);
	DRLG_FreeLevels();
}