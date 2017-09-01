#pragma once
#include "D2Client.hpp"

class D2Menu
{
protected:
	D2Panel* m_panels;
	D2Panel* m_visiblePanels;

	D2Menu();
	~D2Menu();

public:
	void AddPanel(D2Panel* pPanel);
	virtual void Draw() = 0;
};