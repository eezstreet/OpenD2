#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"
#include "D2Widget_TextEntry.hpp"

class D2Panel_TCPIPJoin : public D2Panel
{
private:
	tex_handle panelBackground;
	char16_t* ipText;

	D2Widget_Button* m_okButton;
	D2Widget_Button* m_cancelButton;
	D2Widget_TextEntry* m_ipEntry;
public:
	D2Panel_TCPIPJoin();
	virtual ~D2Panel_TCPIPJoin();
	virtual void Draw();
	char16_t* GetEnteredIP();
};