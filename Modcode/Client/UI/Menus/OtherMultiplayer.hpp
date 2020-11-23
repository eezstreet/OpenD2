#pragma once
#include "../D2Menu.hpp"
#include "../Panels/OtherMultiplayer.hpp"

namespace D2Menus
{
	class OtherMultiplayer : public D2Menu
	{
	private:
		tex_handle backgroundTexture;
		anim_handle flameLeftAnim;
		anim_handle flameRightAnim;
		anim_handle blackLeftAnim;
		anim_handle blackRightAnim;

		D2Panels::OtherMultiplayer* m_panel;
	public:
		OtherMultiplayer();
		virtual ~OtherMultiplayer();

		virtual void Draw();
	};
}