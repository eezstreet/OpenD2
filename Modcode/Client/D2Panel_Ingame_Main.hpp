#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"

class D2Panel_Ingame_Main : public D2Panel
{
private:
	D2Widget_Button*	m_newStatButton;
	D2Widget_Button*	m_newSkillButton;
	D2Widget_Button*	m_questLogButton;
	D2Widget_Button*	m_miniPanelButton;
	D2Widget_Button*	m_leftAttackButton;
	D2Widget_Button*	m_rightAttackButton;
};
