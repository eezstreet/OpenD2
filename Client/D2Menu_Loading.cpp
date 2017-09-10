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
 *	THE MAIN LOADING PROCEDURE STARTS HERE
 */
void D2Menu_Loading::AdvanceLoading()
{
	if (cl.nLoadState == 0)
	{	// load (and transmit, if on a non-local game) savegame
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 1)
	{	// load D2Common
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 2)
	{	// load D2Game, if necessary
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 3)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 4)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 5)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 6)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 7)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 8)
	{	// ??
		cl.nLoadState++;
	}
	else if (cl.nLoadState == 9)
	{	// go ingame
		cl.nLoadState = 0;
	}
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
	AdvanceLoading();
}