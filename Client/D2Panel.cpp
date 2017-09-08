#include "D2Panel.hpp"

/*
 *	Draws all of the widgets for this panel.
 */
void D2Panel::DrawWidgets()
{
	D2Widget* pCurrent = m_visibleWidgets;

	while (pCurrent != nullptr)
	{
		pCurrent->Draw();
		pCurrent = pCurrent->m_pNextVisibleWidget;
	}
}

/*
 *	Add a widget to the panel
 */
void D2Panel::AddWidget(D2Widget* pWidget, bool bShow)
{
	D2Widget* pCurrent;

	Log_WarnAssert(pWidget);
	if (pWidget->m_pOwner == this)
	{	// we already own it
		return;
	}

	pWidget->m_pOwner = this;
	pWidget->x += x;
	pWidget->y += y;

	// Add it to the list of widgets
	if (m_widgets == nullptr)
	{
		m_widgets = pWidget;
	}
	else
	{
		pCurrent = m_widgets;
		while (pCurrent->m_pNextWidget != nullptr)
		{
			pCurrent = pCurrent->m_pNextWidget;
		}
		pCurrent->m_pNextWidget = pWidget;
	}

	if (bShow)
	{
		ShowWidget(pWidget);
	}
}

/*
 *	Show a specific widget.
 */
void D2Panel::ShowWidget(D2Widget* pWidget)
{
	// Don't show it if it's already visible, it might fire off unwanted events
	if (pWidget->IsVisible())
	{
		return;
	}

	// Show it, firing off its events
	pWidget->Show();

	// Most recently shown widget is shown on the bottom.
	pWidget->m_pNextVisibleWidget = m_visibleWidgets;
	m_visibleWidgets = pWidget;
}

/*
 *	Hides a specific widget
 */
void D2Panel::HideWidget(D2Widget* pWidget)
{
	D2Widget* pCurrent;
	D2Widget* pLast = nullptr;

	// Don't hide it if it's already invisible, this might trigger unwanted things
	if (!pWidget->IsVisible())
	{
		return;
	}

	pWidget->Hide();

	// Remove it from the list of visible panels
	pCurrent = m_visibleWidgets;
	if (pWidget == m_visibleWidgets)
	{
		m_visibleWidgets = pWidget->m_pNextVisibleWidget;
		pWidget->m_pNextVisibleWidget = nullptr;
		return;
	}

	while (pCurrent != nullptr && pCurrent != pWidget)
	{
		pLast = pCurrent;
		pCurrent = pCurrent->m_pNextVisibleWidget;
	}

	pLast->m_pNextVisibleWidget = pCurrent->m_pNextVisibleWidget;
}

/*
 *	Show all of the widgets.
 */
void D2Panel::ShowAllWidgets()
{
	D2Widget* pCurrent;
	D2Widget* pLast = nullptr;

	pCurrent = m_widgets;
	while (pCurrent != nullptr)
	{
		pCurrent->Show();

		if (pLast != nullptr)
		{
			pLast->m_pNextVisibleWidget = pCurrent;
		}
		pLast = pCurrent;
		pCurrent = pCurrent->m_pNextWidget;
	}

	m_visibleWidgets = m_widgets;
}

/*
 *	Hides all of the widgets.
 */
void D2Panel::HideAllWidgets()
{
	D2Widget* pCurrent;
	D2Widget* pLast = nullptr;

	pCurrent = m_visibleWidgets;
	while (pCurrent != nullptr)
	{
		// Hide it
		pCurrent->Hide();

		if (pLast != nullptr)
		{
			pLast->m_pNextVisibleWidget = nullptr;
		}

		pLast = pCurrent;
		pCurrent = pCurrent->m_pNextVisibleWidget;
	}

	if (pLast != nullptr)
	{
		pLast->m_pNextVisibleWidget = nullptr;
	}
	m_visibleWidgets = nullptr;
}

/*
 *	Draws all of the widgets
 */
void D2Panel::DrawAllWidgets()
{
	D2Widget* pCurrent;

	pCurrent = m_visibleWidgets;
	while (pCurrent != nullptr)
	{
		pCurrent->Draw();
		pCurrent = pCurrent->m_pNextVisibleWidget;
	}
}

/*
 *	Handle a mouse down in the vicinity
 */
bool D2Panel::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	D2Widget* pCurrent;

	pCurrent = m_visibleWidgets;
	while (pCurrent != nullptr)
	{
		if (pCurrent->HandleMouseDown(dwX, dwY))
		{
			return true;
		}
		pCurrent = pCurrent->m_pNextVisibleWidget;
	}
	return false;
}

/*
 *	Handle a mouse click in the vicinity
 */
bool D2Panel::HandleMouseClicked(DWORD dwX, DWORD dwY)
{
	D2Widget* pCurrent;

	pCurrent = m_visibleWidgets;
	while (pCurrent != nullptr)
	{
		if (pCurrent->HandleMouseClick(dwX, dwY))
		{
			return true;
		}
		pCurrent = pCurrent->m_pNextVisibleWidget;
	}
	return false;
}