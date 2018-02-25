#include "D2Panel_TCPIPMain.hpp"
#include "D2Menu_OtherMultiplayer.hpp"
#include "D2Menu_TCPIP.hpp"

#define MAIN_BUTTON_DC6			"data\\global\\ui\\FrontEnd\\3WideButtonBlank.dc6"
#define SMALL_BUTTON_DC6		"data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6"

#define TBLTEXT_HOSTGAME		5118
#define TBLTEXT_JOINGAME		5119
#define TBLTEXT_CANCEL			3402

/*
 *	Creates the TCP/IP panel.
 *	@author	eezstreet
 */
D2Panel_TCPIPMain::D2Panel_TCPIPMain() : D2Panel()
{
	m_hostGameButton = new D2Widget_Button(265, 170, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
	m_joinGameButton = new D2Widget_Button(265, 230, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
	m_cancelButton = new D2Widget_Button(40, 535, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);

	m_hostGameButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_HOSTGAME));
	m_joinGameButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_JOINGAME));
	m_cancelButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_CANCEL));

	m_hostGameButton->AttachIdentifier("b_host");
	m_joinGameButton->AttachIdentifier("b_join");
	m_cancelButton->AttachIdentifier("b_cancel");

	m_hostGameButton->AttachClickSignal(PanelSignal);
	m_joinGameButton->AttachClickSignal(PanelSignal);
	m_cancelButton->AttachClickSignal(PanelSignal);

	AddWidget(m_hostGameButton);
	AddWidget(m_joinGameButton);
	AddWidget(m_cancelButton);
}

/*
 *	Destroys the TCP/IP panel.
 *	@author	eezstreet
 */
D2Panel_TCPIPMain::~D2Panel_TCPIPMain()
{
	delete m_hostGameButton;
	delete m_joinGameButton;
	delete m_cancelButton;
}

/*
 *	Draws the TCP/IP panel.
 *	@author	eezstreet
 */
void D2Panel_TCPIPMain::Draw()
{
	// Draw the big title at the top of the screen
	trap->R_DrawText(cl.font42, trap->TBL_FindStringFromIndex(5117), 0, 70, 800, 50, ALIGN_CENTER, ALIGN_TOP);

	// Draw all of the widgets
	DrawAllWidgets();
}

/*
 *	Enable/disable the Host Game and Join Game buttons (Join Game was clicked)
 *	@author	eezstreet
 */
void D2Panel_TCPIPMain::EnableButtons(bool bEnable)
{
	if (bEnable)
	{
		m_hostGameButton->Enable();
		m_joinGameButton->Enable();
	}
	else
	{
		m_hostGameButton->Disable();
		m_joinGameButton->Disable();
	}
}

/*
 *	The signal for when a button on the TCP/IP menu is clicked.
 *	@author	eezstreet
 */
void D2Panel_TCPIPMain::PanelSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget)
{
	if (!D2Lib::stricmp(pCallingWidget->GetIdentifier(), "b_host"))
	{
		trap->NET_SetPlayerCount(MAX_PLAYERS_REAL);	// we have no way of adjusting the max players from the menu
	}
	else if (!D2Lib::stricmp(pCallingWidget->GetIdentifier(), "b_join"))
	{
		D2Menu_TCPIP* pMenu = (D2Menu_TCPIP*)cl.pActiveMenu;
		pMenu->ShowJoinSubmenu(true);
	}
	else if (!D2Lib::stricmp(pCallingWidget->GetIdentifier(), "b_cancel"))
	{
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menu_OtherMultiplayer();
	}
}