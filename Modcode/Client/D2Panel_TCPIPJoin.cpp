#include "D2Panel_TCPIPJoin.hpp"
#include "D2Menu_TCPIP.hpp"

#define DC6_PANEL_BACKGROUND	"data\\global\\ui\\FrontEnd\\PopUpOkCancel2.dc6"
#define DC6_PANEL_BUTTON		"data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6"

#define TBLTEXT_OK				5102
#define TBLTEXT_CANCEL			5103
#define TBLTEXT_DESC			5120

/*
 *	Function that gets called when a button is pressed on the panel
 *	@author	eezstreet
 */
extern void D2Client_AdvanceToCharSelect();
static void JoinGamePanelSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget)
{
	D2Menu_TCPIP* pTCPIPMenu = (D2Menu_TCPIP*)cl.pActiveMenu;
	D2Panel_TCPIPJoin* pTCPPanel = (D2Panel_TCPIPJoin*)pCallingPanel;

	if (!D2Lib::stricmp(pCallingWidget->GetIdentifier(), "b_join"))
	{	// copy the IP
		cl.bLocalServer = false;
		D2Lib::qwctomb(cl.szCurrentIPDestination, 32, pTCPPanel->GetEnteredIP());
		D2Client_AdvanceToCharSelect();
	}
	else if (!D2Lib::stricmp(pCallingWidget->GetIdentifier(), "b_cancel"))
	{
		pTCPIPMenu->ShowJoinSubmenu(false);
	}
}

/*
 *	Creates the join panel
 *	@author	eezstreet
 */
D2Panel_TCPIPJoin::D2Panel_TCPIPJoin() : D2Panel()
{
	// Create background
	panelBackground = engine->renderer->TextureFromStitchedDC6(DC6_PANEL_BACKGROUND, "PopUpOkCancel2", 0, 1, PAL_UNITS);

	ipText = engine->TBL_FindStringFromIndex(TBLTEXT_DESC);

	// Create widgets
	m_okButton = new D2Widget_Button(155, 130, DC6_PANEL_BUTTON, "medium", 0, 0, 1, 1, 1, 1);
	m_cancelButton = new D2Widget_Button(15, 130, DC6_PANEL_BUTTON, "medium", 0, 0, 1, 1, 1, 1);
	m_ipEntry = new D2Widget_TextEntry(25, 70, true, true, false, true);

	m_okButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_OK));
	m_cancelButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CANCEL));
	m_okButton->SetFont(cl.fontRidiculous);
	m_cancelButton->SetFont(cl.fontRidiculous);

	m_okButton->AttachClickSignal(JoinGamePanelSignal);
	m_cancelButton->AttachClickSignal(JoinGamePanelSignal);

	AddWidget(m_okButton);
	AddWidget(m_cancelButton);
	AddWidget(m_ipEntry);

	m_okButton->AttachIdentifier("b_join");
	m_cancelButton->AttachIdentifier("b_cancel");
}

/*
 *	Destroys the join panel
 *	@author	eezstreet
 */
D2Panel_TCPIPJoin::~D2Panel_TCPIPJoin()
{
	engine->renderer->DeregisterTexture("PopUpOkCancel2", panelBackground);
}

/*
 *	Draws the join panel
 *	@author	eezstreet
 */
void D2Panel_TCPIPJoin::Draw()
{
	// Draw the background
	DWORD dwWidth = 0, dwHeight = 0;
	engine->renderer->PollTexture(panelBackground, &dwWidth, &dwHeight);
	engine->renderer->DrawTexture(panelBackground, x, y, dwWidth, dwHeight, 0, 0);

	// Draw the text
	engine->renderer->DrawText(cl.font16, ipText, x + 45, y + 25, 175, 40, ALIGN_CENTER, ALIGN_TOP);

	// Draw the widgets
	DrawAllWidgets();
}

/*
 *	Get the IP that we typed in
 *	@author	eezstreet
 */
char16_t* D2Panel_TCPIPJoin::GetEnteredIP()
{
	return m_ipEntry->GetText();
}