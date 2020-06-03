#pragma once
#include "../Shared/D2Shared.hpp"

// FileSystem.cpp
namespace FS
{
	void Init(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig);
	void Shutdown();
	void LogSearchPaths();
	size_t Open(const char* filename, fs_handle* f, OpenD2FileModes mode, bool bBinary = false);
	size_t Read(fs_handle f, void* buffer, size_t dwBufferLen = 4, size_t dwCount = 1);
	size_t Write(fs_handle f, void* buffer, size_t dwBufferLen = 1, size_t dwCount = 1);
	size_t WritePlaintext(fs_handle f, const char* text);
	void CloseFile(fs_handle f);
	void Seek(fs_handle f, size_t offset, int nSeekType);
	size_t Tell(fs_handle f);
	bool Find(const char *szFileName, char* szBuffer, size_t dwBufferLen);
	char** ListFilesInDirectory(const char *szDirectory, const char *szExtensionFilter, int *nFiles);
	void FreeFileList(char** pszFileList, int nNumFiles);
	void CreateSubdirectory(const char *szSubdirectory);
}
