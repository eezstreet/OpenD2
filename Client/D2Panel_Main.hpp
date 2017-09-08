/////////////////////////////////////////////////////
//
//	Main Menu Panel
//	Contains all of the buttons on the main menu

#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"

class D2Panel_Main : public D2Panel
{
private:
	D2Widget_Button* m_singleplayerButton;
	D2Widget_Button* m_battleNetButton;
	D2Widget_Button* m_gatewayButton;
	D2Widget_Button* m_multiplayerButton;
	D2Widget_Button* m_creditsButton;
	D2Widget_Button* m_cinematicsButton;
	D2Widget_Button* m_exitButton;

public:
	D2Panel_Main();
	~D2Panel_Main();

	virtual void Draw();
};