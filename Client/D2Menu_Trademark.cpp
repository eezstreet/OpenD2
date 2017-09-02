#include "D2Menu_Trademark.hpp"

/*
 *	Creates the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::D2Menu_Trademark() : D2Menu()
{
	// The trademark screen background
#ifdef EXPANSION
	backgroundTexture = 
		trap->R_StitchedDC6Texture("data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6", "trademark", 0, 11, PAL_UNITS);
#else
	backgroundTexture = 
		trap->R_StitchedDC6Texture("data\\global\\ui\\FrontEnd\\trademark.dc6", "trademark", 0, 11, PAL_UNITS);
#endif

	// There are only three widgets on the trademark screen:
	// 1. Animated Diablo II logo (left half)
	// 2. Animated Diablo II logo (right half)
	// 3. Copyright Text
}

/*
 *	Destroys the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::~D2Menu_Trademark()
{
	trap->R_DeregisterTexture(nullptr, backgroundTexture);
}

/*
 *	Draws the trademark menu
 *	@author	eezstreet
 */
void D2Menu_Trademark::Draw()
{
	// Draw the background
	trap->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the widgets
	D2Menu::Draw();

	// Note: on this menu, we don't draw the mouse cursor
}