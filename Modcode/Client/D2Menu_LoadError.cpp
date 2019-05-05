#include "D2Menu_LoadError.hpp"

/*
 *	Creates the LoadError menu
 *	@author	eezstreet
 */
D2Menu_LoadError::D2Menu_LoadError(WORD wStringIndex) : D2Menu()
{
	szErrorText = engine->TBL_FindStringFromIndex(wStringIndex);
}

/*
 *	Draw the LoadError menu
 *	@author	eezstreet
 */
void D2Menu_LoadError::Draw()
{
	engine->R_Clear();
	engine->R_ColorModFont(cl.font16, 255, 255, 255);
	engine->R_DrawText(cl.font16, szErrorText, 20, 200, 780, 20, ALIGN_CENTER, ALIGN_TOP);
}

/*
 *	Handle a mouse click event on the LoadError menu.
 *	@author	eezstreet
 */
bool D2Menu_LoadError::HandleMouseClicked(DWORD dwX, DWORD dwY)
{
	D2Client_GoToContextMenu();
	return true;
}