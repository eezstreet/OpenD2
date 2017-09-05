#pragma once
#include "D2Widget.hpp"

class D2Widget_Button : public D2Widget
{
private:
	bool bDisabled;
	

public:
	virtual void Draw() = 0;

	tex_handle texture;
};