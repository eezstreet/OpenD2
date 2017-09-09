#include "D2Widget_TextEntry.hpp"

/*
 *	Creates the text entry widget
 *	@author	eezstreet
 */
D2Widget_TextEntry::D2Widget_TextEntry(int _x, int _y, bool bStartFocus, bool bAlwaysFocus, bool bCharSelect) : D2Widget()
{
	x = _x;
	y = _y;

	m_bHasFocus = bStartFocus;
	m_bAlwaysFocus = bAlwaysFocus;

	szTextBuffer[0] = '\0';

	if (bCharSelect)
	{
		backgroundTexture = 
			trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\textbox.dc6", "textbox", PAL_FECHAR);
		backgroundAnim = trap->R_RegisterAnimation(backgroundTexture, "textbox", 0);
	}
	else
	{
		backgroundTexture = 
			trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\textbox2.dc6", "textbox2", PAL_UNITS);
		backgroundAnim = trap->R_RegisterAnimation(backgroundTexture, "textbox2", 0);
	}

	trap->R_PollTexture(backgroundTexture, (DWORD*)&this->w, (DWORD*)&this->h);
}

/*
 *	Destroys the text entry widget
 *	@author	eezstreet
 */
D2Widget_TextEntry::~D2Widget_TextEntry()
{
	trap->R_DeregisterTexture(nullptr, backgroundTexture);
}

/*
 *	Adds text (label) to the widget
 *	@author	eezstreet
 */
void D2Widget_TextEntry::AttachLabel(char16_t* szText)
{
	D2_qstrncpyz(szLabel, szText, 32);
	m_bHasAttachedText = true;
}

/*
 *	Removes text (label) from the widget
 *	@author eezstreet
 */
void D2Widget_TextEntry::DetachLabel()
{
	m_bHasAttachedText = false;
}

/*
 *	Draws the text entry widget
 *	@author	eezstreet
 */
void D2Widget_TextEntry::Draw()
{
	// draw background
	trap->R_Animate(backgroundAnim, 0, x, y);

	// draw a label, if present
	if (m_bHasAttachedText)
	{
		trap->R_ColorModFont(cl.font16, 150, 135, 100);
		trap->R_DrawText(cl.font16, szLabel, x, y - 15, w, 10, ALIGN_LEFT, ALIGN_BOTTOM);
	}

	if (szTextBuffer[0] != '\0')
	{
		// draw text, if present
		trap->R_ColorModFont(cl.fontFormal12, 255, 255, 255);
		trap->R_DrawText(cl.fontFormal12, szTextBuffer, x + 2, y, w, h, ALIGN_LEFT, ALIGN_CENTER);
	}
}

/*
 *	Handle a mouse down on the text entry widget
 *	@author	eezstreet
 */
bool D2Widget_TextEntry::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwY >= y && dwX <= x + w && dwY <= y + h)
	{
		return true;
	}
	return false;
}

/*
 *	Handle a mouse click on the text entry widget
 *	@author	eezstreet
 */
bool D2Widget_TextEntry::HandleMouseClick(DWORD dwX, DWORD dwY)
{
	if (m_bAlwaysFocus)
	{	// we're always in focus, just go away
		return false;
	}

	if (dwX >= x && dwY >= y && dwX <= x + w && dwY <= y + h)
	{
		m_bHasFocus = true;
		return true;
	}

	m_bHasFocus = false;
	return false;
}