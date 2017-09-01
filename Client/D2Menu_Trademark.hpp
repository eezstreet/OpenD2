#pragma once
#include "D2Menu.hpp"

class D2Menu_Trademark : public D2Menu
{
private:
	tex_handle backgroundTexture;

public:
	D2Menu_Trademark();
	~D2Menu_Trademark();

	virtual void Draw();
};
