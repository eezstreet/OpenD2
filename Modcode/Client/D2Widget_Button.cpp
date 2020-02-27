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
	backgroundObjectUp = engine->renderer->AddStaticDC6(szDC6Path, dwStartEnabled, dwEndEnabled);
	backgroundObjectUp->SetDrawCoords(x, y, -1, -1);
	backgroundObjectDown = engine->renderer->AddStaticDC6(szDC6Path, dwStartDown, dwEndDown);
	backgroundObjectDown->SetDrawCoords(x, y, -1, -1);
	backgroundObjectDisabled = engine->renderer->AddStaticDC6(szDC6Path, dwStartDisabled, dwEndDisabled);
	backgroundObjectDisabled->SetDrawCoords(x, y, -1, -1);
#if 0
	char typeBuffer[32]{ 0 };

	font = cl.fontExocet10;

	// register all of the textures
	snprintf(typeBuffer, 32, "%s_up", szButtonType);
	texture_up = engine->renderer->TextureFromStitchedDC6(szDC6Path, typeBuffer, dwStartEnabled, dwEndEnabled, PAL_UNITS);

	snprintf(typeBuffer, 32, "%s_dn", szButtonType);
	texture_down = engine->renderer->TextureFromStitchedDC6(szDC6Path, typeBuffer, dwStartDown, dwEndDown, PAL_UNITS);

	snprintf(typeBuffer, 32, "%s_ds", szButtonType);
	texture_disabled = engine->renderer->TextureFromStitchedDC6(szDC6Path, typeBuffer, dwStartDisabled, dwEndDisabled, PAL_UNITS);

	engine->renderer->PollTexture(texture_up, (DWORD*)&this->w, (DWORD*)&this->h);

#endif
	clickedSound = engine->S_RegisterSound("data\\global\\sfx\\cursor\\button.wav");

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

	backgroundObjectUp->GetDrawCoords(nullptr, nullptr, &w, &h);
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
		backgroundObjectDisabled->Draw();
	}
	else if (bDown)
	{
		backgroundObjectDown->Draw();
	}
	else
	{
		backgroundObjectUp->Draw();
	}
#if 0
	if (bDisabled)
	{
		if (bAlphaModulateDisable)
		{
			engine->renderer->ColorModTexture(texture_disabled, 122, 122, 122);
		}
		engine->renderer->DrawTexture(texture_disabled, m_pOwner->x + x, m_pOwner->y + y, w, h, 0, 0);
		if (bAlphaModulateDisable)
		{
			engine->renderer->ColorModTexture(texture_disabled, 255, 255, 255);
		}
	}
	else if (bDown)
	{
		engine->renderer->DrawTexture(texture_down, m_pOwner->x + x, m_pOwner->y + y, w, h, 0, 0);
	}
	else
	{
		engine->renderer->DrawTexture(texture_up, m_pOwner->x + x, m_pOwner->y + y, w, h, 0, 0);
	}

	if (bHasText)
	{
		engine->renderer->ColorModFont(font, 0, 0, 0);	// buttons only have text in black exocet, font16
		if (bDown)
		{ // shift the x and y a little bit to make it look like we're pressing the button
			engine->renderer->DrawText(font, buttonTextBuffer, m_pOwner->x + x - 2, m_pOwner->y + y + 2, 
				w, h, ALIGN_CENTER, ALIGN_CENTER);
		}
		else
		{
			engine->renderer->DrawText(font, buttonTextBuffer, m_pOwner->x + x, m_pOwner->y + y, w, h, ALIGN_CENTER, ALIGN_CENTER);
		}
		
	}
#endif
}

/*
 *	Attaches text to the button widget
 */
void D2Widget_Button::AttachText(char16_t* szText)
{
	D2Lib::qstrncpyz(buttonTextBuffer, szText, 64);
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

	if (dwX >= x && dwX <= m_pOwner->x + x + w && dwY >= m_pOwner->y + y && dwY <= m_pOwner->y + y + h)
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

	if (dwX >= x && dwX <= m_pOwner->x + x + w && dwY >= m_pOwner->y + y && dwY <= m_pOwner->y + y + h)
	{
		if (bWasDown)
		{
			if (bHasClickSignal)
			{
				m_pOwner->NotifySignalReady(clickSignal, this);
			}

			engine->S_PlaySound(clickedSound, 0);
			return true;
		}
	}
	return false;
}