#pragma once
#include "D2Menu.hpp"
#include "D2Panel_Main.hpp"

class D2Menu_Main : public D2Menu
{
private:
	tex_handle backgroundTexture;
	anim_handle flameLeftAnim;
	anim_handle flameRightAnim;
	anim_handle blackLeftAnim;
	anim_handle blackRightAnim;

	D2Panel_Main* pMainPanel;

public:
	D2Menu_Main();
	~D2Menu_Main();

	virtual void Draw();
};