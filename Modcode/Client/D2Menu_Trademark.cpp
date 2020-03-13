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
	IGraphicsHandle* flameTexLeft = engine->graphics->LoadGraphic(
		"data\\global\\ui\\FrontEnd\\D2LogoFireLeft.dc6",
		UsagePolicy_Permanent
	);
	IGraphicsHandle* flameTexRight = engine->graphics->LoadGraphic(
		"data\\global\\ui\\FrontEnd\\D2LogoFireRight.dc6",
		UsagePolicy_Permanent
	);
	IGraphicsHandle* blackTexLeft = engine->graphics->LoadGraphic(
		"data\\global\\ui\\FrontEnd\\D2LogoBlackLeft.dc6",
		UsagePolicy_Permanent
	);
	IGraphicsHandle* blackTexRight = engine->graphics->LoadGraphic(
		"data\\global\\ui\\FrontEnd\\D2LogoBlackRight.dc6",
		UsagePolicy_Permanent
	);

	engine->renderer->SetGlobalPalette(PAL_SKY);
	backgroundObject = engine->renderer->AllocateObject(0);
	backgroundObject->AttachCompositeTextureResource(background, 0, -1);
	backgroundObject->SetDrawCoords(0, 0, 800, 600);
	backgroundObject->SetPalshift(0);

	flameLeft = engine->renderer->AllocateObject(0);
	flameRight = engine->renderer->AllocateObject(0);
	blackLeft = engine->renderer->AllocateObject(0);
	blackRight = engine->renderer->AllocateObject(0);

	flameLeft->AttachAnimationResource(flameTexLeft);
	flameRight->AttachAnimationResource(flameTexRight);
	blackLeft->AttachAnimationResource(blackTexLeft);
	blackRight->AttachAnimationResource(blackTexRight);

	flameLeft->SetDrawMode(3);
	flameRight->SetDrawMode(3);

	flameLeft->SetDrawCoords(400, -285, -1, -1);
	flameRight->SetDrawCoords(400, -285, -1, -1);
	blackLeft->SetDrawCoords(400, -285, -1, -1);
	blackRight->SetDrawCoords(400, -285, -1, -1);

	// Background isn't needed at this point, it should be in VRAM
	engine->graphics->UnloadGraphic(background);

	szCopyrightText = engine->TBL_FindStringFromIndex(TBLINDEX_COPYRIGHT);
	szAllRightsReservedText = engine->TBL_FindStringFromIndex(TBLINDEX_ALLRIGHTS);

	int trademarkWidth, rightsWidth;

	trademark = engine->renderer->AllocateObject(1);
	allRightsReserved = engine->renderer->AllocateObject(1);

	trademark->AttachFontResource(cl.fontFormal12);
	allRightsReserved->AttachFontResource(cl.fontFormal12);

	trademark->SetText(szCopyrightText);
	allRightsReserved->SetText(szAllRightsReservedText);

	float r, g, b;
	// 4 = gold
	// 6 = black
	// 7 = also gold??
	// 8 = orange
	// 9 = yellow
	// 10 = dark green
	// 11 = purple
	// 12 = medium green?
	if(engine->PAL_GetPL2ColorModulation(engine->renderer->GetGlobalPalette(), TextColor_Gold, r, g, b))
	{
		trademark->SetColorModulate(r, g, b, 1.0f);
		allRightsReserved->SetColorModulate(r, g, b, 1.0f);
	}

	trademark->GetDrawCoords(nullptr, nullptr, &trademarkWidth, nullptr);
	trademark->SetDrawCoords(400 - (trademarkWidth / 2), 500, 0, 0);

	allRightsReserved->GetDrawCoords(nullptr, nullptr, &rightsWidth, nullptr);
	allRightsReserved->SetDrawCoords(400 - (rightsWidth / 2), 525, 0, 0);

	dwStartTicks = engine->Milliseconds();
}

/*
 *	Destroys the trademark menu
 *	@author	eezstreet
 */
D2Menu_Trademark::~D2Menu_Trademark()
{
	engine->renderer->Remove(backgroundObject);
	engine->renderer->Remove(flameLeft);
	engine->renderer->Remove(flameRight);
	engine->renderer->Remove(blackLeft);
	engine->renderer->Remove(blackRight);
	engine->renderer->Remove(trademark);
	engine->renderer->Remove(allRightsReserved);
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
	blackLeft->Draw();
	blackRight->Draw();
	flameLeft->Draw();
	flameRight->Draw();
	trademark->Draw();
	allRightsReserved->Draw();
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
