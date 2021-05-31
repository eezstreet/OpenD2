#pragma once
#include "../../Shared/D2DataTables.hpp"

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
uint16_t Seed_Next(uint16_t& seed);
uint32_t Seed_Next(uint32_t& seed);
uint64_t Seed_Next(uint64_t& seed);
int Seed_Range(uint16_t& seed, int min, int max);
int Seed_Range(uint32_t& seed, int min, int max);
int Seed_Range(uint64_t& seed, int min, int max);
int Seed_Range(int& seed, int min, int max);

// DRLG_Main.cpp
D2COMMONAPI void D2Common_ConstructSingleLevel(int nLevelId, int& seed);

// DataTables.cpp
void DataTables_Init();
void DataTables_Free();

////////////////////////////////////////////
//
//	Global Variables

extern D2ModuleImportStrc* engine;
extern D2GameConfigStrc* gpConfig;
extern OpenD2ConfigStrc* gpOpenConfig;