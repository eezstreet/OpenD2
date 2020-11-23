/////////////////////////////////////////////////////
//
//	Main Menu Panel
//	Contains all of the buttons on the main menu

#pragma once
#include "../D2Panel.hpp"
#include "../Widgets/Button.hpp"

namespace D2Panels
{
	class Main : public D2Panel
	{
	private:
		D2Widgets::Button* m_singleplayerButton;
		D2Widgets::Button* m_battleNetButton;
		D2Widgets::Button* m_gatewayButton;
		D2Widgets::Button* m_multiplayerButton;
		D2Widgets::Button* m_creditsButton;
		D2Widgets::Button* m_cinematicsButton;
		D2Widgets::Button* m_exitButton;

#ifdef _DEBUG
		D2Widgets::Button* m_debugMapButton;
#endif

	public:
		Main();
		virtual ~Main();

		virtual void Draw();
	};
}