#pragma once
#include "D2Client.hpp"

/*
 *	In OpenD2, the decision was made to go with an object-oriented approach to menus.
 *
 *	There are three levels of hierarchy here:
 *	1. Menus: Only one is ever visible at a time. Contains one or more panels.
 *	2. Panels: Anchored to a point on the screen, they contain widgets in a layout.
 *	3. Widgets: The most basic unit of interaction. These are things like buttons, etc.
 */

class D2Menu
{
protected:
	D2Panel* m_panels;
	D2Panel* m_visiblePanels;

	D2Menu();

public:
	void AddPanel(D2Panel* pPanel);
	bool HandleMouseDown(DWORD dwX, DWORD dwY);
	bool HandleMouseClicked(DWORD dwX, DWORD dwY);
	virtual void Draw() = 0;

	~D2Menu();
};