#include "D2Menu_Main.hpp"

/*
 *	Creates the main menu
 *	@author	eezstreet
 */
D2Menu_Main::D2Menu_Main() : D2Menu()
{
#ifdef EXPANSION
	backgroundTexture = 
		trap->R_StitchedDC6Texture("data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6", "mainbg", 0, 11, PAL_UNITS);
#else
	backgroundTexture =
		trap->R_StitchedDC6Texture("data\\global\\ui\\FrontEnd\\gameselectscreen.dc6", "mainbg", 0, 11, PAL_UNITS);
#endif
}

/*
 *	Destroys the main menu
 *	@author	eezstreet
 */
D2Menu_Main::~D2Menu_Main()
{
	// We don't kill the game select background, because we might need it again later
}

/*
 *	Draws the main menu
 *	@author	eezstreet
 */
void D2Menu_Main::Draw()
{
	trap->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);
}