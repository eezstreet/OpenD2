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
	IGraphicsHandle* background = engine->graphics->LoadGraphic(
			"data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6",
			UsagePolicy_SingleUse);

	engine->renderer->SetGlobalPalette(PAL_SKY);
	backgroundObject = engine->renderer->AllocateObject();
	backgroundObject->AttachCompositeTextureResource(background, 0, -1);
	backgroundObject->SetDrawCoords(0, 0, 800, 600);
	backgroundObject->SetTextureCoords(0, 0, 800, 600);
	backgroundObject->SetPalshift(0);

	// Background isn't needed at this point, it should be in VRAM
	delete background;
#if 0
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
		engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6", "trademark", 0, 11, PAL_UNITS);
#else
	backgroundTexture = 
		engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\trademark.dc6", "trademark", 0, 11, PAL_UNITS);
#endif

	szCopyrightText = engine->TBL_FindStringFromIndex(TBLINDEX_COPYRIGHT);
	szAllRightsReservedText = engine->TBL_FindStringFromIndex(TBLINDEX_ALLRIGHTS);
#endif
	dwStartTicks = engine->Milliseconds();
}

/*
 *	Destroys the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::~D2Menu_Trademark()
{
	engine->renderer->Remove(backgroundObject);
#if 0
	engine->renderer->DeregisterTexture(nullptr, backgroundTexture);
#endif
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
	backgroundObject->Draw();
#if 0
	// Draw the background
	engine->renderer->DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// Draw the flaming logo
	engine->renderer->Animate(blackLeftAnim, 25, 400, -7);
	engine->renderer->Animate(blackRightAnim, 25, 400, -7);
	engine->renderer->Animate(flameLeftAnim, 25, 400, -50);
	engine->renderer->Animate(flameRightAnim, 25, 400, -57);

	// Draw the trademark text
	engine->renderer->ColorModFont(cl.fontFormal12, 150, 135, 100);
	engine->renderer->DrawText(cl.fontFormal12, szCopyrightText, 0, 500, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	engine->renderer->DrawText(cl.fontFormal12, szAllRightsReservedText, 0, 525, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	engine->renderer->ColorModFont(cl.fontFormal12, 255, 255, 255);
#endif
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
