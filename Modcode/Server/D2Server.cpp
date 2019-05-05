#include "D2Server.hpp"

D2ModuleImportStrc* engine = nullptr;
D2GameConfigStrc* config = nullptr;
OpenD2ConfigStrc* openConfig = nullptr;

/*
 *	Called every frame on the server.
 *	@author	eezstreet
 */
static void D2Server_RunFrame()
{

}

/*
 *	Called when initializing the server for the first time.
 *	@author	eezstreet
 */
static void D2Server_InitializeServer(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	config = pConfig;
	openConfig = pOpenConfig;

	D2Common_Init(engine, pConfig, pOpenConfig);
}

/*
 *	This gets called whenever the module is cleaned up.
 */
static void D2Server_Shutdown()
{
}

/*
 *	This gets called every frame. We return the next module to run after this one.
 */
static OpenD2Modules D2Server_RunModuleFrame(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	if (config == nullptr && openConfig == nullptr && pConfig != nullptr && pOpenConfig != nullptr)
	{
		// now is our chance! initialize!
		D2Server_InitializeServer(pConfig, pOpenConfig);
	}

	D2Server_RunFrame();

	/*if (sv.bKillServer)
	{
		return MODULE_NONE;
	}

	if (sv.bDedicatedServer)
	{	// If we're running a dedicated server, we should only be running the server stuff.
		return MODULE_SERVER;
	}*/

	return MODULE_CLIENT;
}

/*
 *	GetModuleAPI allows us to exchange a series of function pointers with the engine.
 *	This is effectively the entry point for the library.
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

		if (pImports->nApiVersion != D2SERVERAPI_VERSION)
		{ // not the right API version
			return nullptr;
		}

		engine = pImports;

		gExports.nApiVersion = D2SERVERAPI_VERSION;
		gExports.RunModuleFrame = D2Server_RunModuleFrame;
		gExports.CleanupModule = D2Server_Shutdown;

		return &gExports;
	}
}