#include "D2Menu_CharCreate.hpp"

/*
 *	Creates the character creation menu. Lots of creation.
 */
D2Menu_CharCreate::D2Menu_CharCreate()
{
	tex_handle fireTex = trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\fire.DC6", "ccfire", PAL_FECHAR);

	backgroundTex = trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\characterCreationScreenEXP.dc6", 
		"ccback", 0, 11, PAL_FECHAR);
	fireAnim = trap->R_RegisterAnimation(fireTex, "ccfire", 0);

	pStaticPanel = new D2Panel_CharCreate_Static();
	pDynamicPanel = new D2Panel_CharCreate_Dynamic();

	AddPanel(pStaticPanel, true);
	AddPanel(pDynamicPanel, false);
}

/*
 *	Destroys the character creation menu
 */
D2Menu_CharCreate::~D2Menu_CharCreate()
{
	// we can reasonably unregister any textures here, we probably won't see them again for a long time
	trap->R_DeregisterTexture(nullptr, backgroundTex);
	trap->R_DeregisterTexture("ccfire", INVALID_HANDLE);
	trap->R_DeregisterAnimation(fireAnim);

	delete pStaticPanel;
	delete pDynamicPanel;
}

/*
 *	Draws the character creation menu
 */
void D2Menu_CharCreate::Draw()
{
	// draw background and fire
	trap->R_DrawTexture(backgroundTex, 0, 0, 800, 600, 0, 0);
	trap->R_Animate(fireAnim, 25, 380, 150);

	DrawAllPanels();
}