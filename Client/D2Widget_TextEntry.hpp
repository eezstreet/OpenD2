#pragma once
#include "D2Widget.hpp"

class D2Widget_TextEntry : public D2Widget
{
private:
	bool m_bHasFocus;
	bool m_bAlwaysFocus;
	char16_t szTextBuffer[32];

	tex_handle backgroundTexture;
	anim_handle backgroundAnim;
	
	bool m_bHasAttachedText;
	char16_t szLabel[32];
public:
	D2Widget_TextEntry(int x, int y, bool bStartFocus, bool bAlwaysFocus, bool bCharSelect);
	~D2Widget_TextEntry();

	void AttachLabel(char16_t* szText);
	void DetachLabel();
	char16_t* GetText() { return szTextBuffer; }

	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);
	virtual void Draw();
};