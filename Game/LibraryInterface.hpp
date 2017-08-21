#pragma once
#include "diablo2.h"

class LibraryInterface
{
protected:
	template<typename T> 
	void BindFunction(WORD ordinal);
};