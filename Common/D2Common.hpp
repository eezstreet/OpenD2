#pragma once
#include "../Shared/D2DataTables.hpp"

////////////////////////////////////////////
//
//	Preprocessor Definitions

#define D2DATATABLES_DIR	"DATA\\GLOBAL\\EXCEL\\"

////////////////////////////////////////////
//
//	Structures and Types

enum D2MathFunc
{
	D2MATH_OR,
	D2MATH_AND,
	D2MATH_XOR,
	D2MATH_MOV,
	D2MATH_MOVZ,
	D2MATH_ANDNOT,
};

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

// D2Common_Math.cpp
void Math_Perform(D2MathFunc func, DWORD* pIn, DWORD pOperand);

// DataTables.cpp
void DataTables_Init();
void DataTables_Free();

////////////////////////////////////////////
//
//	Global Variables

extern D2ModuleImportStrc* trap;
extern D2GameConfigStrc* gpConfig;
extern OpenD2ConfigStrc* gpOpenConfig;