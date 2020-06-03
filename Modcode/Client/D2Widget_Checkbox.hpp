#pragma once
#include "D2Widget.hpp"

class D2Widget_Checkbox : public D2Widget
{
private:
	tex_handle checkboxTex;
	anim_handle checkboxAnim;

	char16_t szLabel[32];
	bool m_bHasLabel;

	bool m_bChecked;
public:
	D2Widget_Checkbox(int x, int y, bool bStartChecked);
	~D2Widget_Checkbox() override;

	bool IsChecked() const { return m_bChecked; }

	void AttachLabel(char16_t* szText);
	void RemoveLabel();

	bool HandleMouseDown(DWORD dwX, DWORD dwY) override;
	bool HandleMouseClick(DWORD dwX, DWORD dwY) override;
	void Draw() override;
};
