#include "TCPIPJoin.hpp"
#include "../Menus/TCPIP.hpp"
#include "../Widgets/Button.hpp"
#include "../Widgets/TextEntry.hpp"

#define DC6_PANEL_BACKGROUND	"data\\global\\ui\\FrontEnd\\PopUpOkCancel2.dc6"
#define DC6_PANEL_BUTTON		"data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6"

#define TBLTEXT_OK				5102
#define TBLTEXT_CANCEL			5103
#define TBLTEXT_DESC			5120

namespace D2Panels
{
	/*
	 *	Creates the join panel
	 *	@author	eezstreet
	 */
	TCPIPJoin::TCPIPJoin() : D2Panel()
	{
#if 0
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
#endif
	}

	/*
	 *	Destroys the join panel
	 *	@author	eezstreet
	 */
	TCPIPJoin::~TCPIPJoin()
	{
#if 0
		engine->renderer->DeregisterTexture("PopUpOkCancel2", panelBackground);
#endif
	}

	/*
	 *	Draws the join panel
	 *	@author	eezstreet
	 */
	void TCPIPJoin::Draw()
	{
#if 0
		// Draw the background
		DWORD dwWidth = 0, dwHeight = 0;
		engine->renderer->PollTexture(panelBackground, &dwWidth, &dwHeight);
		engine->renderer->DrawTexture(panelBackground, x, y, dwWidth, dwHeight, 0, 0);

		// Draw the text
		engine->renderer->DrawText(cl.font16, ipText, x + 45, y + 25, 175, 40, ALIGN_CENTER, ALIGN_TOP);

		// Draw the widgets
		DrawAllWidgets();
#endif
	}

	/*
	 *	Get the IP that we typed in
	 *	@author	eezstreet
	 */
	char16_t* TCPIPJoin::GetEnteredIP()
	{
		return m_ipEntry->GetText();
	}
}