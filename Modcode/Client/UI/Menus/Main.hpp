#pragma once
#include "../D2Menu.hpp"
#include "../Panels/Main.hpp"

namespace D2Menus
{
	class Main : public D2Menu
	{
	private:
		IRenderObject* flameLeft;
		IRenderObject* flameRight;
		IRenderObject* blackLeft;
		IRenderObject* blackRight;

		IRenderObject* versionText;

		D2Panels::Main* pMainPanel;
		IRenderObject* backgroundObject;
		IGraphicsReference* background;

	public:
		Main();
		virtual ~Main();

		virtual void Draw();
	};
}