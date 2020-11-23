#pragma once
#include "../D2Menu.hpp"

namespace D2Menus
{
	class Trademark : public D2Menu
	{
	private:
		char16_t* szCopyrightText;
		char16_t* szAllRightsReservedText;
		int m_timeRemaining;

		IRenderObject* backgroundObject;
		IRenderObject* flameLeft;
		IRenderObject* flameRight;
		IRenderObject* blackLeft;
		IRenderObject* blackRight;
		IRenderObject* trademark;
		IRenderObject* allRightsReserved;

		IGraphicsReference* background;

	public:
		Trademark();
		virtual ~Trademark();

		virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
		virtual void Tick(DWORD dwDeltaMs) override;

		virtual void Draw();
	};
}