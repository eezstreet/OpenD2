#pragma once
#pragma comment(lib,"D2Win")
#include "D2Shared.hpp"
#include <Windows.h>

#define D2_API extern "C" __declspec (dllimport)

D2_API DWORD __stdcall D2WIN_10000(HINSTANCE, DWORD, DWORD, DWORD);
D2_API DWORD __stdcall D2WIN_10001(DWORD, DWORD);
D2_API DWORD __stdcall D2WIN_10002(void);
D2_API void __fastcall D2WIN_10036(void);
D2_API BOOL __fastcall D2WIN_10037(void);
D2_API BOOL __fastcall D2WIN_10171(void*, void*, DWORD, void*);
D2_API void __fastcall D2WIN_10174(void);
D2_API void __fastcall D2WIN_10205(void);

                   

