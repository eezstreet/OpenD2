#include "D2Menu_TCPIP.hpp"

#define TBLTEXT_YOURIP	5121

/*
 *	Creates the TCP/IP Menu.
 *	@author	eezstreet
 */
D2Menu_TCPIP::D2Menu_TCPIP() : D2Menu()
{
	backgroundTexture =
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\TCPIPscreen.dc6", "tcpip", 0, 11, PAL_UNITS);

	m_joinMenu = new D2Panel_TCPIPJoin();
	m_mainMenu = new D2Panel_TCPIPMain();

	AddPanel(m_joinMenu);
	AddPanel(m_mainMenu);

	HidePanel(m_joinMenu);

	trap->NET_GetLocalIP(m_ipStringBuffer, 32, GAME_PORT);
	m_yourIPString = trap->TBL_FindStringFromIndex(TBLTEXT_YOURIP);
}

/*
 *	Destroys the TCP/IP Menu
 *	@author	eezstreet
 */
D2Menu_TCPIP::~D2Menu_TCPIP()
{
	delete m_joinMenu;
	delete m_mainMenu;
}

/*
 *	Draws the TCP/IP menu
 *	@author	eezstreet
 */
void D2Menu_TCPIP::Draw()
{
	// Draw the background
	trap->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the "Your IP Address is:" string
	trap->R_DrawText(cl.fontFormal12, m_yourIPString, 265, 110, 300, 15, ALIGN_CENTER, ALIGN_TOP);

	// Draw the IP string
	trap->R_DrawText(cl.fontFormal12, m_ipStringBuffer, 265, 130, 300, 15, ALIGN_CENTER, ALIGN_TOP);

	// Draw the panel
	DrawAllPanels();
}