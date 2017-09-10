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
	void AddWidget(D2Widget* pWidget, bool bShow = true);
	void ShowWidget(D2Widget* pWidget);
	void HideWidget(D2Widget* pWidget);
	void ShowAllWidgets();
	void HideAllWidgets();
	void DrawAllWidgets();

	D2Panel* GetNext() { return m_pNextPanel; }
	D2Panel* GetNextVisible() { return m_pNextVisible; }
	D2Menu* GetOwner() { return m_pOwner; }

	// Virtual methods
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
	virtual bool HandleKeyUp(DWORD dwKey);
	virtual bool HandleKeyDown(DWORD dwKey);
	virtual bool HandleTextInput(char* szText);
	virtual bool HandleTextEditing(char* szText, int nStart, int nLength);
	virtual void Draw() = 0;

	// Signals
	void NotifySignalReady(MenuSignal pSignal, D2Widget* pCallingWidget);

	int x, y;

	~D2Panel();

	friend class D2Menu;
};