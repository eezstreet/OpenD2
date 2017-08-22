#pragma once
#include "D2Shared.hpp"
/*
#ifndef _LIB
#pragma comment(lib,"Storm")
#define D2_API(Name,Return,Type,Params)	\
extern "C" __declspec (dllimport) void __stdcall Name();	\
typedef Return (Type * T##Name)Params;	\
static T##Name _##Name = (T##Name)Name;
#else
#define D2_API(Name,Return,Type,Params)	\
extern "C" __declspec (dllexport) Return Type Name(Params);
#endif

#ifndef _LIB
extern "C" __declspec(dllimport) void* __stdcall STORM_403(void* pMem, char* szBlockName, int nLine, int nZero);
#else
extern "C" __declspec(dllexport) void* __stdcall STORM_403(void* pMem, char* szBlockName, int nLine, int nZero);
#endif
*/

#ifndef _LIB
#pragma comment(lib,"Storm")
#define D2DLL_API extern "C" __declspec(dllimport)
#else
#define D2DLL_API extern "C" __declspec(dllexport)
#endif

D2DLL_API void* __stdcall STORM_403(void* pMem, char* szBlockName, int nLine, int nZero);