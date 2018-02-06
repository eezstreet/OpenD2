#define WIN32_LEAN_AND_MEAN
#include "Diablo2.hpp"
#include <Windows.h>
#include <stdlib.h>
#include <cstdio>
#include <shlobj.h>
#include <crtdbg.h>

#define D2REGISTRY_BETA_KEY	"SOFTWARE\\Blizzard Entertainment\\Diablo II Beta"
#define D2REGISTRY_KEY		"SOFTWARE\\Blizzard Entertainment\\Diablo II"

#define REGISTRY_KEY_SIZE	2048

//////////////////////////////////////////////////////////////
//
//	Data Structures

struct D2ModuleInternal
{
	HMODULE dwModule;
	D2ModuleExportStrc* pExports;
};

//////////////////////////////////////////////////////////////
//
//	Global variables

static D2ModuleInternal gModules[MODULE_MAX]{ 0 };

//////////////////////////////////////////////////////////////
//
//	Platform-Specific Functions

namespace Sys
{

	/*
	 *	Copy registry keys (and delete them) from the Diablo II beta to the retail game.
	 *	This step is done before anything else.
	 *	Never really gets used anymore, but since we're in the business of replicating base game behavior...
	 */
	void CopyBetaRegistryKeys()
	{
		HKEY betakey;
		HKEY key;
		int i;
		DWORD type;
		BYTE data;
		DWORD cbdata = 0;
		LSTATUS status;
		char keybuffer[REGISTRY_KEY_SIZE]{ 0 };
		DWORD keybufferSize = REGISTRY_KEY_SIZE;

		if (!RegOpenKeyA(HKEY_LOCAL_MACHINE, D2REGISTRY_BETA_KEY, &betakey))
		{	// We had the Diablo II beta installed. Copy the keys and delete the old ones.
			RegCreateKeyA(HKEY_LOCAL_MACHINE, D2REGISTRY_KEY, &key);

			i = 0;
			do
			{
				status = RegEnumValueA(betakey, i++, keybuffer, &keybufferSize, 0, &type, &data, &cbdata);
			} while (status == 0 && RegSetValueExA(key, keybuffer, 0, type, &data, cbdata));

			RegCloseKey(betakey);
			RegCloseKey(key);
			RegDeleteKeyA(HKEY_LOCAL_MACHINE, D2REGISTRY_BETA_KEY);
		}
	}

	/*
	 *	Copy registry keys from the retail game and make them available in D2.ini
	 *	This step has to be done after we've initialized the file system
	 */
	void CopySettings()
	{

	}

	/*
	 *	Get default homepath
	 */
	void DefaultHomepath(char* szBuffer, size_t dwBufferLen)
	{
		TCHAR homeDirectory[MAX_PATH];

		if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, homeDirectory)))
		{	// Couldn't find it, I guess?
			return;
		}

		snprintf(szBuffer, dwBufferLen, "%s/My Games/" GAME_HOMEPATH, homeDirectory);
	}

	/*
	 *	Get current working directory
	 */
	void GetWorkingDirectory(char* szBuffer, size_t dwBufferLen)
	{
		GetCurrentDirectoryA(dwBufferLen, szBuffer);
	}

	/*
	 *	Given a buffer and an amount of memory, writes to the buffer "X MB (Y GB)"
	 */
	static void FormatMemory(char* buffer, DWORD dwBufferLen, DWORDLONG ullMemory)
	{
		double fMemoryKB = ullMemory / 1024.0;
		double fMemoryMB = fMemoryKB / 1024.0;
		double fMemoryGB = fMemoryMB / 1024.0;

		snprintf(buffer, dwBufferLen, "%.0f MB (%.2f GB)", fMemoryMB, fMemoryGB);
	}

	/*
	 *	Get system info from the operating system
	 *	@author	eezstreet
	 */
	void GetSystemInfo(D2SystemInfoStrc* pInfo)
	{
		DWORD dwWriteSize;
		OSVERSIONINFO osi;

		osi.dwOSVersionInfoSize = sizeof(osi);

		GetWorkingDirectory(pInfo->szWorkingDirectory, MAX_D2PATH_ABSOLUTE);

		dwWriteSize = 64;
		GetComputerName(pInfo->szComputerName, &dwWriteSize);

		dwWriteSize = 128;
		GetVersionEx(&osi);

		// Version string is tricky because Microsoft is bad at numbering
		switch (osi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s:
			D2Lib::strncpyz(pInfo->szOSName, "Windows 3.x", dwWriteSize);
			break;
		case VER_PLATFORM_WIN32_NT:
			snprintf(pInfo->szOSName, dwWriteSize,
				"Windows NT (Version %i.%i) %s",
				osi.dwMajorVersion, osi.dwMinorVersion, osi.szCSDVersion);
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			D2Lib::strncpyz(pInfo->szOSName, osi.dwMinorVersion == 0 ? "Windows 95" : "Windows 98", dwWriteSize);
			break;
		default:
			D2Lib::strncpyz(pInfo->szOSName, "{unknown}", dwWriteSize);
			break;
		}

		// RAM is pretty straightforward
		MEMORYSTATUSEX ms;
		ms.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&ms);

		dwWriteSize = 64;
		FormatMemory(pInfo->szRAMPhysical, dwWriteSize, ms.ullTotalPhys);
		FormatMemory(pInfo->szRAMPaging, dwWriteSize, ms.ullTotalPageFile);
		FormatMemory(pInfo->szRAMVirtual, dwWriteSize, ms.ullTotalVirtual);

		// Windows Strikes Again with the poor options to grab processor information.
		// The best way seems to be to pull it out of the registry, so we'll go with that.
		HKEY key = 0;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
			0, KEY_QUERY_VALUE, &key) == 0)
		{
			DWORD MHz = 0;

			dwWriteSize = 32;
			RegQueryValueEx(key, "VendorIdentifier", NULL, NULL, (LPBYTE)pInfo->szProcessorVendor, &dwWriteSize);

			dwWriteSize = 64;
			RegQueryValueEx(key, "ProcessorNameString", NULL, NULL, (LPBYTE)pInfo->szProcessorModel, &dwWriteSize);
			RegQueryValueEx(key, "Identifier", NULL, NULL, (LPBYTE)pInfo->szProcessorIdentifier, &dwWriteSize);

			dwWriteSize = sizeof(DWORD);
			RegQueryValueEx(key, "~MHz", NULL, NULL, (LPBYTE)&MHz, &dwWriteSize);
			RegCloseKey(key);

			dwWriteSize = 64;
			snprintf(pInfo->szProcessorSpeed, dwWriteSize, "~%i MHz (approx.)", MHz);
		}
		else
		{
			D2Lib::strncpyz(pInfo->szProcessorVendor, "{unknown}", 32);
			D2Lib::strncpyz(pInfo->szProcessorModel, "{unknown}", 64);
			D2Lib::strncpyz(pInfo->szProcessorSpeed, "{unknown}", 64);
		}
	}

	/*
	 *	Create directory if it doesn't already exist
	 */
