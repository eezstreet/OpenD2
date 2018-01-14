#include "D2Widget_CharSelectList.hpp"

/*
 *	Creates a Character Select list widget.
 *	This method is responsible for loading up all of the savegames.
 *	We should maybe cache the results of the savegame loading so that going to the charselect page doesn't take a while.
 */
D2Widget_CharSelectList::D2Widget_CharSelectList(int x, int y, int w, int h) : D2Widget(x, y, w, h)
{

}

/*
 *	Draws a Character Select list widget.
 */
void D2Widget_CharSelectList::Draw()
{

}

/*
 *	Handles a mouse-down event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		return true;
	}
	return false;
}

/*
 *	Handles a mouse click event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseClick(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		return true;
	}
	return false;
}