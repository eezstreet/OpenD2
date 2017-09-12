#include "D2Common.hpp"

D2COMMONAPI D2DataTablesStrc* sgptDataTables;

static D2DataTablesStrc gDataTables{ 0 };

////////////////////////////////////////
//
//	Functions

/*
 *	Loads a specific data table
 *	@author	eezstreet
 */
static int DataTables_Load(const char* szDataTableName, void** pDestinationData, 
	D2TxtLinkStrc** pDestinationLink, size_t dwRowSize)
{
	char szPath[MAX_D2PATH]{ 0 };
	size_t dwFileSize = 0;

	// Try and load the BIN file first
	if (!gpConfig->bTXT)
	{
		snprintf(szPath, MAX_D2PATH, "%s%s.bin", D2DATATABLES_DIR, szDataTableName);

		if (BIN_Read(szPath, pDestinationData, &dwFileSize))
		{	// found the BIN file
			// we don't need to fill out the rest of the linker, it's just for .txt files
			return dwFileSize / dwRowSize;
		}
	}
	

	// If that doesn't work (or we have -txt mode enabled) we need to compile the BIN itself from .txt
	snprintf(szPath, MAX_D2PATH, "%s%s.txt", D2DATATABLES_DIR, szDataTableName);
	return 0;
}

/*
 *	Load all of the data tables
 *	@author	eezstreet
 */
void DataTables_Init()
{
	sgptDataTables = &gDataTables;

	//////////////
	//
	//	Levels

	// levels.bin and leveldef.bin are special in that they are created from levels.txt
	// Therefore, they will share the same record count.
	// (and therefore, the count doesn't need to be stored in sgptDataTables)
	sgptDataTables->nLevelsTxtRecordCount = 
		DataTables_Load("levels", (void**)&sgptDataTables->pLevelsTxt, nullptr, sizeof(D2LevelsTxt));
	DataTables_Load("leveldefs", (void**)&sgptDataTables->pLevelDefBin, nullptr, sizeof(D2LevelDefBin));

	sgptDataTables->nLvlTypesTxtRecordCount =
		DataTables_Load("lvltypes", (void**)&sgptDataTables->pLvlTypesTxt, nullptr, sizeof(D2LvlTypesTxt));
	sgptDataTables->nLvlSubTxtRecordCount =
		DataTables_Load("lvlsub", (void**)&sgptDataTables->pLvlSubTxt, nullptr, sizeof(D2LvlSubTxt));
	sgptDataTables->nLvlWarpTxtRecordCount =
		DataTables_Load("lvlwarp", (void**)&sgptDataTables->pLvlWarpTxt, nullptr, sizeof(D2LvlWarpTxt));
	sgptDataTables->nLvlMazeTxtRecordCount =
		DataTables_Load("lvlmaze", (void**)&sgptDataTables->pLvlMazeTxt, nullptr, sizeof(D2LvlMazeTxt));
	sgptDataTables->nLvlPrestTxtRecordCount =
		DataTables_Load("lvlprest", (void**)&sgptDataTables->pLvlPrestTxt, nullptr, sizeof(D2LvlPrestTxt));
}

/*
 *	Delete all of the data tables
 *	@author	eezstreet
 */
void DataTables_Free()
{

}

