#include "D2Menu_Loading.hpp"

/*
 *	Creates the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::D2Menu_Loading()
{
	loadscreenTex = 
		engine->R_RegisterAnimatedDC6("data\\global\\ui\\Loading\\loadingscreen.dc6", "loadingscreen", PAL_LOADING);
	loadscreenAnim = engine->R_RegisterAnimation(loadscreenTex, "loadingscreen", 0);
}

/*
 *	Destroys the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::~D2Menu_Loading()
{
	engine->R_DeregisterAnimation(loadscreenAnim);
	engine->R_DeregisterTexture(nullptr, loadscreenTex);
}

/*
 *	Draws the loading screen
 *	@author	eezstreet
 */
void D2Menu_Loading::Draw()
{
	engine->R_DrawRectangle(0, 0, 800, 600, 0, 0, 0, 255);
	engine->R_SetAnimFrame(loadscreenAnim, cl.nLoadState);
	engine->R_Animate(loadscreenAnim, 0, 272, 172);
}