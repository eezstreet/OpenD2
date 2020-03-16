#include "D2Menu_Loading.hpp"

/*
 *	Creates the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::D2Menu_Loading()
{
	engine->renderer->SetGlobalPalette(PAL_LOADING);
	loadscreen = engine->renderer->AllocateObject(0);
	loadscreenTexture = engine->graphics->LoadGraphic("data\\global\\ui\\Loading\\loadingscreen.dc6", UsagePolicy_SingleUse);
	loadscreen->AttachAnimationResource(loadscreenTexture, true);
}

/*
 *	Destroys the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::~D2Menu_Loading()
{
	engine->renderer->Remove(loadscreen);
	engine->graphics->UnloadGraphic(loadscreenTexture);
}

/*
 *	Draws the loading screen
 *	@author	eezstreet
 */
void D2Menu_Loading::Draw()
{
	loadscreen->Draw();
}