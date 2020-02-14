#include "D2Client.hpp"
#include "D2Menu_Trademark.hpp"
#include "D2Menu_Main.hpp"
#include "D2Menu_TCPIP.hpp"

D2ModuleImportStrc* engine = nullptr;
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

	cl.dwStartMS = engine->Milliseconds();

	// Set first menu to be trademark menu
	cl.gamestate = GS_TRADEMARK;
	cl.pActiveMenu = new D2Menu_Trademark();

	cl.font16 = engine->renderer->RegisterFont("font16");
	cl.font16 = engine->renderer->RegisterFont("font16");
	cl.font30 = engine->renderer->RegisterFont("font30");
	cl.font42 = engine->renderer->RegisterFont("font42");
	cl.fontFormal12 = engine->renderer->RegisterFont("fontformal12");
	cl.fontExocet10 = engine->renderer->RegisterFont("fontExocet10");
	cl.fontRidiculous = engine->renderer->RegisterFont("fontridiculous");
}

/*
 *	Go back to the "main" menu, whereever it should be.
 *	The Trademark menu always goes to the main menu, but after that we depend on the charSelectContext to guide us.
 *	@author	eezstreet
 */
void D2Client_GoToContextMenu()
{
	switch (cl.charSelectContext)
	{
		case CSC_SINGLEPLAYER:
			cl.pActiveMenu = new D2Menu_Main();
			break;
		case CSC_TCPIP:
			cl.pActiveMenu = new D2Menu_TCPIP();
			break;
	}
}

/*
 *	Set up local game server
 *	@author	eezstreet
 */
void D2Client_SetupServerConnection()
{
	if (cl.szCurrentIPDestination[0] != '\0')
	{
		// If we have an IP, this means that we need to connect to that server. Not host one.
		if (!engine->NET_Connect(cl.szCurrentIPDestination, GAME_PORT))
		{
			// Error out!
			engine->Warning(__FILE__, __LINE__, "Failed to connect to server.");
			cl.gamestate = GS_MAINMENU;
			D2Client_GoToContextMenu();
		}
		cl.bLocalServer = false;
	}
	else
	{
		engine->NET_Listen(GAME_PORT);
		cl.bLocalServer = true;
	}
	
}

/*
 *	Handle window events
 */
static void D2Client_HandleWindowEvent(WindowEvent windowEvent)
{
	switch (windowEvent.event)
	{
		case D2WindowEventType::WINDOWEVENT_FOCUS_GAINED:
			engine->S_ResumeAudio();
			break;
		case D2WindowEventType::WINDOWEVENT_FOCUS_LOST:
			engine->S_PauseAudio();
			break;
	}
}

/*
 *	Pump input
 */
static void D2Client_HandleInput()
{
	engine->In_PumpEvents(openConfig);

	// All of the input data now lives in the OpenD2 config (yeah...) and we can iterate over it.
	for (DWORD i = 0; i < openConfig->dwNumPendingCommands; i++)
	{
		D2CommandQueue* pCmd = &openConfig->pCmds[i];
				// Handle all of the different event types
		switch (pCmd->cmdType)
		{
			case IN_WINDOW:
				D2Client_HandleWindowEvent(pCmd->cmdData.window);
				break;
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
				if (cl.pActiveMenu != nullptr)
				{
					cl.pActiveMenu->HandleKeyDown(pCmd->cmdData.button.buttonID);
				}
				break;

			case IN_KEYUP:
				// FIXME: handle binds also
				if (cl.pActiveMenu != nullptr)
				{
					cl.pActiveMenu->HandleKeyUp(pCmd->cmdData.button.buttonID);
				}
				break;

			case IN_TEXTEDITING:
				// only menus need to work with text editing
				if (cl.pActiveMenu != nullptr)
				{
					cl.pActiveMenu->HandleTextEditing(pCmd->cmdData.text.text, 
						pCmd->cmdData.text.start, pCmd->cmdData.text.length);
				}
				break;

			case IN_TEXTINPUT:
				if (cl.pActiveMenu != nullptr)
				{
					cl.pActiveMenu->HandleTextInput(pCmd->cmdData.text.text);
				}
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
 *	Load stuff between main menu and regular game
 *	@author	eezstreet
 */
static void D2Client_LoadData()
{
	if (cl.nLoadState == 0)
	{	// load D2Common
		D2Common_Init(engine, config, openConfig);
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 1)
	{	// load interface

		// Prime the server for init.
		if (cl.bLocalServer)
		{
			cl.bClientReadyForServer = true;
		}
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 2)
	{	// D2Game is loaded on this step if we are running a local game. The client need not do anything here.
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 3)
	{	// START HERE if in an inter-act loading.
		// Create the level
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 4)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 5)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 6)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 7)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 8)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 9)
	{	// go ingame
		cl.nLoadState = 0;
		cl.gamestate = GS_INGAME;
	}
}

