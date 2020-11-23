#pragma once
#include "../D2Panel.hpp"

namespace D2Panels
{
	class TCPIPMain : public D2Panel
	{
	private:
		D2Widgets::Button* m_hostGameButton;
		D2Widgets::Button* m_joinGameButton;
		D2Widgets::Button* m_cancelButton;

	public:
		TCPIPMain();
		virtual ~TCPIPMain();
		virtual void Draw();

		void EnableButtons(bool bEnable);
	};
}