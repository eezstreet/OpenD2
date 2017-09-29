#pragma once
#include "../Shared/D2DataTables.hpp"

////////////////////////////////////////////
//
//	Preprocessor Definitions

#define D2DATATABLES_DIR	"DATA\\GLOBAL\\EXCEL\\"

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

// DataTables.cpp
void DataTables_Init();
void DataTables_Free();

////////////////////////////////////////////
//
//	Global Variables

extern D2ModuleImportStrc* trap;
extern D2GameConfigStrc* gpConfig;
extern OpenD2ConfigStrc* gpOpenConfig;