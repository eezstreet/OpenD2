#pragma once
#include "D2Client.hpp"

/*
 *	Panels are a subsection of UI.
 *	They contain one or more widgets and are part of a menu.
 *	They can be hidden or removed entirely when they aren't needed.
 */
class D2Panel
{
private:
	D2Panel* m_pNextPanel;
	D2Panel* m_pNextVisible;
	int m_nPanelID;
	D2Menu* m_pOwner;

protected:
	bool m_bVisible;

public:
	bool IsVisible() { return m_bVisible; }
	D2Panel* GetNext() { return m_pNextPanel; }
	D2Panel* GetNextVisible() { return m_pNextVisible; }
	D2Menu* GetOwner() { return m_pOwner; }
	void DrawWidgets();

	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY) = 0;
	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY) = 0;

	friend class D2Menu;
};