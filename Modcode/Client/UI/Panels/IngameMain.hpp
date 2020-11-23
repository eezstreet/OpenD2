#pragma once
#include "../D2Panel.hpp"
#include "../Widgets/Button.hpp"

class IngameMain : public D2Panel
{
private:
	D2Widgets::Button*	m_newStatButton;
	D2Widgets::Button*	m_newSkillButton;
	D2Widgets::Button*	m_questLogButton;
	D2Widgets::Button*	m_miniPanelButton;
	D2Widgets::Button*	m_leftAttackButton;
	D2Widgets::Button*	m_rightAttackButton;
};