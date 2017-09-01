#include "D2Client.hpp"

D2ModuleImportStrc* trap = nullptr;
D2GameConfigStrc* config = nullptr;
OpenD2ConfigStrc* openConfig = nullptr;
bool bLocalServer = false;

/*
 *	This gets called every frame. We return the next module to run after this one.
 */
static OpenD2Modules D2Client_RunModuleFrame(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	if (config == nullptr && openConfig == nullptr && pConfig != nullptr && pOpenConfig != nullptr)
	{
		// now is our chance! initialize!
	}

	if (bLocalServer)
	{	// If we're running a local server, we need to run that next (it will *always* run the client in the next step)
		return MODULE_SERVER;
	}

	return MODULE_CLIENT;
}

/*
 *	GetModuleAPI allows us to exchange a series of function pointers with the engine.
 */
static D2ModuleExportStrc gExports{ 0 };
extern "C"
{
	D2EXPORT D2ModuleExportStrc* GetModuleAPI(D2ModuleImportStrc* pImports)
	{
		if (pImports == nullptr)
		{
			return nullptr;
		}

		if (pImports->nApiVersion != D2CLIENTAPI_VERSION)
		{ // not the right API version
			return nullptr;
		}

		trap = pImports;

		gExports.nApiVersion = D2CLIENTAPI_VERSION;
		gExports.RunModuleFrame = D2Client_RunModuleFrame;

		return &gExports;
	}
}
