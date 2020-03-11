#pragma once
#include "D2Menu.hpp"
#include "D2Panel_Main.hpp"

class D2Menu_Main : public D2Menu
{
private:
	IRenderObject* flameLeft;
	IRenderObject* flameRight;
	IRenderObject* blackLeft;
	IRenderObject* blackRight;

	IRenderObject* versionText;

	D2Panel_Main* pMainPanel;
	IRenderObject* backgroundObject;

public:
	D2Menu_Main();
	virtual ~D2Menu_Main();

	virtual void Draw();
};