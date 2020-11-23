#pragma once
#include "../D2Widget.hpp"
#include "Button.hpp"
#include "SlideControl.hpp"

namespace D2Widgets
{
	class Scrollbar : public D2Widget
	{
	private:
		Button* upButton;
		Button* dnButton;
		SlideControl* slider;
	public:
	};
}