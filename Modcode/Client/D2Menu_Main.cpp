#include "D2Menu_Main.hpp"

/*
 *	Creates the main menu
 *	@author	eezstreet
 */
D2Menu_Main::D2Menu_Main() : D2Menu()
{
	IGraphicsReference* flameTexLeft = engine->graphics->CreateReference(
		"data\\global\\ui\\FrontEnd\\D2LogoFireLeft.dc6",
		UsagePolicy_Permanent
	);
	IGraphicsReference* flameTexRight = engine->graphics->CreateReference(
		"data\\global\\ui\\FrontEnd\\D2LogoFireRight.dc6",
		UsagePolicy_Permanent
	);
	IGraphicsReference* blackTexLeft = engine->graphics->CreateReference(
		"data\\global\\ui\\FrontEnd\\D2LogoBlackLeft.dc6",
		UsagePolicy_Permanent
	);
	IGraphicsReference* blackTexRight = engine->graphics->CreateReference(
		"data\\global\\ui\\FrontEnd\\D2LogoBlackRight.dc6",
		UsagePolicy_Permanent
	);

	background = engine->graphics->CreateReference(
		"data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6",
		UsagePolicy_Permanent // for now
	);

	backgroundObject = engine->renderer->AllocateObject(0);
	backgroundObject->AttachCompositeTextureResource(background, 0, -1);
	backgroundObject->SetDrawCoords(0, 0, 800, 600);
	backgroundObject->SetPalshift(0);

	flameLeft = engine->renderer->AllocateObject(0);
	flameRight = engine->renderer->AllocateObject(0);
	blackLeft = engine->renderer->AllocateObject(0);
	blackRight = engine->renderer->AllocateObject(0);

	flameLeft->AttachAnimationResource(flameTexLeft, true);
	flameRight->AttachAnimationResource(flameTexRight, true);
	blackLeft->AttachAnimationResource(blackTexLeft, true);
	blackRight->AttachAnimationResource(blackTexRight, true);

	flameLeft->SetDrawMode(3);
	flameRight->SetDrawMode(3);

	flameLeft->SetDrawCoords(400, -285, -1, -1);
	flameRight->SetDrawCoords(400, -285, -1, -1);
	blackLeft->SetDrawCoords(400, -285, -1, -1);
	blackRight->SetDrawCoords(400, -285, -1, -1);

	versionText = engine->renderer->AllocateObject(1);
	versionText->AttachFontResource(cl.font16);
	versionText->SetText(GAME_FULL_UTF16);
	versionText->SetDrawCoords(20, 560, 0, 0);

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
	backgroundObject->Draw();
	blackLeft->Draw();
	blackRight->Draw();
	flameLeft->Draw();
	flameRight->Draw();
	versionText->Draw();

	DrawAllPanels();
}