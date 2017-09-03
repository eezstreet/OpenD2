#include "D2Menu_Main.hpp"

/*
 *	Creates the main menu
 *	@author	eezstreet
 */
D2Menu_Main::D2Menu_Main() : D2Menu()
{
	tex_handle flameLeftTex = 
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireLeft.dc6", "flameleft", PAL_UNITS);
	tex_handle flameRightTex =
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireRight.dc6", "flameright", PAL_UNITS);
	trap->R_SetTextureBlendMode(flameLeftTex, BLEND_MOD);
	trap->R_SetTextureBlendMode(flameRightTex, BLEND_MOD);

	flameLeftAnim = trap->R_RegisterAnimation(flameLeftTex, "flameleft", 0);
	flameRightAnim = trap->R_RegisterAnimation(flameRightTex, "flameright", 0);

#ifdef EXPANSION
	backgroundTexture = 
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6", "mainbg", 0, 11, PAL_UNITS);
#else
	backgroundTexture =
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\gameselectscreen.dc6", "mainbg", 0, 11, PAL_UNITS);
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
	// Draw the background
	trap->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the flaming logo
	trap->R_Animate(flameLeftAnim, 25, 400, -50);
	trap->R_Animate(flameRightAnim, 25, 400, -57);
}