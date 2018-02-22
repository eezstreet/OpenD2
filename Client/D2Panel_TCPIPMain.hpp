#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"

class D2Panel_TCPIPMain : public D2Panel
{
private:
	D2Widget_Button* m_hostGameButton;
	D2Widget_Button* m_joinGameButton;
	D2Widget_Button* m_cancelButton;

	static void PanelSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget);

public:
	D2Panel_TCPIPMain();
	virtual ~D2Panel_TCPIPMain();
	virtual void Draw();
};