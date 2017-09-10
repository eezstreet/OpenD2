#include "D2Widget.hpp"

D2Widget::D2Widget()
{
	m_pNextVisibleWidget = nullptr;
	m_pNextWidget = nullptr;
	m_bVisible = false;
	bHasIdentifier = false;
}

D2Widget::D2Widget(int _x, int _y, int _w, int _h)
{
	m_pNextVisibleWidget = nullptr;
	m_pNextWidget = nullptr;
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	m_bVisible = false;
	bHasIdentifier = false;
}

D2Widget::~D2Widget()
{

}

/*
 *	Attaches an identifier to this object which we can use to reference later
 */
void D2Widget::AttachIdentifier(char* szIdentifier)
{
	bHasIdentifier = true;
	D2_strncpyz(identifier, szIdentifier, 64);
}

/*
 *	Removes the identifier.
 */
void D2Widget::RemoveIdentifier()
{
	bHasIdentifier = false;
}

/*
 *	Gets the identifier.
 */
char* D2Widget::GetIdentifier()
{
	if (!bHasIdentifier)
	{
		return nullptr;
	}
	return identifier;
}

/*
 *	Handles a text input event
 *	@author	eezstreet
 */
bool D2Widget::HandleTextInput(char* szText)
{
	return false;	// most widgets can't handle a text input event
}

/*
 *	Handles a text editing event
 *	@author	eezstreet
 */
bool D2Widget::HandleTextEditing(char* szText, int nStart, int nLength)
{
	return false;	// most widgets can't handle a text editing event
}

/*
 *	Handles a key down event
 *	@author	eezstreet
 */
bool D2Widget::HandleKeyDown(DWORD dwKey)
{
	return false;	// most widgets can't handle a key down event
}

/*
 *	Handles a key up event
 *	@author	eezstreet
 */
bool D2Widget::HandleKeyUp(DWORD dwKey)
{
	return false;	// most widgets can't handle a key up event
}