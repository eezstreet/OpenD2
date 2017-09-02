#pragma once
#include "D2Menu.hpp"

class D2Menu_Main : public D2Menu
{
private:
	tex_handle backgroundTexture;

public:
	D2Menu_Main();
	~D2Menu_Main();

	virtual void Draw();
};