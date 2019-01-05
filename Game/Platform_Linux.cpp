#include "Diablo2.hpp"
#include <stdlib.h>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>

#define D2REGISTRY_BETA_KEY	"SOFTWARE\\Blizzard Entertainment\\Diablo II Beta"
#define D2REGISTRY_KEY		"SOFTWARE\\Blizzard Entertainment\\Diablo II"

#define REGISTRY_KEY_SIZE	2048

//////////////////////////////////////////////////////////////
//
//	Data Structures

struct D2ModuleInternal
{
	void* dwModule;
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
	*	Copy registry keys from the retail game and make them available in D2.ini
	*	This step has to be done after we've initialized the file system
	*/
	void CopySettings()
	{

	}

	/*
	*	Get topmost adapter IP address
	*/
#define ADAPTER_LIST_SIZE	15000
	char16_t* GetAdapterIP()
	{
		/*IP_ADAPTER_ADDRESSES* addresses = (IP_ADAPTER_ADDRESSES*)malloc(ADAPTER_LIST_SIZE);
		PIP_ADAPTER_ADDRESSES currAddress;
		DWORD dwSize = ADAPTER_LIST_SIZE;
		static char16_t szAddress[32];

		if (GetAdaptersAddresses(AF_INET, 0, nullptr, addresses, &dwSize) != ERROR_BUFFER_OVERFLOW)
		{
		currAddress = (PIP_ADAPTER_ADDRESSES)addresses;
		while (currAddress)
		{
		if (currAddress->OperStatus == IfOperStatusUp && currAddress->Ipv4Enabled &&
		currAddress->FirstUnicastAddress)
		{	// current adapter is operational, is IPv4 enabled and has TCP (unicast) capabilities
		DWORD dwOffset = 0;
		for (int i = 2; i < 6; i++)
		{
		size_t dwWritten = 0;

		if (i != 2)
		{	// add dots between the numbers
		szAddress[dwOffset++] = u'.';
		}

		// put the number on
		D2Lib::qnitoa((BYTE)currAddress->FirstUnicastAddress->Address.lpSockaddr->sa_data[i],
		szAddress + dwOffset, 32 - dwOffset, 10, dwWritten);
		dwOffset += dwWritten;
		}
		free(addresses);
		return szAddress;
		}
		currAddress = currAddress->Next;
		}
		}

		free(addresses);
		D2Lib::qsnprintf(szAddress, 32, u"0.0.0.0");
		return szAddress;*/
		return u"192.168.1.1";
	}

	/*
	*	Get default homepath
	*/
	void DefaultHomepath(char* szBuffer, size_t dwBufferLen)
	{
		/*char* homeDirectory[MAX_PATH];

		if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, homeDirectory)))
		{	// Couldn't find it, I guess?
		return;
		}

		snprintf(szBuffer, dwBufferLen, "%s/My Games/" GAME_HOMEPATH, homeDirectory);*/
		return;
	}

	/*
	*	Get current working directory
	*/
	void GetWorkingDirectory(char* szBuffer, size_t dwBufferLen)
	{
		getcwd(szBuffer, dwBufferLen);
	}

	/*
	*	Given a buffer and an amount of memory, writes to the buffer "X MB (Y GB)"
	*/
	static void FormatMemory(char* buffer, DWORD dwBufferLen, uint64_t ullMemory)
	{
		double fMemoryKB = ullMemory / 1024.0;
		double fMemoryMB = fMemoryKB / 1024.0;
		double fMemoryGB = fMemoryMB / 1024.0;

		snprintf(buffer, dwBufferLen, "%.0f MB (%.2f GB)", fMemoryMB, fMemoryGB);
	}

	/*
	struct D2SystemInfoStrc
	{
	char szComputerName[64];
	char szOSName[128];
	char szProcessorVendor[32];
	char szProcessorModel[64];
	char szProcessorSpeed[64];	// maybe not present on linux?
	char szProcessorIdentifier[64];
	char szRAMPhysical[64];
	char szRAMVirtual[64];
	char szRAMPaging[64];
	char szWorkingDirectory[MAX_D2PATH_ABSOLUTE];
	};
	*/

	/*
	*	Get system info from the operating system
	*	@author	eezstreet
	*/
	void GetSystemInfo(D2SystemInfoStrc* pInfo)
	{
		strcpy(pInfo->szComputerName, "quadcore");
		strcpy(pInfo->szOSName, "Linux");
		strcpy(pInfo->szProcessorVendor, "Intel?");
		strcpy(pInfo->szProcessorModel, "model number 1.0");
		strcpy(pInfo->szProcessorIdentifier, "whatever this is");
		strcpy(pInfo->szRAMPhysical, "500 TB");
		strcpy(pInfo->szRAMVirtual, "400 TB");
		strcpy(pInfo->szRAMPaging, "Rampaging? O.o");
		GetWorkingDirectory(pInfo->szWorkingDirectory, MAX_D2PATH_ABSOLUTE);
	}

	/*
	*	Create directory if it doesn't already exist
	*/
#ifdef CreateDirectory
#undef CreateDirectory
#endif
	bool CreateDirectory(char* szPath)
	{
		return false;
		//return ::CreateDirectoryA(szPath, NULL);
	}

	/*
	*	Build a list of files with an extension filter.
	*	If the extension filter is *.*, there is essentially no filter.
	*	@author	eezstreet
	*/
	void ListFilesInDirectory(char* szPath, char* szExtensionFilter, char* szOriginalPath, int* nFiles, char(*szList)[MAX_FILE_LIST_SIZE][MAX_D2PATH_ABSOLUTE])
	{
		/*char szFullPath[MAX_D2PATH_ABSOLUTE]{ 0 };
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
		} while (FindNextFile(hFile, &findData) == TRUE);*/
		return;
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
			bModuleFound = FS::Find("libD2Client.so", szModulePath, MAX_D2PATH_ABSOLUTE);
		}
		else if (nModule == MODULE_SERVER)
		{
			bModuleFound = FS::Find("liD2Game.so", szModulePath, MAX_D2PATH_ABSOLUTE);
		}
		Log_ErrorAssertReturn(bModuleFound, nullptr);

		gModules[nModule].dwModule = dlopen(szModulePath, RTLD_LAZY);
		Log_ErrorAssertReturn(gModules[nModule].dwModule != 0, nullptr);

		D2ModuleExportStrc* (*ModuleAPI)(D2ModuleImportStrc* strc);

		ModuleAPI = (D2ModuleExportStrc*(*)(D2ModuleImportStrc*))dlsym(gModules[nModule].dwModule, "GetModuleAPI");
		Log_ErrorAssertReturn(ModuleAPI != nullptr, nullptr);

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

		dlclose(gModules[nModule].dwModule);
		memset(&gModules[nModule], 0, sizeof(D2ModuleInternal));
	}
}

int main(int argc, char* argv[])
{
	return InitGame(argc, argv);
}