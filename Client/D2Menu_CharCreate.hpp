#pragma once
#include "D2Menu.hpp"

class D2Menu_CharCreate : public D2Menu
{
private:
	tex_handle backgroundTex;

public:
	D2Menu_CharCreate();
	~D2Menu_CharCreate();

	virtual void Draw();
};