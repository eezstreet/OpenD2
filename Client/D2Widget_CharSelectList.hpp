#pragma once
#include "D2Widget.hpp"
#include "D2Widget_Scrollbar.hpp"

class D2Widget_CharSelectList : public D2Widget
{
private:
	D2Widget_Scrollbar*		pScrollBar;

public:
	D2Widget_CharSelectList(int x, int y, int w, int h);

	virtual void Draw();
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);
};