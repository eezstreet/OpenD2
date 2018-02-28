#include "D2Menu_LoadError.hpp"

/*
 *	Creates the LoadError menu
 *	@author	eezstreet
 */
D2Menu_LoadError::D2Menu_LoadError(WORD wStringIndex) : D2Menu()
{
	szErrorText = trap->TBL_FindStringFromIndex(wStringIndex);
}

/*
 *	Draw the LoadError menu
 *	@author	eezstreet
 */
void D2Menu_LoadError::Draw()
{
	trap->R_Clear();
	trap->R_ColorModFont(cl.font16, 255, 255, 255);
	trap->R_DrawText(cl.font16, szErrorText, 20, 200, 780, 20, ALIGN_CENTER, ALIGN_TOP);
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