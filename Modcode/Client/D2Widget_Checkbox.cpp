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
	label = nullptr;

	checkboxRendered = engine->renderer->AllocateObject(0);
	checkboxTexture = engine->graphics->CreateReference("data\\global\\ui\\FrontEnd\\clickbox.dc6", UsagePolicy_Permanent);
	checkboxRendered->AttachTextureResource(checkboxTexture, m_bChecked);
}

/*
 *	Destroys the checkbox widget
 *	@author	eezstreet
 */
D2Widget_Checkbox::~D2Widget_Checkbox()
{
	engine->renderer->Remove(checkboxRendered);
	if (label != nullptr)
	{
		engine->renderer->Remove(label);
	}
}

/*
 *	Draws the checkbox widget
 *	@author	eezstreet
 */
void D2Widget_Checkbox::Draw()
{
	checkboxRendered->SetDrawCoords(x, y, 15, 15);
	checkboxRendered->Draw();
	if (label != nullptr)
	{
		label->SetDrawCoords(x + w + 6, y, 0, 0);
		label->Draw();
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
	label = engine->renderer->AllocateObject(1);
	label->AttachFontResource(cl.font16);
	label->SetText(szText);

	float r, g, b;
	engine->PAL_GetPL2ColorModulation(engine->renderer->GetGlobalPalette(), TextColor_Gold, r, g, b);
	label->SetColorModulate(r, g, b, 1.0f);
}

/*
 *	Removes the label from this widget
 *	@author	eezstreet
 */
void D2Widget_Checkbox::RemoveLabel()
{
	m_bHasLabel = false;
	engine->renderer->Remove(label);
	label = nullptr;
}

/*
 *	Handles a mousedown event on the widget
 *	@author	eezstreet
 */
bool D2Widget_Checkbox::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
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
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		m_bChecked = !m_bChecked;
		checkboxRendered->AttachTextureResource(checkboxTexture, m_bChecked);
		return true;
	}
	return false;
}