#include "D2Menu_Trademark.hpp"
#include "D2Menu_Main.hpp"

#define TBLINDEX_COPYRIGHT	5104
#define	TBLINDEX_ALLRIGHTS	5105

/*
 *	Creates the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::D2Menu_Trademark() : D2Menu()
{
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
		engine->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6", "trademark", 0, 11, PAL_UNITS);
#else
	backgroundTexture = 
		trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\trademark.dc6", "trademark", 0, 11, PAL_UNITS);
#endif

	szCopyrightText = engine->TBL_FindStringFromIndex(TBLINDEX_COPYRIGHT);
	szAllRightsReservedText = engine->TBL_FindStringFromIndex(TBLINDEX_ALLRIGHTS);

	dwStartTicks = engine->Milliseconds();
}

/*
 *	Destroys the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::~D2Menu_Trademark()
{
	engine->R_DeregisterTexture(nullptr, backgroundTexture);
}

/*
 *	Check to see if there is a waiting signal
 */
bool D2Menu_Trademark::WaitingSignal()
{
	DWORD dwTicks;
	dwTicks = engine->Milliseconds();
	if (dwTicks - dwStartTicks >= 10000)
	{
		NotifySignalReady(TrademarkSignal, nullptr, nullptr);
		return true;
	}
	return bSignalReady;
}

/*
 *	Draws the trademark menu
 *	@author	eezstreet
 */
void D2Menu_Trademark::Draw()
{
	// Draw the background
	engine->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the flaming logo
	engine->R_Animate(blackLeftAnim, 25, 400, -7);
	engine->R_Animate(blackRightAnim, 25, 400, -7);
	engine->R_Animate(flameLeftAnim, 25, 400, -50);
	engine->R_Animate(flameRightAnim, 25, 400, -57);

	// Draw the trademark text
	engine->R_ColorModFont(cl.fontFormal12, 150, 135, 100);
	engine->R_DrawText(cl.fontFormal12, szCopyrightText, 0, 500, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	engine->R_DrawText(cl.fontFormal12, szAllRightsReservedText, 0, 525, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	engine->R_ColorModFont(cl.fontFormal12, 255, 255, 255);
}

/*
 *	Handle a mouse click on the menu
 */
bool D2Menu_Trademark::HandleMouseClicked(DWORD dwX, DWORD dwY)
{
	// if mouse is clicked, go to main menu
	NotifySignalReady(TrademarkSignal, nullptr, nullptr);
	return true;
}

/*
 *	The trademark signal is responsible for switching us from Trademark screen to Main Menu
 */
void D2Menu_Trademark::TrademarkSignal(D2Panel* pPanel, D2Widget* pWidget)
{
	delete cl.pActiveMenu;
	cl.pActiveMenu = new D2Menu_Main();
	cl.gamestate = GS_MAINMENU;
}