#pragma once
#include "D2Widget.hpp"
#include "D2Widget_Button.hpp"
#include "D2Widget_SlideControl.hpp"

class D2Widget_Scrollbar : public D2Widget
{
private:
	D2Widget_Button*		upButton;
	D2Widget_Button*		dnButton;
	D2Widget_SlideControl*	slider;
public:
};