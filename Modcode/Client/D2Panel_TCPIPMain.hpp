#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"

class D2Panel_TCPIPMain : public D2Panel
{
private:
	static void PanelSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget);

	D2Widget_Button* m_hostGameButton;
	D2Widget_Button* m_joinGameButton;
	D2Widget_Button* m_cancelButton;

public:
	D2Panel_TCPIPMain();
	~D2Panel_TCPIPMain() override;
	void Draw() override;

	void EnableButtons(bool bEnable);
};
