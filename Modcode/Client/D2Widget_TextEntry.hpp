#pragma once
#include "D2Widget.hpp"

#define MAX_TEXTENTRY_LEN		32
#define MAX_TEXTENTRY_LEN_CHARSELECT	16

class D2Widget_TextEntry : public D2Widget
{
private:
	bool m_bHasFocus;
	bool m_bAlwaysFocus;
	bool m_bCharSelect;
	char16_t szTextBuffer[MAX_TEXTENTRY_LEN];
	size_t m_nCursorPos;
	bool m_bOverstrikeMode;

	tex_handle backgroundTexture;
	anim_handle backgroundAnim;
	
	bool m_bHasAttachedText;
	char16_t szLabel[32];
public:
	D2Widget_TextEntry(int x, int y, bool bStartFocus, bool bAlwaysFocus, bool bCharSelect, bool bIP);
	~D2Widget_TextEntry() override;

	void AttachLabel(char16_t* szText);
	void DetachLabel();
	char16_t* GetText() { return szTextBuffer; }
	size_t GetTextLength() { return D2Lib::qstrlen(szTextBuffer); }

	bool HandleMouseDown(DWORD dwX, DWORD dwY) override;
	bool HandleMouseClick(DWORD dwX, DWORD dwY) override;
	bool HandleTextInput(char* szText) override;
	bool HandleTextEditing(char* szText, int nStart, int nLength) override;
	bool HandleKeyDown(DWORD dwKey) override;
	bool HandleKeyUp(DWORD dwKey) override;
	void Draw() override;
};
