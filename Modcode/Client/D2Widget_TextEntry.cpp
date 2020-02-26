#include "D2Widget_TextEntry.hpp"

/*
 *	Creates the text entry widget
 *	@author	eezstreet
 */
D2Widget_TextEntry::D2Widget_TextEntry(int _x, int _y, bool bStartFocus, bool bAlwaysFocus, bool bCharSelect, bool bIP) : D2Widget()
{
#if 0
	x = _x;
	y = _y;

	m_bHasFocus = bStartFocus;
	m_bAlwaysFocus = bAlwaysFocus;
	m_bCharSelect = bCharSelect;
	m_nCursorPos = 0;
	m_bOverstrikeMode = false;
	m_bHasAttachedText = false;
	memset(szTextBuffer, 0, MAX_TEXTENTRY_LEN);

	if (bCharSelect)
	{
		backgroundTexture = 
			engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\textbox.dc6", "textbox", PAL_FECHAR);
		backgroundAnim = engine->renderer->RegisterDC6Animation(backgroundTexture, "textbox", 0);
	}
	else if (bIP)
	{
		backgroundTexture = engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\IPAddressBox.dc6", "ipaddressbox", PAL_UNITS);
		backgroundAnim = engine->renderer->RegisterDC6Animation(backgroundTexture, "ipaddressbox", 0);
	}
	else
	{
		backgroundTexture = 
			engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\textbox2.dc6", "textbox2", PAL_UNITS);
		backgroundAnim = engine->renderer->RegisterDC6Animation(backgroundTexture, "textbox2", 0);
	}

	engine->renderer->PollTexture(backgroundTexture, (DWORD*)&this->w, (DWORD*)&this->h);
#endif
}

/*
 *	Destroys the text entry widget
 *	@author	eezstreet
 */
D2Widget_TextEntry::~D2Widget_TextEntry()
{
#if 0
	engine->renderer->DeregisterTexture(nullptr, backgroundTexture);
#endif
}

/*
 *	Adds text (label) to the widget
 *	@author	eezstreet
 */
void D2Widget_TextEntry::AttachLabel(char16_t* szText)
{
	D2Lib::qstrncpyz(szLabel, szText, 32);
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
#if 0
	// draw background
	engine->renderer->Animate(backgroundAnim, 0, m_pOwner->x + x, m_pOwner->y + y);

	// draw a label, if present
	if (m_bHasAttachedText)
	{
		engine->renderer->ColorModFont(cl.font16, 150, 135, 100);
		engine->renderer->DrawText(cl.font16, szLabel, m_pOwner->x + x, m_pOwner->y + y - 15, w, 10, ALIGN_LEFT, ALIGN_BOTTOM);
	}

	if (szTextBuffer[0] != '\0')
	{
		// draw text, if present
		engine->renderer->ColorModFont(cl.fontFormal12, 255, 255, 255);
		engine->renderer->DrawText(cl.fontFormal12, szTextBuffer, m_pOwner->x + x + 6, m_pOwner->y + y - 2, w, h, ALIGN_LEFT, ALIGN_CENTER);
	}
#endif
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

/*
 *	Handle a text input event on the text entry widget
 *	@author	eezstreet
 */
bool D2Widget_TextEntry::HandleTextInput(char* szText)
{
	int i, j;
	size_t maxLen = m_bCharSelect ? MAX_TEXTENTRY_LEN_CHARSELECT : MAX_TEXTENTRY_LEN;
	size_t textLen;
	size_t originalLen;

	if (!m_bHasFocus)
	{
		return false;
	}

	textLen = strlen(szText);

	if (m_bCharSelect)
	{	// strip characters out of the text which are illegal
		for (i = 0; i < textLen; i++)
		{
			if (szText[i] == '-' || szText[i] == '_' ||
				(szText[i] >= '0' && szText[i] <= '9') ||
				(szText[i] >= 'A' && szText[i] <= 'Z') ||
				(szText[i] >= 'a' && szText[i] <= 'z'))
			{	// legal character
				continue;
			}
			// shift all characters back
			for (j = i; j < textLen; j++)
			{
				szText[j] = szText[j + 1];
			}
			textLen--;
		}
	}

	if (textLen <= 0)
	{	// no characters got inputted, but we captured the text
		return true;
	}

	originalLen = D2Lib::qstrlen(szTextBuffer);

	if (m_bOverstrikeMode)
	{
		// in overstrike mode, we physically copy over the text
		i = 0;
		while (m_nCursorPos < maxLen && i < textLen)
		{
			szTextBuffer[m_nCursorPos++] = szText[i++];
		}
	}
	else
	{
		// shift string to the right
		for (i = originalLen - m_nCursorPos; i > m_nCursorPos; i--)
		{
			if (i >= maxLen)
			{
				continue;
			}
			szTextBuffer[i] = szTextBuffer[i - textLen];
		}

		// insert characters at cursor
		for (i = 0; i < textLen && m_nCursorPos + i < maxLen; i++)
		{
			szTextBuffer[m_nCursorPos + i] = szText[i];
		}

		// increment text cursor
		m_nCursorPos += textLen;
		if (m_nCursorPos >= maxLen - 1)
		{
			m_nCursorPos = maxLen - 1;
		}
	}

	szTextBuffer[maxLen - 1] = '\0';
	return true;
}

/*
 *	Handle a text editing event on the text entry widget
 *	@author	eezstreet
 */
bool D2Widget_TextEntry::HandleTextEditing(char* szText, int nStart, int nLength)
{
	if (!m_bHasFocus)
	{
		return false;
	}
	return true;
}

/*
*	Handles a key down event
*	@author	eezstreet
*/
bool D2Widget_TextEntry::HandleKeyDown(DWORD dwKey)
{
	int i;
	bool bBackspace = false;

	if (!m_bHasFocus)
	{
		return false;
	}

	switch (dwKey)
	{
		case B_HOME:
			m_nCursorPos = 0;
			break;
		case B_END:
			m_nCursorPos = D2Lib::qstrlen(szTextBuffer);
			break;
		case B_LEFTARROW:
			if (m_nCursorPos > 0)
			{
				m_nCursorPos--;
			}
			break;
		case B_RIGHTARROW:
			if (m_nCursorPos < D2Lib::qstrlen(szTextBuffer))
			{
				m_nCursorPos++;
			}
			break;
		case B_BACKSPACE:
			if (m_nCursorPos > 0)
			{
				// shift whole buffer left
				i = m_nCursorPos - 1;
				while (szTextBuffer[i] != '\0')
				{
					szTextBuffer[i] = szTextBuffer[i + 1];
					i++;
				}
				m_nCursorPos--;
			}
			break;
		case B_DELETE:
			// shift whole buffer left
			i = m_nCursorPos;
			if (szTextBuffer[i] == '\0')
			{
				i = m_nCursorPos - 1;
				bBackspace = true;
			}
			while (i >= 0 && szTextBuffer[i] != '\0')
			{
				szTextBuffer[i] = szTextBuffer[i + 1];
				i++;
			}
			if (bBackspace)
			{
				m_nCursorPos--;
			}
			break;
		case B_INSERT:
			m_bOverstrikeMode = !m_bOverstrikeMode;
			break;

	}

	return true;
}

/*
*	Handles a key up event
*	@author	eezstreet
*/
bool D2Widget_TextEntry::HandleKeyUp(DWORD dwKey)
{
	if (!m_bHasFocus)
	{
		return false;
	}
	return true;
}