#include "D2Client.hpp"
#include "D2Menu_Trademark.hpp"

D2ModuleImportStrc* trap = nullptr;
D2GameConfigStrc* config = nullptr;
OpenD2ConfigStrc* openConfig = nullptr;
D2Client cl;

/*
 *	Initializes the client
 */
static void D2Client_InitializeClient(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	config = pConfig;
	openConfig = pOpenConfig;

	memset(&cl, 0, sizeof(D2Client));

	// Set first menu to be trademark menu
	cl.pActiveMenu = new D2Menu_Trademark();
}

/*
 *	Runs a single frame on the client.
 */
static void D2Client_RunClientFrame()
{
	if (cl.pActiveMenu != nullptr)
	{
		cl.pActiveMenu->Draw();
	}

	trap->R_Present();
}

/*
 *	This gets called every frame. We return the next module to run after this one.
 */
static OpenD2Modules D2Client_RunModuleFrame(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig)
{
	if (config == nullptr && openConfig == nullptr && pConfig != nullptr && pOpenConfig != nullptr)
	{
		// now is our chance! initialize!
		D2Client_InitializeClient(pConfig, pOpenConfig);
	}

	D2Client_RunClientFrame();

	if (cl.bLocalServer)
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
