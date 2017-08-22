#pragma once
#pragma comment(lib,"D2Client")
#include "D2Shared.hpp"
#define D2_API extern "C" __declspec (dllimport)

typedef DWORD (__fastcall* pfInterface)(BYTE*);

D2_API pfInterface __fastcall QueryInterface(void);