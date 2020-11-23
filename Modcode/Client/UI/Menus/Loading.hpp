#pragma once
#include "../D2Menu.hpp"

namespace D2Menus
{
	class Loading : public D2Menu
	{
	private:
		IRenderObject* loadscreen;
		IGraphicsReference* loadscreenTexture;

	public:
		Loading();
		virtual ~Loading();

		virtual void Draw();
	};
}