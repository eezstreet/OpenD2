#include "D2Widget_Button.hpp"

/*
 *	Creates a new button widget item
 */
D2Widget_Button::D2Widget_Button(int x, int y, char* szDC6Path, char* szButtonType,
	DWORD dwStartEnabled, DWORD dwEndEnabled,
	DWORD dwStartDown, DWORD dwEndDown,
	DWORD dwStartDisabled, DWORD dwEndDisabled)
	: D2Widget(x, y, 0, 0)
{
	char typeBuffer[32]{ 0 };

	font = cl.fontExocet10;

	// register all of the textures
	snprintf(typeBuffer, 32, "%s_up", szButtonType);
	texture_up = trap->R_RegisterDC6Texture(szDC6Path, typeBuffer, dwStartEnabled, dwEndEnabled, PAL_UNITS);

	snprintf(typeBuffer, 32, "%s_dn", szButtonType);
	texture_down = trap->R_RegisterDC6Texture(szDC6Path, typeBuffer, dwStartDown, dwEndDown, PAL_UNITS);

	snprintf(typeBuffer, 32, "%s_ds", szButtonType);
	texture_disabled = trap->R_RegisterDC6Texture(szDC6Path, typeBuffer, dwStartDisabled, dwEndDisabled, PAL_UNITS);

	trap->R_PollTexture(texture_up, (DWORD*)&this->w, (DWORD*)&this->h);

	bDisabled = false;
	bDown = false;
	bHasClickSignal = false;

	if (dwStartDisabled == dwStartEnabled)
	{
		bAlphaModulateDisable = true;
	}
	else
	{
		bAlphaModulateDisable = false;
	}
}

/*
 *	Deconstructs a button widget
 */
D2Widget_Button::~D2Widget_Button()
{

}

/*
 *	Draws the button widget
 */
void D2Widget_Button::Draw()
{
	if (bDisabled)
	{
		if (bAlphaModulateDisable)
		{
			trap->R_ColorModTexture(texture_disabled, 122, 122, 122);
		}
		trap->R_DrawTexture(texture_disabled, x, y, w, h, 0, 0);
		if (bAlphaModulateDisable)
		{
			trap->R_ColorModTexture(texture_disabled, 255, 255, 255);
		}
	}
	else if (bDown)
	{
		trap->R_DrawTexture(texture_down, x, y, w, h, 0, 0);
	}
	else
	{
		trap->R_DrawTexture(texture_up, x, y, w, h, 0, 0);
	}

	if (bHasText)
	{
		trap->R_ColorModFont(font, 0, 0, 0);	// buttons only have text in black exocet, font16
		if (bDown)
		{ // shift the x and y a little bit to make it look like we're pressing the button
			trap->R_DrawText(font, buttonTextBuffer, x - 2, y + 2, w, h, ALIGN_CENTER, ALIGN_CENTER);
		}
		else
		{
			trap->R_DrawText(font, buttonTextBuffer, x, y, w, h, ALIGN_CENTER, ALIGN_CENTER);
		}
		
	}
}

/*
 *	Attaches text to the button widget
 */
void D2Widget_Button::AttachText(char16_t* szText)
{
	D2_qstrncpyz(buttonTextBuffer, szText, 64);
	bHasText = true;
}

/*
 *	Removes text from the button widget
 */
void D2Widget_Button::DetachText()
{
	bHasText = false;
}

/*
 *	Adds a click signal to this button
 */
void D2Widget_Button::AttachClickSignal(MenuSignal pClickSignal)
{
	bHasClickSignal = true;
	clickSignal = pClickSignal;
}

/*
 *	Handles a mouse down
 */
bool D2Widget_Button::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (bDisabled)
	{
		return false; // don't do anything when disabled
	}

	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{	// it got pressed
		bDown = true;
		return true;
	}
	else
	{
		bDown = false;
		return false;
	}
}

/*
 *	Handles a mouse click
 */
bool D2Widget_Button::HandleMouseClick(DWORD dwX, DWORD dwY)
{
	bool bWasDown = bDown;

	if (bDisabled)
	{
		return false; // don't do anything when disabled
	}

	bDown = false;

	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		if (bWasDown)
		{
			if (bHasClickSignal)
			{
				m_pOwner->NotifySignalReady(clickSignal, this);
			}
			return true;
		}
	}
	return false;
}