#ifdef CreateDirectory
#undef CreateDirectory
#endif
	bool CreateDirectory(char* szPath)
	{
		return ::CreateDirectoryA(szPath, NULL);
	}

	/*
	 *	Build a list of files with an extension filter.
	 *	If the extension filter is *.*, there is essentially no filter.
	 *	@author	eezstreet
	 */
	void ListFilesInDirectory(char* szPath, char* szExtensionFilter, char* szOriginalPath, int* nFiles, char(*szList)[MAX_FILE_LIST_SIZE][MAX_D2PATH_ABSOLUTE])
	{
		char szFullPath[MAX_D2PATH_ABSOLUTE]{ 0 };
		HANDLE hFile;
		WIN32_FIND_DATA findData;

		snprintf(szFullPath, MAX_D2PATH_ABSOLUTE, "%s/%s", szPath, szExtensionFilter);

		// Find the first file.
		hFile = FindFirstFile(szFullPath, &findData);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		// Iterate through the list of files, adding them to the szList
		do
		{
			snprintf((*szList)[*nFiles], MAX_D2PATH_ABSOLUTE, "%s/%s", szOriginalPath, findData.cFileName);
			*nFiles = *nFiles + 1;
		} while (FindNextFile(hFile, &findData) == TRUE);
	}

	/*
	 *	Gets the API of a module
	 *	Note: This doesn't validate the API version etc, we need to do this manually afterward.
	 */
	D2ModuleExportStrc* OpenModule(OpenD2Modules nModule, D2ModuleImportStrc* pImports)
	{
		char szModulePath[MAX_D2PATH_ABSOLUTE]{ 0 };
		bool bModuleFound = false;

		if (gModules[nModule].dwModule != 0)
		{
			return gModules[nModule].pExports;
		}

		if (nModule == MODULE_CLIENT)
		{
			bModuleFound = FS::Find("D2Client.dll", szModulePath, MAX_D2PATH_ABSOLUTE);
		}
		else if (nModule == MODULE_SERVER)
		{
			bModuleFound = FS::Find("D2Game.dll", szModulePath, MAX_D2PATH_ABSOLUTE);
		}
		Log_ErrorAssert(bModuleFound, nullptr);

		gModules[nModule].dwModule = LoadLibrary(szModulePath);
		Log_ErrorAssert(gModules[nModule].dwModule != 0, nullptr);

		GetAPIType ModuleAPI;

		ModuleAPI = (GetAPIType)GetProcAddress(gModules[nModule].dwModule, "GetModuleAPI");
		Log_ErrorAssert(ModuleAPI != nullptr, nullptr);

		return ModuleAPI(pImports);
	}

	/*
	 *	Closes a single module
	 */
	void CloseModule(OpenD2Modules nModule)
	{
		if (gModules[nModule].dwModule == 0)
		{
			return;
		}

		FreeLibrary(gModules[nModule].dwModule);
		memset(&gModules[nModule], 0, sizeof(D2ModuleInternal));
	}
}

/*
 *	The main entrypoint of the program (on Windows)
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* szCmdLine, int nShowCmd)
{
	// TODO: copy fields from registry and put them in D2.ini, which we also need to load and parse..
	Sys::CopyBetaRegistryKeys();

	return InitGame(__argc, __argv, (DWORD)hInstance);
}