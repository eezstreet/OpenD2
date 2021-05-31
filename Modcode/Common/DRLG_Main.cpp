#include "D2Common.hpp"
#include "DRLG.hpp"

D2COMMONAPI void D2Common_ConstructWorld()
{
	
}

D2COMMONAPI D2LvlPrestTxt* D2Common_GetLvlPrestForLevel(int nLevelId)
{
	for (int i = 0; i < sgptDataTables->nLvlPrestTxtRecordCount; i++)
	{
		D2LvlPrestTxt* prest = &sgptDataTables->pLvlPrestTxt[i];
		if (prest->dwLevelId == nLevelId)
		{
			return prest;
		}
	}
	return nullptr;
}

D2COMMONAPI void D2Common_ConstructPresetLevel(int nLevelId, int& seed, const char* fileName)
{
	// load DS1
	char path[MAX_D2PATH];
	snprintf(path, MAX_D2PATH, "data/global/tiles/%s", fileName);
	handle map_handle = engine->DS1_Load(path);
}

D2COMMONAPI void D2Common_ConstructSingleLevel(int nLevelId, int& seed)
{
	D2LevelDefBin* levelDef = &sgptDataTables->pLevelDefBin[nLevelId];
	D2LvlPrestTxt* prest = D2Common_GetLvlPrestForLevel(nLevelId);
	switch (levelDef->dwDrlgType)
	{
		case 1: // maze

			break;
		case 2: // preset
			// find levelprest.txt entry that matches this level,then randomly pick one of the files
			
			if (prest == nullptr)
			{
				return;
			}
			D2Common_ConstructPresetLevel(nLevelId, seed, prest->szFile[Seed_Range(seed, 0, prest->dwFiles)]);
			break;
		case 3: // outdoors
			break;
	}
}