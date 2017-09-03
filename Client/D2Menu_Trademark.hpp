#pragma once
#include "D2Menu.hpp"

class D2Menu_Trademark : public D2Menu
{
private:
	tex_handle backgroundTexture;
	anim_handle flameLeftAnim;
	anim_handle flameRightAnim;

public:
	D2Menu_Trademark();
	~D2Menu_Trademark();

	virtual void Draw();
};
