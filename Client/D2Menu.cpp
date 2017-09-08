#include "D2Menu.hpp"
#include "D2Panel.hpp"

/*
 *	Main constructor for the abstract D2Menu class.
 */
D2Menu::D2Menu() : m_panels(nullptr), m_visiblePanels(nullptr), bSignalReady(false)
{

}

/*
 *	Main destructor for the abstract D2Menu class.
 */
D2Menu::~D2Menu()
{
	// Delete all of the panels
	D2Panel* pPanel = m_panels;
	D2Panel* pPrevPanel = nullptr;

	while (pPanel != nullptr)
	{
		Log_WarnAssert((pPanel->GetOwner() == this));
		delete pPrevPanel;
		pPrevPanel = pPanel;
		pPanel = pPanel->GetNext();
	}

	if (pPrevPanel != nullptr)
	{
		delete pPrevPanel;
	}
}

/*
 *	Add a panel to the menu
 */
void D2Menu::AddPanel(D2Panel* pPanel, bool bVisible)
{
	D2Panel* pCurrent;

	Log_WarnAssert(pPanel);

	if (pPanel->m_pOwner == this)
	{	// already own it
		return;
	}

	pPanel->m_pOwner = this;

	// Add it to the list of panels
	if (m_panels == nullptr)
	{
		m_panels = pPanel;
	}
	else
	{
		pCurrent = m_panels;
		while (pCurrent->GetNext() != nullptr)
		{
			pCurrent = pCurrent->GetNext();
		}
		pCurrent->m_pNextPanel = pPanel;
	}

	if (bVisible)
	{
		ShowPanel(pPanel);
	}
}

/*
*	Show a specific panel.
*/
void D2Menu::ShowPanel(D2Panel* pPanel)
{
	// Don't show it if it's already visible, it might fire off unwanted events
	if (pPanel->IsVisible())
	{
		return;
	}

	// Show it, firing off its events
	pPanel->Show();

	// Most recently shown panel is shown on the bottom.
	pPanel->m_pNextVisible = m_visiblePanels;
	m_visiblePanels = pPanel;
}

/*
 *	Hides a specific panel
 */
void D2Menu::HidePanel(D2Panel* pPanel)
{
	D2Panel* pCurrent;
	D2Panel* pLast = nullptr;

	// Don't hide it if it's already invisible, this might trigger unwanted things
	if (!pPanel->IsVisible())
	{
		return;
	}

	pPanel->Hide();

	// Remove it from the list of visible panels
	pCurrent = m_visiblePanels;
	if (pPanel == m_visiblePanels)
	{
		m_visiblePanels = pPanel->m_pNextVisible;
		pPanel->m_pNextVisible = nullptr;
		return;
	}

	while (pCurrent != nullptr && pCurrent != pPanel)
	{
		pLast = pCurrent;
		pCurrent = pCurrent->m_pNextVisible;
	}

	pLast->m_pNextVisible = pCurrent->m_pNextVisible;
}

/*
*	Show all of the panels.
*/
void D2Menu::ShowAllPanels()
{
	D2Panel* pCurrent;
	D2Panel* pLast = nullptr;

	pCurrent = m_panels;
	while (pCurrent != nullptr)
	{
		pCurrent->Show();

		if (pLast != nullptr)
		{
			pLast->m_pNextVisible = pCurrent;
		}
		pLast = pCurrent;
		pCurrent = pCurrent->m_pNextPanel;
	}

	m_visiblePanels = m_panels;
}

/*
 *	Hides all of the panels.
 */
void D2Menu::HideAllPanels()
{
	D2Panel* pCurrent;
	D2Panel* pLast = nullptr;

	pCurrent = m_visiblePanels;
	while (pCurrent != nullptr)
	{
		// Hide it
		pCurrent->Hide();

		if (pLast != nullptr)
		{
			pLast->m_pNextVisible = nullptr;
		}

		pLast = pCurrent;
		pCurrent = pCurrent->m_pNextVisible;
	}

	if (pLast != nullptr)
	{
		pLast->m_pNextVisible = nullptr;
	}
	m_visiblePanels = nullptr;
}

/*
 *	Draws all of the panels for the menu.
 */
void D2Menu::DrawAllPanels()
{
	D2Panel* pCurrent = m_visiblePanels;

	while (pCurrent != nullptr)
	{
		pCurrent->Draw();
		pCurrent = pCurrent->m_pNextVisible;
	}
}

/*
 *	Notify the menu that there is a signal waiting to be processed.
 */
void D2Menu::NotifySignalReady(MenuSignal pSignal, D2Panel* pCallingPanel, D2Widget* pCallingWidget)
{
	bSignalReady = true;
	m_pWaitingSignal = pSignal;
	m_pSignalCallingPanel = pCallingPanel;
	m_pSignalCallingWidget = pCallingWidget;
}

/*
 *	Get any signal on this menu that's waiting to be executed.
 *	This should be performed every frame on the active menu.
 */
MenuSignal D2Menu::GetWaitingSignal(D2Panel** pCallingPanel, D2Widget** pCallingWidget)
{
	*pCallingPanel = m_pSignalCallingPanel;
	*pCallingWidget = m_pSignalCallingWidget;
	return m_pWaitingSignal;
}

/*
 *	Handle all waiting signals on the menu.
 *	NOTE: static method
 */
void D2Menu::ProcessMenuSignals(D2Menu* pMenu)
{
	D2Panel* pCallingPanel = nullptr;
	D2Widget* pCallingWidget = nullptr;
	MenuSignal signal;

	signal = pMenu->GetWaitingSignal(&pCallingPanel, &pCallingWidget);

	signal(pCallingPanel, pCallingWidget);
}

/*
 *	Handle a mouse down event
 */
bool D2Menu::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	D2Panel* pPanel = m_visiblePanels;
	while (pPanel != nullptr)
	{
		if (pPanel->HandleMouseDown(dwX, dwY))
		{
			return true;
		}
		pPanel = pPanel->GetNextVisible();
	}
	return false;
}

/*
 *	Handle a mouse up (click) event
 */
bool D2Menu::HandleMouseClicked(DWORD dwX, DWORD dwY)
{
	D2Panel* pPanel = m_visiblePanels;
	while (pPanel != nullptr)
	{
		if (pPanel->HandleMouseClicked(dwX, dwY))
		{
			return true;
		}
		pPanel = pPanel->GetNextVisible();
	}
	return false;
}