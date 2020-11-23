#pragma once
#include "../D2Panel.hpp"

namespace D2Panels
{
	class OtherMultiplayer : public D2Panel
	{
	private:
		D2Widgets::Button* m_openBattleNetButton;
		D2Widgets::Button* m_TCPIPButton;
		D2Widgets::Button* m_cancelButton;

	public:
		OtherMultiplayer();
		virtual ~OtherMultiplayer();

		virtual void Draw();
	};
}