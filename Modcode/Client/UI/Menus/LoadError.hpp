#pragma once
#include "../D2Menu.hpp"

namespace D2Menus
{
	// This menu is extremely straightforward. It draws a bit of text, and when the user clicks,
	// it yanks us back to the previous context.
	class LoadError : public D2Menu
	{
	private:
		char16_t* szErrorText;
	public:
		LoadError(WORD wStringIndex);

		virtual void Draw();
		virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
	};
}