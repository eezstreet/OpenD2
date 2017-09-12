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

	szCopyrightText = trap->TBL_FindStringFromIndex(TBLINDEX_COPYRIGHT);
	szAllRightsReservedText = trap->TBL_FindStringFromIndex(TBLINDEX_ALLRIGHTS);

	dwStartTicks = trap->Milliseconds();
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
 *	Check to see if there is a waiting signal
 */
bool D2Menu_Trademark::WaitingSignal()
{
	DWORD dwTicks;
	dwTicks = trap->Milliseconds();
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
	trap->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the flaming logo
	trap->R_Animate(blackLeftAnim, 25, 400, -7);
	trap->R_Animate(blackRightAnim, 25, 400, -7);
	trap->R_Animate(flameLeftAnim, 25, 400, -50);
	trap->R_Animate(flameRightAnim, 25, 400, -57);

	// Draw the trademark text
	trap->R_ColorModFont(cl.fontFormal12, 150, 135, 100);
	trap->R_DrawText(cl.fontFormal12, szCopyrightText, 0, 500, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	trap->R_DrawText(cl.fontFormal12, szAllRightsReservedText, 0, 525, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	trap->R_ColorModFont(cl.fontFormal12, 255, 255, 255);
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