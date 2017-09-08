#include "D2Widget.hpp"

D2Widget::D2Widget()
{
	m_pNextVisibleWidget = nullptr;
	m_pNextWidget = nullptr;
	m_bVisible = false;
}

D2Widget::D2Widget(int _x, int _y, int _w, int _h)
{
	m_pNextVisibleWidget = nullptr;
	m_pNextWidget = nullptr;
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	m_bVisible = false;
}

D2Widget::~D2Widget()
{

}