#pragma once
#include "D2Menu.hpp"

class D2Menu_Loading : public D2Menu
{
private:
	IRenderObject* loadscreen;
	IGraphicsReference* loadscreenTexture;

public:
	D2Menu_Loading();
	virtual ~D2Menu_Loading();

	virtual void Draw();
};