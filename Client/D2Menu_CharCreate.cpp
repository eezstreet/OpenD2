#include "D2Menu_CharCreate.hpp"

/*
 *	Creates the character creation menu. Lots of creation.
 */
D2Menu_CharCreate::D2Menu_CharCreate()
{
	backgroundTex = trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\characterCreationScreenEXP.dc6", 
		"ccback", 0, 11, PAL_FECHAR);
}

/*
 *	Destroys the character creation menu
 */
D2Menu_CharCreate::~D2Menu_CharCreate()
{
	// we can reasonably unregister any textures here, we probably won't see them again for a long time
	trap->R_DeregisterTexture(nullptr, backgroundTex);
}

/*
 *	Draws the character creation menu
 */
void D2Menu_CharCreate::Draw()
{
	trap->R_DrawTexture(backgroundTex, 0, 0, 800, 600, 0, 0);

	DrawAllPanels();
}