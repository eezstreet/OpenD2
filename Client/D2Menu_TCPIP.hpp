#pragma once
#include "D2Menu.hpp"
#include "D2Panel_TCPIPJoin.hpp"
#include "D2Panel_TCPIPMain.hpp"

class D2Menu_TCPIP : public D2Menu
{
private:
	tex_handle backgroundTexture;

	char16_t m_ipStringBuffer[32];
	char16_t* m_yourIPString;

	D2Panel_TCPIPJoin* m_joinMenu;
	D2Panel_TCPIPMain* m_mainMenu;
public:
	D2Menu_TCPIP();
	virtual ~D2Menu_TCPIP();
	virtual void Draw();
};