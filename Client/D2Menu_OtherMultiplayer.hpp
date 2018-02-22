#pragma once
#include "D2Menu.hpp"
#include "D2Panel_OtherMultiplayer.hpp"

class D2Menu_OtherMultiplayer : public D2Menu
{
private:
	tex_handle backgroundTexture;
	anim_handle flameLeftAnim;
	anim_handle flameRightAnim;
	anim_handle blackLeftAnim;
	anim_handle blackRightAnim;

	D2Panel_OtherMultiplayer* m_panel;
public:
	D2Menu_OtherMultiplayer();
	virtual ~D2Menu_OtherMultiplayer();

	virtual void Draw();
};