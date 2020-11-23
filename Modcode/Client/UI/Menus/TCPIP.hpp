#pragma once
#include "../D2Menu.hpp"
#include "../Panels/TCPIPJoin.hpp"
#include "../Panels/TCPIPMain.hpp"

namespace D2Menus
{
	class TCPIP : public D2Menu
	{
	private:
		tex_handle backgroundTexture;

		char16_t* m_yourIPString;
		char16_t* m_yourIP;

		D2Panels::TCPIPJoin* m_joinMenu;
		D2Panels::TCPIPMain* m_mainMenu;
	public:
		TCPIP();
		virtual ~TCPIP();
		virtual void Draw();

		void ShowJoinSubmenu(bool bShow);
	};
}