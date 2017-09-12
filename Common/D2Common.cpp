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

	gbCommonInitialized = true;
}