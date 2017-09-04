#include "D2Widget.hpp"

D2Widget::D2Widget(int _x, int _y, int _w, int _h, D2Panel* pPanel)
{
	pParent = pPanel;
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

D2Widget::~D2Widget()
{

}