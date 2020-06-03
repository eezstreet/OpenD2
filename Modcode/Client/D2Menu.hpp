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

	void DrawAllPanels();

	bool bSignalReady;
	MenuSignal m_pWaitingSignal;
	D2Panel* m_pSignalCallingPanel;
	D2Widget* m_pSignalCallingWidget;

public:
	// Panel management
	void AddPanel(D2Panel* pPanel, bool bVisible = true);
	void HidePanel(D2Panel* pPanel);
	void ShowPanel(D2Panel* pPanel);
	void HideAllPanels();
	void ShowAllPanels();

	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
	virtual bool HandleKeyUp(DWORD keyButton);
	virtual bool HandleKeyDown(DWORD keyButton);
	virtual void HandleTextInput(char* szText);
	virtual void HandleTextEditing(char* szText, int nStart, int nLength);
	virtual void Draw() = 0;

	void RefreshInputFrame() { bSignalReady = false; }
	void NotifySignalReady(MenuSignal signal, D2Panel* pCallingPanel, D2Widget* pCallingWidget);
	virtual bool WaitingSignal() { return bSignalReady; }
	MenuSignal GetWaitingSignal(D2Panel** pCallingPanel, D2Widget** pCallingWidget);

	static void ProcessMenuSignals(D2Menu* pMenu);

	D2Menu();
	virtual ~D2Menu();
};
