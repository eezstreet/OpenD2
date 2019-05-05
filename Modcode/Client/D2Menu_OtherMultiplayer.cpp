#include "D2Menu_OtherMultiplayer.hpp"

/*
 *	Creates the Other Multiplayer menu.
 *	@author	eezstreet
 */
D2Menu_OtherMultiplayer::D2Menu_OtherMultiplayer() : D2Menu()
{
	// Copied from the Main Menu
	tex_handle flameLeftTex =
		engine->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireLeft.dc6", "flameleft", PAL_UNITS);
	tex_handle flameRightTex =
		engine->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireRight.dc6", "flameright", PAL_UNITS);
	tex_handle blackLeftTex =
		engine->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoBlackLeft.dc6", "blackleft", PAL_UNITS);
	tex_handle blackRightTex =
		engine->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoBlackRight.dc6", "blackright", PAL_UNITS);

	engine->R_SetTextureBlendMode(flameLeftTex, BLEND_ADD);
	engine->R_SetTextureBlendMode(flameRightTex, BLEND_ADD);
	engine->R_SetTextureBlendMode(blackLeftTex, BLEND_ALPHA);
	engine->R_SetTextureBlendMode(blackRightTex, BLEND_ALPHA);

	flameLeftAnim = engine->R_RegisterAnimation(flameLeftTex, "flameleft", 0);
	flameRightAnim = engine->R_RegisterAnimation(flameRightTex, "flameright", 0);
	blackLeftAnim = engine->R_RegisterAnimation(blackLeftTex, "blackleft", 0);
	blackRightAnim = engine->R_RegisterAnimation(blackRightTex, "blackright", 0);

#ifdef EXPANSION
	backgroundTexture =
		engine->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6", "mainbg", 0, 11, PAL_UNITS);
#else
	backgroundTexture =
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\gameselectscreen.dc6", "mainbg", 0, 11, PAL_UNITS);
#endif

	m_panel = new D2Panel_OtherMultiplayer();
	AddPanel(m_panel);
}

/*
 *	Destroys the Other Multiplayer menu.
 *	@author	eezstreet
 */
D2Menu_OtherMultiplayer::~D2Menu_OtherMultiplayer()
{
	delete m_panel;
}

/*
 *	Draws the Other Multiplayer menu.
 *	@author	eezstreet
 */
void D2Menu_OtherMultiplayer::Draw()
{
	// Draw the background
	engine->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the flaming logo
	engine->R_Animate(blackLeftAnim, 25, 400, -7);
	engine->R_Animate(blackRightAnim, 25, 400, -7);
	engine->R_Animate(flameLeftAnim, 25, 400, -50);
	engine->R_Animate(flameRightAnim, 25, 400, -57);

	// Draw the version number
	engine->R_ColorModFont(cl.font16, 255, 255, 255);
	engine->R_DrawText(cl.font16, GAME_FULL_UTF16, 20, 560, 0, 0, ALIGN_LEFT, ALIGN_TOP);

	DrawAllPanels();
}