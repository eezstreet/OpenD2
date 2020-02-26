#include "D2Menu_Loading.hpp"

/*
 *	Creates the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::D2Menu_Loading()
{
#if 0
	loadscreenTex = 
		engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\Loading\\loadingscreen.dc6", "loadingscreen", PAL_LOADING);
	loadscreenAnim = engine->renderer->RegisterDC6Animation(loadscreenTex, "loadingscreen", 0);
#endif
}

/*
 *	Destroys the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::~D2Menu_Loading()
{
#if 0
	engine->renderer->DeregisterAnimation(loadscreenAnim);
	engine->renderer->DeregisterTexture(nullptr, loadscreenTex);
#endif
}

/*
 *	Draws the loading screen
 *	@author	eezstreet
 */
void D2Menu_Loading::Draw()
{
#if 0
	engine->renderer->DrawRectangle(0, 0, 800, 600, 0, 0, 0, 255);
	engine->renderer->SetAnimFrame(loadscreenAnim, cl.nLoadState);
	engine->renderer->Animate(loadscreenAnim, 0, 272, 172);
#endif
}