#include "Button.hpp"

namespace D2Widgets
{
	/*
	 *	Creates a new button widget item
	 */
	Button::Button(int x, int y, char* szDC6Path, char* szButtonType,
		DWORD dwStartEnabled, DWORD dwEndEnabled,
		DWORD dwStartDown, DWORD dwEndDown,
		DWORD dwStartDisabled, DWORD dwEndDisabled)
		: D2Widget(x, y, 0, 0)
	{
		IGraphicsReference* graphic = engine->graphics->CreateReference(szDC6Path,
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
	Button::~Button()
	{
		engine->renderer->Remove(text);
		engine->renderer->Remove(backgroundObjectDisabled);
		engine->renderer->Remove(backgroundObjectDown);
		engine->renderer->Remove(backgroundObjectUp);
	}

	/*
	 *	Draws the button widget
	 */
	void Button::Draw()
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
	}

	/*
	 *	Attaches text to the button widget
	 */
	void Button::AttachText(char16_t* szText)
	{
		D2Lib::qstrncpyz(buttonTextBuffer, szText, 64);
		bHasText = true;

		text->AttachFontResource(font);
		text->SetText(szText);
		text->SetDrawMode(4);

		int32_t width, height;
		text->GetDrawCoords(nullptr, nullptr, &width, &height);
		text->SetDrawCoords(m_pOwner->x + (x + (w / 2)) - (width / 2), m_pOwner->y + y + (h / 2) - (height / 2), 0, 0);
	}

	/*
	 *	Removes text from the button widget
	 */
	void Button::DetachText()
	{
		bHasText = false;
	}

	/*
	 *	Handles a mouse down
	 */
	bool Button::HandleMouseDown(DWORD dwX, DWORD dwY)
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
	bool Button::HandleMouseClick(DWORD dwX, DWORD dwY)
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
				OnClicked.Dispatch();

				engine->S_PlaySound(clickedSound, 0);
				return true;
			}
		}
		return false;
	}

	/**
	 * Adds a callback to this button.
	 */
	handle Button::AddEventListener(ButtonWidgetClickEvent eventType, void (*function)())
	{
		switch (eventType)
		{
		case Clicked:
			return OnClicked.AddListener((void (*)())function);
		case Down:
			return OnDown.AddListener((void (*)())function);
		case Up:
			return OnUp.AddListener((void (*)())function);
		default:
			return INVALID_HANDLE;
		}
	}

	/**
	 * Removes a callback from this button.
	 */
	void Button::RemoveEventListener(ButtonWidgetClickEvent eventType, handle eventHandle)
	{
		switch (eventType)
		{
		case Clicked:
			OnClicked.RemoveListener(eventHandle);
			break;
		case Down:
			OnDown.RemoveListener(eventHandle);
			break;
		case Up:
			OnUp.RemoveListener(eventHandle);
			break;
		}
	}
}