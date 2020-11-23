#include "Main.hpp"
#include "../Menus/CharCreate.hpp"
#include "../Menus/CharSelect.hpp"
#include "../Menus/OtherMultiplayer.hpp"
#ifdef _DEBUG
#include "../Menus/Debug.hpp"
#endif

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

namespace D2Panels
{
	/*
	 *	Creates the main menu panel
	 */
	Main::Main() : D2Panel()
	{
		m_singleplayerButton = new D2Widgets::Button(265, 290, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
		m_battleNetButton = new D2Widgets::Button(265, 332, BATTLE_BUTTON_DC6, "wide02", 0, 1, 2, 3, 0, 1);
		m_gatewayButton = new D2Widgets::Button(265, 366, THIN_BUTTON_DC6, "narrow", 0, 1, 2, 3, 0, 1);
		m_multiplayerButton = new D2Widgets::Button(265, 400, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
		m_creditsButton = new D2Widgets::Button(265, 495, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
		m_cinematicsButton = new D2Widgets::Button(410, 495, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
		m_exitButton = new D2Widgets::Button(265, 535, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
#ifdef _DEBUG
		m_debugMapButton = new D2Widgets::Button(265, 248, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
#endif

		m_creditsButton->SetFont(cl.fontRidiculous);
		m_cinematicsButton->SetFont(cl.fontRidiculous);

		AddWidget(m_singleplayerButton);
		AddWidget(m_battleNetButton);
		AddWidget(m_gatewayButton);
		AddWidget(m_multiplayerButton);
		AddWidget(m_creditsButton);
		AddWidget(m_cinematicsButton);
		AddWidget(m_exitButton);
#ifdef _DEBUG
		AddWidget(m_debugMapButton);

		m_debugMapButton->AttachText(u"DEBUG MENU");
#endif

		m_singleplayerButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_SINGLEPLAYER));
		m_battleNetButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_BATTLENET));
		m_multiplayerButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_MULTIPLAYER));
		m_creditsButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CREDITS));
		m_cinematicsButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CINEMATICS));
		m_exitButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_EXIT));

		// Disable the battle.net button and the gateway button.
		// Closed Battle.net is not allowed in OpenD2.
		m_battleNetButton->Disable();
		m_gatewayButton->Disable();
	}

	/*
	 *	Destroys the main menu panel
	 */
	Main::~Main()
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
	void Main::Draw()
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
		char** szFileList = engine->FS_ListFilesInDirectory("Save", "*.d2s", &nNumFiles);

		delete cl.pActiveMenu;
		if (nNumFiles <= 0)
		{
			cl.pActiveMenu = new D2Menus::CharCreate();
		}
		else
		{
			cl.pActiveMenu = new D2Menus::CharSelect(szFileList, nNumFiles);
			engine->FS_FreeFileList(szFileList, nNumFiles);
		}
	}
}