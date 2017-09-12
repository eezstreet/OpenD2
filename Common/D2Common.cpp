#include "D2Common.hpp"

////////////////////////////////////////
//
//	Global Variables

static bool gbCommonInitialized = false;

D2ModuleImportStrc* trap = nullptr;
D2GameConfigStrc* gpConfig = nullptr;
OpenD2ConfigStrc* gpOpenConfig = nullptr;

////////////////////////////////////////
//
//	Functions

/*
 *	Initializes shared data in D2Common
 *	@author	eezstreet
 */
D2COMMONAPI void D2Common_Init(D2ModuleImportStrc* pTrap, D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	if (gbCommonInitialized)
	{
		return;	// already initialized the game
	}

	trap = pTrap;
	gpConfig = pConfig;
	gpOpenConfig = pOpenConfig;

	// Load the data tables
	DataTables_Init();

	gbCommonInitialized = true;
}

/*
 *	Frees shared data in D2Common
 *	@author	eezstreet
 */
D2COMMONAPI void D2Common_Shutdown()
{
	if (!gbCommonInitialized)
	{	// don't shut down unless we've initialized the library first
		return;
	}

	// Kill off any remaining memory in data tables
	DataTables_Free();
}