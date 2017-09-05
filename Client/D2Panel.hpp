#pragma once
#include "D2Client.hpp"
#include "D2Widget.hpp"

/*
 *	Panels are a subsection of UI.
 *	They contain one or more widgets and are part of a menu.
 *	They can be hidden or removed entirely when they aren't needed.
 */
class D2Panel
{
private:
	D2Menu* m_pOwner;

	D2Panel* m_pNextPanel;
	D2Panel* m_pNextVisible;

	D2Widget* m_widgets;
	D2Widget* m_visibleWidgets;

protected:
	bool m_bVisible;

	void DrawWidgets();

public:
	bool IsVisible() { return m_bVisible; }
	void Show() { m_bVisible = true; }
	void Hide() { m_bVisible = false; }

	// Widget management
	void AddWidget(D2Widget* pWidget);
	void ShowWidget(D2Widget* pWidget);
	void HideWidget(D2Widget* pWidget);
	void ShowAllWidgets();
	void HideAllWidgets();

	D2Panel* GetNext() { return m_pNextPanel; }
	D2Panel* GetNextVisible() { return m_pNextVisible; }
	D2Menu* GetOwner() { return m_pOwner; }

	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY) = 0;
	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY) = 0;
	virtual void Draw() = 0;

	int x, y;

	friend class D2Menu;
};