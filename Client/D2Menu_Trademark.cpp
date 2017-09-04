#include "D2Menu_Trademark.hpp"

/*
 *	Creates the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::D2Menu_Trademark() : D2Menu()
{
	tex_handle flameLeftTex =
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireLeft.dc6", "flameleft", PAL_UNITS);
	tex_handle flameRightTex =
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireRight.dc6", "flameright", PAL_UNITS);
	tex_handle blackLeftTex =
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoBlackLeft.dc6", "blackleft", PAL_UNITS);
	tex_handle blackRightTex =
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoBlackRight.dc6", "blackright", PAL_UNITS);

	trap->R_SetTextureBlendMode(flameLeftTex, BLEND_ADD);
	trap->R_SetTextureBlendMode(flameRightTex, BLEND_ADD);
	trap->R_SetTextureBlendMode(blackLeftTex, BLEND_ALPHA);
	trap->R_SetTextureBlendMode(blackRightTex, BLEND_ALPHA);

	flameLeftAnim = trap->R_RegisterAnimation(flameLeftTex, "flameleft", 0);
	flameRightAnim = trap->R_RegisterAnimation(flameRightTex, "flameright", 0);
	blackLeftAnim = trap->R_RegisterAnimation(blackLeftTex, "blackleft", 0);
	blackRightAnim = trap->R_RegisterAnimation(blackRightTex, "blackright", 0);

#ifdef EXPANSION
	backgroundTexture = 
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6", "trademark", 0, 11, PAL_UNITS);
#else
	backgroundTexture = 
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\trademark.dc6", "trademark", 0, 11, PAL_UNITS);
#endif
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

	// Draw the flaming logo
	trap->R_Animate(blackLeftAnim, 25, 400, -7);
	trap->R_Animate(blackRightAnim, 25, 400, -7);
	trap->R_Animate(flameLeftAnim, 25, 400, -50);
	trap->R_Animate(flameRightAnim, 25, 400, -57);

	// Draw the widgets
	D2Menu::Draw();

	// Note: on this menu, we don't draw the mouse cursor
}