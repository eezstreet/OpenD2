#pragma once
#include "../Shared/D2Shared.hpp"

// Window.cpp
namespace Window
{
	void InitSDL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
	void ShutdownSDL();
	void ShowMessageBox(int nMessageBoxType, const char *szTitle, const char *szMessage);
	bool InFocus(DWORD nWindowID);
}
