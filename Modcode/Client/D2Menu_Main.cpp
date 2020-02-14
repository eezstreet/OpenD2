#include "D2Menu_Main.hpp"

/*
 *	Creates the main menu
 *	@author	eezstreet
 */
D2Menu_Main::D2Menu_Main() : D2Menu()
{
	tex_handle flameLeftTex = 
		engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireLeft.dc6", "flameleft", PAL_UNITS);
	tex_handle flameRightTex =
		engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoFireRight.dc6", "flameright", PAL_UNITS);
	tex_handle blackLeftTex =
		engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoBlackLeft.dc6", "blackleft", PAL_UNITS);
	tex_handle blackRightTex =
		engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\D2LogoBlackRight.dc6", "blackright", PAL_UNITS);

	engine->renderer->SetTextureBlendMode(flameLeftTex, BLEND_ADD);
	engine->renderer->SetTextureBlendMode(flameRightTex, BLEND_ADD);
	engine->renderer->SetTextureBlendMode(blackLeftTex, BLEND_ALPHA);
	engine->renderer->SetTextureBlendMode(blackRightTex, BLEND_ALPHA);

	flameLeftAnim = engine->renderer->RegisterDC6Animation(flameLeftTex, "flameleft", 0);
	flameRightAnim = engine->renderer->RegisterDC6Animation(flameRightTex, "flameright", 0);
	blackLeftAnim = engine->renderer->RegisterDC6Animation(blackLeftTex, "blackleft", 0);
	blackRightAnim = engine->renderer->RegisterDC6Animation(blackRightTex, "blackright", 0);

#ifdef EXPANSION
	backgroundTexture = 
		engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6", "mainbg", 0, 11, PAL_UNITS);
#else
	backgroundTexture =
		engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\gameselectscreen.dc6", "mainbg", 0, 11, PAL_UNITS);
#endif

	pMainPanel = new D2Panel_Main();
	AddPanel(pMainPanel);
}

/*
 *	Destroys the main menu
 *	@author	eezstreet
 */
D2Menu_Main::~D2Menu_Main()
{
	// We don't kill the game select background, because we might need it again later
	delete pMainPanel;
}

/*
 *	Draws the main menu
 *	@author	eezstreet
 */
void D2Menu_Main::Draw()
{
	// Draw the background
	engine->renderer->DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the flaming logo
	engine->renderer->Animate(blackLeftAnim, 25, 400, -7);
	engine->renderer->Animate(blackRightAnim, 25, 400, -7);
	engine->renderer->Animate(flameLeftAnim, 25, 400, -50);
	engine->renderer->Animate(flameRightAnim, 25, 400, -57);

	// Draw the version number
	engine->renderer->ColorModFont(cl.font16, 255, 255, 255);
	engine->renderer->DrawText(cl.font16, GAME_FULL_UTF16, 20, 560, 0, 0, ALIGN_LEFT, ALIGN_TOP);

	DrawAllPanels();
}