#include "OtherMultiplayer.hpp"
#include "../Menus/Main.hpp"
#include "../Menus/TCPIP.hpp"
#include "../Widgets/Button.hpp"

#define MAIN_BUTTON_DC6			"data\\global\\ui\\FrontEnd\\3WideButtonBlank.dc6"

#define TBLTEXT_OPENBNET		5115
#define TBLTEXT_TCPIP			5116
#define TBLTEXT_CANCEL			3402

namespace D2Panels
{
	/*
	 *	Initializes the Other Multiplayer panel.
	 *	@author	eezstreet
	 */
	OtherMultiplayer::OtherMultiplayer() : D2Panel()
	{
		m_openBattleNetButton = new D2Widgets::Button(265, 275, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
		m_TCPIPButton = new D2Widgets::Button(265, 315, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
		m_cancelButton = new D2Widgets::Button(265, 530, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);

		AddWidget(m_openBattleNetButton);
		AddWidget(m_TCPIPButton);
		AddWidget(m_cancelButton);

		// Disable Open battle.net as it's not allowed in OpenD2
		m_openBattleNetButton->Disable();

		// Attach text to the buttons
		m_openBattleNetButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_OPENBNET));
		m_TCPIPButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_TCPIP));
		m_cancelButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CANCEL));
	}

	/*
	 *	Destroys the Other Multiplayer panel.
	 *	@author	eezstreet
	 */
	OtherMultiplayer::~OtherMultiplayer()
	{
		delete m_openBattleNetButton;
		delete m_TCPIPButton;
		delete m_cancelButton;
	}

	/*
	 *	Draws the Other Multiplayer panel.
	 *	@author	eezstreet
	 */
	void OtherMultiplayer::Draw()
	{
		DrawAllWidgets();
	}
}