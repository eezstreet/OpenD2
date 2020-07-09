#pragma once
#include "D2Menu.hpp"

class D2Menu_Trademark : public D2Menu
{
private:
	char16_t* szCopyrightText;
	char16_t* szAllRightsReservedText;
	DWORD dwStartTicks;

	IRenderObject* backgroundObject;
	IRenderObject* flameLeft;
	IRenderObject* flameRight;
	IRenderObject* blackLeft;
	IRenderObject* blackRight;
	IRenderObject* trademark;
	IRenderObject* allRightsReserved;

	IGraphicsReference* background;

public:
	D2Menu_Trademark();
	virtual ~D2Menu_Trademark();

	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
	virtual bool WaitingSignal();
	static void TrademarkSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget);

	virtual void Draw();
};
