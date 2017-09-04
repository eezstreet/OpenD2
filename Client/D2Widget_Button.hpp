#pragma once
#include "D2Widget.hpp"

class D2Widget_Button : public D2Widget
{
private:
	tex_handle texture;
	bool bDisabled;

public:
	virtual void Draw();
};