#include "D2Panel_OtherMultiplayer.hpp"
#include "D2Menu_Main.hpp"
#include "D2Menu_TCPIP.hpp"

#define MAIN_BUTTON_DC6			"data\\global\\ui\\FrontEnd\\3WideButtonBlank.dc6"

#define TBLTEXT_OPENBNET		5115
#define TBLTEXT_TCPIP			5116
#define TBLTEXT_CANCEL			3402

/*
 *	Initializes the Other Multiplayer panel.
 *	@author	eezstreet
 */
D2Panel_OtherMultiplayer::D2Panel_OtherMultiplayer() : D2Panel()
{
	m_openBattleNetButton = new D2Widget_Button(265, 275, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
	m_TCPIPButton = new D2Widget_Button(265, 315, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
	m_cancelButton = new D2Widget_Button(265, 530, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);

	AddWidget(m_openBattleNetButton);
	AddWidget(m_TCPIPButton);
	AddWidget(m_cancelButton);

	// Disable Open battle.net as it's not allowed in OpenD2
	m_openBattleNetButton->Disable();

	// Attach text to the buttons
	m_openBattleNetButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_OPENBNET));
	m_TCPIPButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_TCPIP));
	m_cancelButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CANCEL));

	// Attach identifiers to literally everything on this menu
	m_TCPIPButton->AttachIdentifier("b_tcpip");
	m_cancelButton->AttachIdentifier("b_cancel");

	// Attach signals to buttons
	m_TCPIPButton->AttachClickSignal(PanelSignal);
	m_cancelButton->AttachClickSignal(PanelSignal);
}

/*
 *	Destroys the Other Multiplayer panel.
 *	@author	eezstreet
 */
D2Panel_OtherMultiplayer::~D2Panel_OtherMultiplayer()
{
	delete m_openBattleNetButton;
	delete m_TCPIPButton;
	delete m_cancelButton;
}

/*
 *	Draws the Other Multiplayer panel.
 *	@author	eezstreet
 */
void D2Panel_OtherMultiplayer::Draw()
{
	DrawAllWidgets();
}

/*
 *	The static function that is called on common button clicks
 *	@author	eezstreet
 */
void D2Panel_OtherMultiplayer::PanelSignal(D2Panel* pCallingPanel, D2Widget* pCallerWidget)
{
	if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_tcpip"))
	{
		delete cl.pActiveMenu;
		cl.charSelectContext = CSC_TCPIP;
		cl.pActiveMenu = new D2Menu_TCPIP();
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "b_cancel"))
	{
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menu_Main();
	}
}