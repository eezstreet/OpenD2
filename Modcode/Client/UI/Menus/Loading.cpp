#include "Loading.hpp"

namespace D2Menus
{
	/*
	 *	Creates the loading screen
	 *	@author	eezstreet
	 */
	Loading::Loading() : D2Menu()
	{
		engine->renderer->SetGlobalPalette(PAL_LOADING);
		loadscreen = engine->renderer->AllocateObject(0);
		loadscreenTexture = engine->graphics->CreateReference("data\\global\\ui\\Loading\\loadingscreen.dc6", UsagePolicy_SingleUse);
		loadscreen->AttachAnimationResource(loadscreenTexture, true);
	}

	/*
	 *	Destroys the loading screen
	 *	@author	eezstreet
	 */
	Loading::~Loading()
	{
		engine->renderer->Remove(loadscreen);
		engine->graphics->DeleteReference(loadscreenTexture);
	}

	/*
	 *	Draws the loading screen
	 *	@author	eezstreet
	 */
	void Loading::Draw()
	{
		loadscreen->Draw();
	}
}