#include "TCPIP.hpp"

#define TBLTEXT_YOURIP	5121

namespace D2Menus
{
	/*
	 *	Creates the TCP/IP Menu.
	 *	@author	eezstreet
	 */
	TCPIP::TCPIP() : D2Menu()
	{
#if 0
		backgroundTexture =
			engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\TCPIPscreen.dc6", "tcpip", 0, 11, PAL_UNITS);

		m_joinMenu = new D2Panel_TCPIPJoin();
		m_mainMenu = new D2Panel_TCPIPMain();

		m_joinMenu->x = 265;
		m_joinMenu->y = 160;

		AddPanel(m_mainMenu);
		AddPanel(m_joinMenu);

		HidePanel(m_joinMenu);

		m_yourIPString = engine->TBL_FindStringFromIndex(TBLTEXT_YOURIP);
		m_yourIP = engine->NET_GetLocalIP();
#endif
	}

	/*
	 *	Destroys the TCP/IP Menu
	 *	@author	eezstreet
	 */
	TCPIP::~TCPIP()
	{
		delete m_joinMenu;
		delete m_mainMenu;
	}

	/*
	 *	Draws the TCP/IP menu
	 *	@author	eezstreet
	 */
	void TCPIP::Draw()
	{
#if 0
		// Draw the background
		engine->renderer->DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

		// Draw the "Your IP Address is:" string
		engine->renderer->DrawText(cl.fontFormal12, m_yourIPString, 0, 110, 800, 15, ALIGN_CENTER, ALIGN_TOP);

		// Draw the IP string
		engine->renderer->DrawText(cl.fontFormal12, m_yourIP, 0, 130, 800, 15, ALIGN_CENTER, ALIGN_TOP);

		// Draw the panel
		DrawAllPanels();
#endif
	}

	/*
	 *	Show or hide the Join Game submenu
	 *	@author	eezstreet
	 */
	void TCPIP::ShowJoinSubmenu(bool bShow)
	{
		m_mainMenu->EnableButtons(!bShow);
		if (bShow)
		{
			ShowPanel(m_joinMenu);
		}
		else
		{
			HidePanel(m_joinMenu);
		}
	}
}