#include "D2Panel_Main.hpp"
#include "D2Menu_CharCreate.hpp"
#include "D2Menu_CharSelect.hpp"
#include "D2Menu_OtherMultiplayer.hpp"

#define TBLTEXT_SINGLEPLAYER	5106
#define TBLTEXT_BATTLENET		5107
#define TBLTEXT_MULTIPLAYER		5108
#define TBLTEXT_EXIT			5109
#define TBLTEXT_CREDITS			5110
#define TBLTEXT_CINEMATICS		5111

#define MAIN_BUTTON_DC6			"data\\global\\ui\\FrontEnd\\3WideButtonBlank.dc6"
#define BATTLE_BUTTON_DC6		"data\\global\\ui\\FrontEnd\\WideButtonBlank02.dc6"
#define SMALL_BUTTON_DC6		"data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6"
#define THIN_BUTTON_DC6			"data\\global\\ui\\FrontEnd\\NarrowButtonBlank.dc6"

/*
 *	Creates the main menu panel
 */
D2Panel_Main::D2Panel_Main() : D2Panel()
{
	m_singleplayerButton = new D2Widget_Button(265, 290, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
	m_battleNetButton = new D2Widget_Button(265, 332, BATTLE_BUTTON_DC6, "wide02", 0, 1, 2, 3, 0, 1);
	m_gatewayButton = new D2Widget_Button(265, 366, THIN_BUTTON_DC6, "narrow", 0, 1, 2, 3, 0, 1);
	m_multiplayerButton = new D2Widget_Button(265, 400, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
	m_creditsButton = new D2Widget_Button(265, 495, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
	m_cinematicsButton = new D2Widget_Button(410, 495, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
	m_exitButton = new D2Widget_Button(265, 535, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);

	m_creditsButton->SetFont(cl.fontRidiculous);
	m_cinematicsButton->SetFont(cl.fontRidiculous);

	AddWidget(m_singleplayerButton);
	AddWidget(m_battleNetButton);
	AddWidget(m_gatewayButton);
	AddWidget(m_multiplayerButton);
	AddWidget(m_creditsButton);
	AddWidget(m_cinematicsButton);
	AddWidget(m_exitButton);

	m_singleplayerButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_SINGLEPLAYER));
	m_battleNetButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_BATTLENET));
	m_multiplayerButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_MULTIPLAYER));
	m_creditsButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_CREDITS));
	m_cinematicsButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_CINEMATICS));
	m_exitButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_EXIT));

	// Disable the battle.net button and the gateway button.
	// Closed Battle.net is not allowed in OpenD2.
	m_battleNetButton->Disable();
	m_gatewayButton->Disable();

	// Attach identifiers to literally everything on this menu
	m_singleplayerButton->AttachIdentifier("b_sp");
	m_multiplayerButton->AttachIdentifier("b_mul");
	m_creditsButton->AttachIdentifier("b_cred");
	m_cinematicsButton->AttachIdentifier("b_cin");
	m_exitButton->AttachIdentifier("b_exit");

	// Attach signals to buttons
	m_singleplayerButton->AttachClickSignal(PanelSignal);
	m_multiplayerButton->AttachClickSignal(PanelSignal);
	m_creditsButton->AttachClickSignal(PanelSignal);
	m_cinematicsButton->AttachClickSignal(PanelSignal);
	m_exitButton->AttachClickSignal(PanelSignal);
}

/*
 *	Destroys the main menu panel
 */
D2Panel_Main::~D2Panel_Main()
{
	delete m_singleplayerButton;
	delete m_battleNetButton;
	delete m_gatewayButton;
	delete m_multiplayerButton;
	delete m_creditsButton;
	delete m_cinematicsButton;
	delete m_exitButton;
}

/*
 *	Draws the main menu panel
 */
void D2Panel_Main::Draw()
{
	DrawAllWidgets();
}

/*
 *	Tries to advance to the character select screen.
 *	If there's no save files present, it advances to the character creation screen instead.
 */
void D2Client_AdvanceToCharSelect()
{
	int nNumFiles = 0;
	char** szFileList = trap->FS_ListFilesInDirectory("Save", "*.d2s", &nNumFiles);

	delete cl.pActiveMenu;
	if (nNumFiles <= 0)
	{
		cl.pActiveMenu = new D2Menu_CharCreate();
	}
	else
	{
		cl.pActiveMenu = new D2Menu_CharSelect(szFileList, nNumFiles);
		trap->FS_FreeFileList(szFileList, nNumFiles);
	}
}

/*
 *	The main signal handler for this panel.
 *	Whenever a button is clicked on the main menu, this function gets called globally
 *	NOTE: static method
 */
void D2Panel_Main::PanelSignal(D2Panel* pCallerPanel, D2Widget* pCallerWidget)
{
	if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_sp"))
	{	// singleplayer button got clicked
		cl.szCurrentIPDestination[0] = '\0';	// set IP to blank (this means that it's a local game)
		trap->NET_SetPlayerCount(1);	// only one player can join
		cl.charSelectContext = CSC_SINGLEPLAYER;
		D2Client_AdvanceToCharSelect();
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_mul"))
	{	// multiplayer button got clicked
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menu_OtherMultiplayer();
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_cred"))
	{	// credits button got clicked
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_cin"))
	{	// cinematics button got clicked
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_exit"))
	{	// exit button got clicked
		cl.bKillGame = true;
		return;
	}
	else
	{	// ? not sure how else this got called
		trap->Warning(__FILE__, __LINE__,
			"D2Panel_Main::PanelSignal got called but the widget identifier was not understood.");
	}
}