#include "Trademark.hpp"
#include "Main.hpp"

#define TBLINDEX_COPYRIGHT	5104
#define	TBLINDEX_ALLRIGHTS	5105

namespace D2Menus
{
	/*
	 *	Creates the trademark menu
	 *	@author	eezstreet
	 */
	Trademark::Trademark() : D2Menu()
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
			"data\\global\\ui\\FrontEnd\\trademarkscreenEXP.dc6",
			UsagePolicy_SingleUse);

		engine->renderer->SetGlobalPalette(PAL_SKY);
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

		szCopyrightText = engine->TBL_FindStringFromIndex(TBLINDEX_COPYRIGHT);
		szAllRightsReservedText = engine->TBL_FindStringFromIndex(TBLINDEX_ALLRIGHTS);

		trademark = engine->renderer->AllocateObject(1);
		allRightsReserved = engine->renderer->AllocateObject(1);

		trademark->AttachFontResource(cl.fontFormal12);
		allRightsReserved->AttachFontResource(cl.fontFormal12);

		trademark->SetText(szCopyrightText);
		allRightsReserved->SetText(szAllRightsReservedText);
		trademark->SetTextAlignment(0, 500, 800, 0, ALIGN_CENTER, ALIGN_TOP);
		allRightsReserved->SetTextAlignment(0, 525, 800, 0, ALIGN_CENTER, ALIGN_TOP);
		trademark->SetTextColor(TextColor_Gold);
		allRightsReserved->SetTextColor(TextColor_Gold);

		m_dwTimeRemaining = 10000;
	}

	/*
	 *	Destroys the trademark menu
	 *	@author	eezstreet
	 */
	Trademark::~Trademark()
	{
		engine->graphics->DeleteReference(background);
		engine->renderer->Remove(backgroundObject);
		engine->renderer->Remove(flameLeft);
		engine->renderer->Remove(flameRight);
		engine->renderer->Remove(blackLeft);
		engine->renderer->Remove(blackRight);
		engine->renderer->Remove(trademark);
		engine->renderer->Remove(allRightsReserved);
	}

	/*
	 *	Some game logic should occur on this frame.
	 *	@author eezstreet
	 */
	void Trademark::Tick(DWORD dwDeltaMs)
	{
		D2Menu::Tick(dwDeltaMs);

		m_dwTimeRemaining -= dwDeltaMs;
		if (m_dwTimeRemaining <= 0)
		{
			delete cl.pActiveMenu;
			cl.pActiveMenu = new Main();
			cl.gamestate = GS_MAINMENU;
		}
	}

	/*
	 *	Draws the trademark menu
	 *	@author	eezstreet
	 */
	void Trademark::Draw()
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
	bool Trademark::HandleMouseClicked(DWORD dwX, DWORD dwY)
	{
		// if mouse is clicked, go to main menu
		m_dwTimeRemaining = 0;
		return true;
	}
}