#include "D2Client.hpp"
#include "D2Menu_Trademark.hpp"
#include "D2Menu_Main.hpp"

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

	cl.dwStartMS = trap->Milliseconds();

	// Set first menu to be trademark menu
	cl.gamestate = GS_TRADEMARK;
	cl.pActiveMenu = new D2Menu_Trademark();

	cl.fontFormal12 = trap->R_RegisterFont("fontformal12");
}

/*
 *	Pump input
 */
static void D2Client_HandleInput()
{
	trap->In_PumpEvents(openConfig);

	// All of the input data now lives in the OpenD2 config (yeah...) and we can iterate over it.
	for (DWORD i = 0; i < openConfig->dwNumPendingCommands; i++)
	{
		D2CommandQueue* pCmd = &openConfig->pCmds[i];
		
		// Handle all of the different event types
		switch (pCmd->cmdType)
		{
			case IN_MOUSEMOVE:
				cl.dwMouseX = pCmd->cmdData.motion.x;
				cl.dwMouseY = pCmd->cmdData.motion.y;
				break;
			case IN_MOUSEDOWN:
				if (pCmd->cmdData.button.buttonID == B_MOUSE1)
				{
					cl.bMouseDown = true;
				}
				break;
			case IN_MOUSEUP:
				if (pCmd->cmdData.button.buttonID == B_MOUSE1)
				{
					if (cl.bMouseDown)
					{
						cl.bMouseDown = false;
						cl.bMouseClicked = true;
					}
				}
				break;
			case IN_KEYDOWN:
			case IN_KEYUP:
				break;
			case IN_MOUSEWHEEL:
				break;
			case IN_QUIT:
				cl.bKillGame = true;
				break;
		}
	}

	if (cl.pActiveMenu != nullptr)
	{
		if (cl.bMouseClicked)
		{
			if (cl.pActiveMenu->HandleMouseClicked(cl.dwMouseX, cl.dwMouseY))
			{
				return;
			}
		}
		else if (cl.bMouseDown)
		{
			if (cl.pActiveMenu->HandleMouseDown(cl.dwMouseX, cl.dwMouseY))
			{
				return;
			}
		}
	}

	// handle worldspace clicking here
}


/*
 *	Runs a single frame on the client.
 */
static void D2Client_RunClientFrame()
{
	cl.dwMS = trap->Milliseconds();

	// Pipe in input events
	D2Client_HandleInput();

	// Trademark screen - if over 10 seconds have passed and we haven't clicked, advance to the main menu
	if (cl.gamestate == GS_TRADEMARK &&
		(cl.bMouseClicked || (cl.dwMS - cl.dwStartMS > 10000)))
	{
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menu_Main();
		cl.gamestate = GS_MAINMENU;
	}

	// Draw stuff
	if (cl.pActiveMenu != nullptr)
	{
		cl.pActiveMenu->Draw();
	}

	trap->R_DrawText(cl.fontFormal12, u"Diablo II 1.10f", 20, 560, 0, 0, ALIGN_LEFT, ALIGN_TOP);

	trap->R_Present();

	// Clear out data
	cl.bMouseClicked = false;
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

	if (cl.bKillGame)
	{
		return MODULE_NONE;
	}

	if (cl.bLocalServer)
	{	// If we're running a local server, we need to run that next (it will *always* run the client in the next step)
		return MODULE_SERVER;
	}

	return MODULE_CLIENT;
}

/*
 *	This gets called whenever the module is cleaned up.
 */
static void D2Client_Shutdown()
{
	if (cl.pActiveMenu != nullptr)
	{
		delete cl.pActiveMenu;
	}
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
		gExports.CleanupModule = D2Client_Shutdown;

		return &gExports;
	}
}