/*
 *	Ping the server, telling it that we're still alive.
 *	@author	eezstreet
 */
static void D2Client_PingServer()
{
	DWORD dwTicks;
	D2Packet packet;

	if (cl.bLocalServer || !cl.bValidatedSave)
	{	// We don't need to do this in a local server, or if we haven't started the handshake yet.
		return;
	}

	dwTicks = engine->Milliseconds();

	if (dwTicks - cl.dwLastPingPacket >= 5000)
	{	// Send one packet every 5 seconds
		packet.nPacketType = D2CPACKET_PING;
		packet.packetData.Ping.dwTickCount = dwTicks;
		packet.packetData.Ping.dwUnknown = 0;
		engine->NET_SendClientPacket(&packet);
		cl.dwLastPingPacket = dwTicks;
	}
}

/*
 *	Runs a single frame on the client.
 */
static void D2Client_RunClientFrame()
{
	cl.dwMS = engine->Milliseconds();

	// Clear out menu signals
	if (cl.pActiveMenu != nullptr)
	{
		cl.pActiveMenu->RefreshInputFrame();
	}

	// Pipe in input events
	D2Client_HandleInput();

	// Handle menus
	if (cl.pActiveMenu != nullptr && cl.gamestate != GS_LOADING)
	{
		if (cl.pActiveMenu->WaitingSignal())
		{ // Process any waiting signals from the menus
			D2Menu::ProcessMenuSignals(cl.pActiveMenu);
		}
		if (cl.pActiveMenu != nullptr)
		{	// it can become null between now and then
			cl.pActiveMenu->Draw();
		}
	}
	else if (cl.pLoadingMenu != nullptr && cl.nLoadState != 0)
	{
		cl.pLoadingMenu->Draw();
	}

	engine->renderer->Present();

	// Load stuff, if we need to
	if (cl.gamestate == GS_LOADING)
	{
		D2Client_LoadData();
	}

	// Ping the server
	if (cl.gamestate == GS_LOADING || cl.gamestate == GS_INGAME)
	{
		D2Client_PingServer();
	}

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

	if (cl.bLocalServer && cl.bClientReadyForServer)
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
 *	This gets called whenever we receive a packet.
 *	@author	eezstreet
 */
static bool D2Client_HandlePacket(D2Packet* pPacket)
{
	switch (pPacket->nPacketType)
	{
		case D2SPACKET_COMPRESSIONINFO:
			ClientPacket::ProcessCompressionPacket(pPacket);
			break;
		case D2SPACKET_SAVESTATUS:
			ClientPacket::ProcessSavegameStatusPacket(pPacket);
			break;
		case D2SPACKET_GAMEFLAGS:
			ClientPacket::ProcessServerMetaPacket(pPacket);
			break;
		case D2SPACKET_PONG:
			ClientPacket::ProcessPongPacket(pPacket);
			break;
	}
	return true;
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

		engine = pImports;

		gExports.nApiVersion = D2CLIENTAPI_VERSION;
		gExports.RunModuleFrame = D2Client_RunModuleFrame;
		gExports.CleanupModule = D2Client_Shutdown;
		gExports.HandlePacket = D2Client_HandlePacket;

		return &gExports;
	}
}
