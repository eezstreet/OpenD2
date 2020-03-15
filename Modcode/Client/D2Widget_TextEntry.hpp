#pragma once
#include "D2Widget.hpp"

#define MAX_TEXTENTRY_LEN				32
#define MAX_TEXTENTRY_LEN_CHARSELECT	16

class D2Widget_TextEntry : public D2Widget
{
private:
	bool m_bHasFocus;
	bool m_bAlwaysFocus;
	bool m_bCharSelect;
	char16_t szTextBuffer[MAX_TEXTENTRY_LEN];
	int m_nCursorPos;
	bool m_bOverstrikeMode;

	IRenderObject* background;
	IRenderObject* text;
	IRenderObject* label;

	bool m_bSetCoords;
	
	bool m_bHasAttachedText;
	char16_t szLabel[32];
public:
	D2Widget_TextEntry(int x, int y, bool bStartFocus, bool bAlwaysFocus, bool bCharSelect, bool bIP);
	virtual ~D2Widget_TextEntry();

	void AttachLabel(char16_t* szText);
	void DetachLabel();
	char16_t* GetText() { return szTextBuffer; }
	size_t GetTextLength() { return D2Lib::qstrlen(szTextBuffer); }

	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);
	virtual bool HandleTextInput(char* szText);
	virtual bool HandleTextEditing(char* szText, int nStart, int nLength);
	virtual bool HandleKeyDown(DWORD dwKey);
	virtual bool HandleKeyUp(DWORD dwKey);
	virtual void Draw();
};