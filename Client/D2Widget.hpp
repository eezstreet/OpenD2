#pragma once
#include "D2Panel.hpp"

class D2Widget
{
private:
	// relative to panel
	D2Panel* pParent;
	int x, y;
	int w, h;

	D2Widget* m_pNextWidget;
	D2Widget* m_pNextVisibleWidget;

	bool m_bVisible;

public:
	D2Widget(int x, int y, int w, int h, D2Panel* pParent);
	~D2Widget();

	void Show() { m_bVisible = true; }
	void Hide() { m_bVisible = false; }

	D2Widget* GetNextWidget() { return m_pNextWidget; }
	D2Widget* GetNextVisible() { return m_pNextVisibleWidget; }

	virtual void Draw() = 0;
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY) = 0;
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY) = 0;

	friend class D2Panel;
};
