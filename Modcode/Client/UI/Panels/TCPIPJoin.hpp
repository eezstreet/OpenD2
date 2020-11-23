#pragma once
#include "../D2Panel.hpp"

namespace D2Panels
{
	class TCPIPJoin : public D2Panel
	{
	private:
		tex_handle panelBackground;
		char16_t* ipText;

		D2Widgets::Button* m_okButton;
		D2Widgets::Button* m_cancelButton;
		D2Widgets::TextEntry* m_ipEntry;
	public:
		TCPIPJoin();
		virtual ~TCPIPJoin();
		virtual void Draw();
		char16_t* GetEnteredIP();
	};
}