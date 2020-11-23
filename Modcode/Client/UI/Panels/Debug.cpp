#include "Debug.hpp"
#include "../D2Menu.hpp"

#define MAIN_BUTTON_DC6			"data\\global\\ui\\FrontEnd\\3WideButtonBlank.dc6"

namespace D2Panels
{
	Debug::Debug() : D2Panel()
	{
		m_loadEncampmentButton = new D2Widgets::Button(265, 290, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);
		m_exitButton = new D2Widgets::Button(265, 535, MAIN_BUTTON_DC6, "3wide", 0, 1, 2, 3, 4, 5);

		AddWidget(m_loadEncampmentButton);
		AddWidget(m_exitButton);

		m_loadEncampmentButton->AttachText(u"ROGUE ENCAMPMENT");
		m_exitButton->AttachText(u"EXIT");
		m_exitButton->AddEventListener(Clicked, [] {
			delete cl.pActiveMenu;
			D2Client_GoToContextMenu();
			return;
			});
	}

	Debug::~Debug()
	{
		delete m_loadEncampmentButton;
		delete m_exitButton;
	}

	void Debug::Draw()
	{
		DrawAllWidgets();
	}
}