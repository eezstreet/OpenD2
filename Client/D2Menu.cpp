#include "D2Menu.hpp"
#include "D2Panel.hpp"

/*
 *	Main constructor for the abstract D2Menu class.
 */
D2Menu::D2Menu() : m_panels(nullptr), m_visiblePanels(nullptr)
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
void D2Menu::AddPanel(D2Panel* pPanel)
{
	D2Panel* pCurrent;

	Log_WarnAssert(pPanel);

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

	// Add it to the list of visible panels
	if (pPanel->IsVisible())
	{
		if (m_visiblePanels == nullptr)
		{
			m_visiblePanels = pPanel;
		}
		else
		{
			pCurrent = m_visiblePanels;
			while (pCurrent->GetNext() != nullptr)
			{
				pCurrent = pCurrent->GetNextVisible();
			}
			pCurrent->m_pNextVisible = pPanel;
		}
	}
}

/*
 *	Draws the menu. Note that this should be subclassed for better functionality.
 */
void D2Menu::Draw()
{
	D2Panel* pCurrent = m_visiblePanels;

	while (pCurrent != nullptr)
	{
		pCurrent->DrawWidgets();
		pCurrent = pCurrent->m_pNextVisible;
	}
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