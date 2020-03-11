#include "D2Widget_Button.hpp"
#include "../../Shared/D2Shared.hpp"

/*
 *	Creates a new button widget item
 */
D2Widget_Button::D2Widget_Button(int x, int y, char* szDC6Path, char* szButtonType,
	DWORD dwStartEnabled, DWORD dwEndEnabled,
	DWORD dwStartDown, DWORD dwEndDown,
	DWORD dwStartDisabled, DWORD dwEndDisabled)
	: D2Widget(x, y, 0, 0)
{
	IGraphicsHandle* graphic = engine->graphics->LoadGraphic(szDC6Path,
			UsagePolicy_Permanent);

	backgroundObjectUp = engine->renderer->AllocateObject(0);
	backgroundObjectUp->AttachCompositeTextureResource(graphic, dwStartEnabled,
			dwEndEnabled);
	backgroundObjectUp->SetDrawCoords(x, y, -1, -1);

	backgroundObjectDown = engine->renderer->AllocateObject(0);
	backgroundObjectDown->AttachCompositeTextureResource(graphic, dwStartDown,
			dwEndDown);
	backgroundObjectDown->SetDrawCoords(x, y, -1, -1);

	backgroundObjectDisabled = engine->renderer->AllocateObject(0);
	backgroundObjectDisabled->AttachCompositeTextureResource(graphic,
			dwStartDisabled, dwEndDisabled);
	backgroundObjectDisabled->SetDrawCoords(x, y, -1, -1);
	backgroundObjectDisabled->SetColorModulate(1.0f, 1.0f, 1.0f, 0.5f);

	text = engine->renderer->AllocateObject(1);

	font = cl.fontExocet10;

	clickedSound = engine->S_RegisterSound("data\\global\\sfx\\cursor\\button.wav");

	bDisabled = false;
	bDown = false;
	bHasClickSignal = false;
	bMovedText = false;

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
	engine->renderer->Remove(text);
	engine->renderer->Remove(backgroundObjectDisabled);
	engine->renderer->Remove(backgroundObjectDown);
	engine->renderer->Remove(backgroundObjectUp);
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
		if (!bMovedText && bHasText)
		{
			int x, y;

			text->GetDrawCoords(&x, &y, nullptr, nullptr);
			text->SetDrawCoords(x - 2, y + 2, 0, 0);
			bMovedText = true;
		}
		backgroundObjectDown->Draw();
	}
	else
	{
		if (bMovedText && bHasText)
		{
			int x, y;

			text->GetDrawCoords(&x, &y, nullptr, nullptr);
			text->SetDrawCoords(x + 2, y - 2, 0, 0);
			bMovedText = false;
		}
		backgroundObjectUp->Draw();
	}

	if (bHasText)
	{
		text->Draw();
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

	text->AttachFontResource(font);
	text->SetText(szText);
	text->SetDrawMode(4);

	int32_t width, height;
	text->GetDrawCoords(nullptr, nullptr, &width, &height);
	text->SetDrawCoords(m_pOwner->x + (x + (w/2)) - (width/2), m_pOwner->y + y + (h/2) - (height/2), 0, 0);
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
