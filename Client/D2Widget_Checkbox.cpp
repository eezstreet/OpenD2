#include "D2Widget_Checkbox.hpp"

/*
 *	Creates the checkbox widget
 *	@author	eezstreet
 */
D2Widget_Checkbox::D2Widget_Checkbox(int _x, int _y, bool bStartChecked) : D2Widget()
{
	x = _x;
	y = _y;
	w = 15;
	h = 16;

	m_bChecked = bStartChecked;
	m_bHasLabel = false;

	checkboxTex = engine->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\clickbox.dc6", "clickbox", PAL_FECHAR);
	checkboxAnim = engine->R_RegisterAnimation(checkboxTex, "clickbox", 0);
}

/*
 *	Destroys the checkbox widget
 *	@author	eezstreet
 */
D2Widget_Checkbox::~D2Widget_Checkbox()
{
	// nothing to destroy
}

/*
 *	Draws the checkbox widget
 *	@author	eezstreet
 */
void D2Widget_Checkbox::Draw()
{
	engine->R_SetAnimFrame(checkboxAnim, m_bChecked);
	engine->R_Animate(checkboxAnim, 0, m_pOwner->x + x, m_pOwner->y + y);

	if (m_bHasLabel)
	{
		engine->R_ColorModFont(cl.font16, 150, 135, 100);
		engine->R_DrawText(cl.font16, szLabel, m_pOwner->x + x + w + 6, m_pOwner->y + y + 3, 0, 0, ALIGN_LEFT, ALIGN_TOP);
	}
}

/*
 *	Attaches a label to this widget
 *	@author	eezstreet
 */
void D2Widget_Checkbox::AttachLabel(char16_t* szText)
{
	D2Lib::qstrncpyz(szLabel, szText, 32);
	m_bHasLabel = true;
}

/*
 *	Removes the label from this widget
 *	@author	eezstreet
 */
void D2Widget_Checkbox::RemoveLabel()
{
	m_bHasLabel = false;
}

/*
 *	Handles a mousedown event on the widget
 *	@author	eezstreet
 */
bool D2Widget_Checkbox::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (dwX >= m_pOwner->x + x && dwX <= m_pOwner->x + x + w && dwY >= m_pOwner->y + y && dwY <= m_pOwner->y + y + h)
	{
		return true;
	}
	return false;
}

/*
 *	Handles a mouse click event on the widget
 *	@author eezstreet
 */
bool D2Widget_Checkbox::HandleMouseClick(DWORD dwX, DWORD dwY)
{
	if (dwX >= m_pOwner->x + x && dwX <= m_pOwner->x + x + w && dwY >= m_pOwner->y + y && dwY <= m_pOwner->y + y + h)
	{
		m_bChecked = !m_bChecked;
		return true;
	}
	return false;
}