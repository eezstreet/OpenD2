#pragma once
#include "../Shared/D2Shared.hpp"
#include "Diablo2.hpp"

// Platform_*.cpp
namespace Sys
{
	void GetWorkingDirectory(char* szBuffer, size_t dwBufferLen);
	void DefaultHomepath(char* szBuffer, size_t dwBufferLen);
	void GetSystemInfo(D2SystemInfoStrc* pInfo);
	bool CreateDirectory(char* szPath);
	void ListFilesInDirectory(const char* szPath, const char* szExtensionFilter, const char* szOriginalPath, int* nFiles, char(*szList)[MAX_FILE_LIST_SIZE][MAX_D2PATH_ABSOLUTE]);
	D2ModuleExportStrc* OpenModule(OpenD2Modules nModule, D2ModuleImportStrc* pImports);
	void CloseModule(OpenD2Modules nModule);
	char16_t* GetAdapterIP();
}
