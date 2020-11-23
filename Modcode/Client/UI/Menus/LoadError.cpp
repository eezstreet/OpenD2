#include "LoadError.hpp"

namespace D2Menus
{
	/*
	 *	Creates the LoadError menu
	 *	@author	eezstreet
	 */
	LoadError::LoadError(WORD wStringIndex) : D2Menu()
	{
		szErrorText = engine->TBL_FindStringFromIndex(wStringIndex);
	}

	/*
	 *	Draw the LoadError menu
	 *	@author	eezstreet
	 */
	void LoadError::Draw()
	{
#if 0
		engine->renderer->Clear();
		engine->renderer->ColorModFont(cl.font16, 255, 255, 255);
		engine->renderer->DrawText(cl.font16, szErrorText, 20, 200, 780, 20, ALIGN_CENTER, ALIGN_TOP);
#endif
	}

	/*
	 *	Handle a mouse click event on the LoadError menu.
	 *	@author	eezstreet
	 */
	bool LoadError::HandleMouseClicked(DWORD dwX, DWORD dwY)
	{
		D2Client_GoToContextMenu();
		return true;
	}
}