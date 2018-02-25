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
static void JoinGamePanelSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget)
{
	D2Menu_TCPIP* pTCPIPMenu = (D2Menu_TCPIP*)cl.pActiveMenu;
	
	if (!D2Lib::stricmp(pCallingWidget->GetIdentifier(), "b_join"))
	{

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
	panelBackground = trap->R_RegisterDC6Texture(DC6_PANEL_BACKGROUND, "PopUpOkCancel2", 0, 1, PAL_UNITS);

	ipText = trap->TBL_FindStringFromIndex(TBLTEXT_DESC);

	// Create widgets
	m_okButton = new D2Widget_Button(155, 130, DC6_PANEL_BUTTON, "medium", 0, 0, 1, 1, 1, 1);
	m_cancelButton = new D2Widget_Button(15, 130, DC6_PANEL_BUTTON, "medium", 0, 0, 1, 1, 1, 1);
	m_ipEntry = new D2Widget_TextEntry(25, 70, true, true, false, true);

	m_okButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_OK));
	m_cancelButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_CANCEL));
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
	trap->R_DeregisterTexture("PopUpOkCancel2", panelBackground);
}

/*
 *	Draws the join panel
 *	@author	eezstreet
 */
void D2Panel_TCPIPJoin::Draw()
{
	// Draw the background
	DWORD dwWidth = 0, dwHeight = 0;
	trap->R_PollTexture(panelBackground, &dwWidth, &dwHeight);
	trap->R_DrawTexture(panelBackground, x, y, dwWidth, dwHeight, 0, 0);

	// Draw the text
	trap->R_DrawText(cl.font16, ipText, 45, 25, 175, 40, ALIGN_CENTER, ALIGN_TOP);

	// Draw the widgets
	DrawAllWidgets();
}