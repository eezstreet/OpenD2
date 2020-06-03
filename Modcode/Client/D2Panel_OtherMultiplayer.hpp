#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"

class D2Panel_OtherMultiplayer : public D2Panel
{
private:
	D2Widget_Button* m_openBattleNetButton;
	D2Widget_Button* m_TCPIPButton;
	D2Widget_Button* m_cancelButton;

	static void PanelSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget);

public:
	D2Panel_OtherMultiplayer();
	~D2Panel_OtherMultiplayer() override;

	void Draw() override;
};
