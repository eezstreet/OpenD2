#pragma once
#include "../D2Panel.hpp"
#include "../Widgets/Button.hpp"

namespace D2Panels
{
	class Debug : public D2Panel
	{
	private:
		D2Widgets::Button* m_loadEncampmentButton;

		D2Widgets::Button* m_exitButton;

	public:
		Debug();
		virtual ~Debug();

		virtual void Draw();
	};
}