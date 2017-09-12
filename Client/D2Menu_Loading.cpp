#include "D2Menu_Loading.hpp"

/*
 *	Creates the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::D2Menu_Loading()
{
	loadscreenTex = 
		trap->R_RegisterAnimatedDC6("data\\global\\ui\\Loading\\loadingscreen.dc6", "loadingscreen", PAL_LOADING);
	loadscreenAnim = trap->R_RegisterAnimation(loadscreenTex, "loadingscreen", 0);
}

/*
 *	Destroys the loading screen
 *	@author	eezstreet
 */
D2Menu_Loading::~D2Menu_Loading()
{
	trap->R_DeregisterAnimation(loadscreenAnim);
	trap->R_DeregisterTexture(nullptr, loadscreenTex);
}

/*
 *	Draws the loading screen
 *	@author	eezstreet
 */
void D2Menu_Loading::Draw()
{
	trap->R_DrawRectangle(0, 0, 800, 600, 0, 0, 0, 255);
	trap->R_SetAnimFrame(loadscreenAnim, cl.nLoadState);
	trap->R_Animate(loadscreenAnim, 0, 272, 172);
}