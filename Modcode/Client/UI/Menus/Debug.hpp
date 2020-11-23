#pragma once
#include "../D2Menu.hpp"
#include "../Panels/Debug.hpp"

namespace D2Menus
{
	class Debug : public D2Menu
	{
	private:
		IRenderObject* backgroundObject;

		D2Panels::Debug* pDebugPanel;

	public:
		Debug();
		virtual ~Debug();

		virtual void Draw();
	};
}