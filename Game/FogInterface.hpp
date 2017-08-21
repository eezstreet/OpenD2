#pragma once
#include "LibraryInterface.hpp"

class FogInterface : LibraryInterface
{
public:
	FogInterface();

	std::function<void __fastcall(const char*)> InitLogManager;
	std::function<void __fastcall(void*, DWORD, void*, DWORD)> InitSystem;
	std::function<void __fastcall(DWORD, DWORD)> InitAsyncData;
	std::function<void __fastcall(void)> CloseAsyncData;
	std::function<void __fastcall(DWORD, DWORD)> SetWorkingDirectory;
	std::function<void __cdecl(void*)> CloseSystem;
	std::function<void __fastcall(void)> InitMaskTables;
	std::function<DWORD __fastcall(void)> CheckExpansion;
};