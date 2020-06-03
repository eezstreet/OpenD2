#pragma once
#include "D2Menu.hpp"

class D2Menu_Loading : public D2Menu
{
private:
	tex_handle loadscreenTex;
	anim_handle loadscreenAnim;

public:
	D2Menu_Loading();
	~D2Menu_Loading() override;

	void Draw() override;
};
