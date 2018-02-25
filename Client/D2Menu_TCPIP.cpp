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

	m_yourIPString = trap->TBL_FindStringFromIndex(TBLTEXT_YOURIP);
	m_yourIP = trap->NET_GetLocalIP();
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
	trap->R_DrawText(cl.fontFormal12, m_yourIPString, 0, 110, 800, 15, ALIGN_CENTER, ALIGN_TOP);

	// Draw the IP string
	trap->R_DrawText(cl.fontFormal12, m_yourIP, 0, 130, 800, 15, ALIGN_CENTER, ALIGN_TOP);

	// Draw the panel
	DrawAllPanels();
}