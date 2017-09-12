#pragma once
#include "../Shared/D2DataTables.hpp"

////////////////////////////////////////////
//
//	Structures

struct D2TxtColumnStrc
{
	const char* szColumnName;
	DWORD		dwColumnSize;
	DWORD		dwColumnOffset;
	void		(*parseCallback)();
};

////////////////////////////////////////////
//
//	Functions