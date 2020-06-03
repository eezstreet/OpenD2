#include "D2Menu_Loading.hpp"

/*
 *	Creates the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::D2Menu_Loading()
{
	loadscreenTex = 
		engine->renderer->TextureFromAnimatedDC6("data/global/ui/Loading/loadingscreen.dc6", "loadingscreen", PAL_LOADING);
	loadscreenAnim = engine->renderer->RegisterDC6Animation(loadscreenTex, "loadingscreen", 0);
}

/*
 *	Destroys the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::~D2Menu_Loading()
{
	engine->renderer->DeregisterAnimation(loadscreenAnim);
	engine->renderer->DeregisterTexture(nullptr, loadscreenTex);
}

/*
 *	Draws the loading screen
 *	@author	eezstreet
 */
void D2Menu_Loading::Draw()
{
	engine->renderer->DrawRectangle(0, 0, 800, 600, 0, 0, 0, 255);
	engine->renderer->SetAnimFrame(loadscreenAnim, cl.nLoadState);
	engine->renderer->Animate(loadscreenAnim, 0, 272, 172);
}
