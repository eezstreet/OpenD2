#include "Diablo2.hpp"
#include <stdlib.h>
#include <string>
#include <assert.h>

/*
 *	The OpenD2 filesystem varies greatly from the one in retail Diablo 2.
 *	Originally, Diablo 2 handled stuff *only* in the original working directory, and the registry.
 *
 *	This possesses a number of problems:
 *	1. It's unintuitive. Configurations should be user-modifiable in locations that make sense.
 *	2. It requires that the program have write permissions in its own directory, which is not ideal.
 *	3. It's rigid and doesn't allow for modifications.
 *
 *	To address this problem, we use three separate directories to load files from:
 *	homepath: Last read, first write. Corresponds to ~ on Linux and My Documents/My Games/OpenD2 on Windows.
 *	basepath: Second read, second write. Corresponds to the working directory.
 *	modpath: First read, last write. Defaults to the working directory.
 *
 *	All of these paths can be adjusted by the user on startup, in D2.ini, or (as a last resort), the registry.
 *	Whenever we reference a file directly (ie, "d2char.mpq"), it's always assumed to be a path relative to a directory.
 */

static char gszHomePath[MAX_D2PATH_ABSOLUTE]{ 0 };
static char gszBasePath[MAX_D2PATH_ABSOLUTE]{ 0 };
static char gszModPath[MAX_D2PATH_ABSOLUTE]{ 0 };

static const char* pszPaths[FS_MAXPATH] = {
	gszHomePath,
	gszBasePath,
	gszModPath,
};

struct FSHandleStore
{
	char szFileName[MAX_D2PATH];
	fs_handle handle;
	OpenD2FileModes mode;

	FSHandleStore* next;
};

static FSHandleStore* glFileHandles = nullptr;

/*
 *	Log the searchpaths
 */
void FS_LogSearchPaths()
{
	Log_Print(PRIORITY_VISUALS, "------------------------------------------------------");
	Log_Print(PRIORITY_MESSAGE, "Search Paths:");
	Log_Print(PRIORITY_MESSAGE, "- %s", gszHomePath);
	Log_Print(PRIORITY_MESSAGE, "- %s", gszBasePath);
	Log_Print(PRIORITY_MESSAGE, "- %s", gszModPath);
	Log_Print(PRIORITY_VISUALS, "------------------------------------------------------");
}

/*
 *	Cleanses a filesystem search path
 */
static void FS_SanitizeSearchPath(char* path)
{
	size_t dwPathLen = 0;
	char szCWD[MAX_D2PATH_ABSOLUTE]{ 0 };

	Sys_GetWorkingDirectory(szCWD, MAX_D2PATH_ABSOLUTE);

	if (path[0] == '\0')
	{
		// Blank strings aren't acceptable. We should be using the working directory if this is the case.
		D2_strncpyz(path, szCWD, MAX_D2PATH_ABSOLUTE);
	}

	if (path[0] == '.' && path[1] == '/')
	{
		// Replace all of the data up to the first slash with the working directory
		strcat(szCWD, path + 1);
		D2_strncpyz(path, szCWD, MAX_D2PATH_ABSOLUTE);
	}
	else if (path[0] == '.' && path[1] == '..')
	{
		// FIXME: not currently supported
		// Replace all of the data up to the first slash with the working directory, up one level.
		// If there's any subsequent .. (in slashes?) the operating system is smart enough to deal with it (hopefully)
		assert(!"Used a currently unsupported feature (.. in file path)");
	}

	// Replace Windows-style slashes (\) with Unix-style slashes (/)
	for (int i = 0; i < strlen(path); i++)
	{
		if (path[i] == '\\')
		{
			path[i] = '/';
		}
	}

	// Remove double slashes (//)
	for (int i = 0; i < strlen(path); i++)
	{
		if (path[i] == '/' && path[i + 1] == '/')
		{	// found em
			D2_strncpyz(path + i, path + i + 1, MAX_D2PATH_ABSOLUTE);
		}
	}

	// Make sure the path ends in a slash
	dwPathLen = strlen(path);
	if (dwPathLen < MAX_D2PATH_ABSOLUTE - 1)
	{
		if (path[dwPathLen - 1] != '/')
		{
			path[dwPathLen] = '/';
			path[dwPathLen + 1] = '\0';
		}
	}

	// If path doesn't exist, create it
	Sys_CreateDirectory(path);
}

/*
 *	Initializes the OpenD2 filesystem
 *	@author	eezstreet
 */
void FS_Init(OpenD2ConfigStrc* pConfig)
{
	// Copy paths from the config to the FS
	if (pConfig->szHomePath[0] == '\0')
	{
		Sys_DefaultHomepath(gszHomePath, MAX_D2PATH_ABSOLUTE);
	}
	else
	{
		D2_strncpyz(gszHomePath, pConfig->szHomePath, MAX_D2PATH_ABSOLUTE);
	}
	D2_strncpyz(gszBasePath, pConfig->szBasePath, MAX_D2PATH_ABSOLUTE);
	D2_strncpyz(gszModPath, pConfig->szModPath, MAX_D2PATH_ABSOLUTE);

	// Make sure there's no garbage in the strings
	FS_SanitizeSearchPath(gszHomePath);
	FS_SanitizeSearchPath(gszBasePath);
	FS_SanitizeSearchPath(gszModPath);

	// Init extensions
	FSMPQ_Init();
}

/*
 *	Shuts down the file system
 *	@author	eezstreet
 */
void FS_Shutdown()
{
	// Shut down any extensions that need closing
	FSMPQ_Shutdown();

	FSHandleStore* pRecord = glFileHandles;
	FSHandleStore* pPrev = nullptr;

	// Kill any files that were opened that weren't closed
	while (pRecord != nullptr)
	{
		pPrev = pRecord;
		pRecord = pRecord->next;
		fclose((FILE*)pPrev->handle);
		free(pPrev);
	}
}

/*
 *	Cleanses a file's path.
 *	Any filename passed into the function is assumed to have MAX_D2PATH characters in its buffer, or otherwise be valid.
 */
static void FS_SanitizeFilePath(char* path)
{
	// Remove leading slashes
	if (*path == '/')
	{
		(*path)++;
	}

	// Remove double slashes (//)
	for (int i = 0; i < strlen(path); i++)
	{
		if (path[i] == '/' && path[i + 1] == '/')
		{	// found em
			D2_strncpyz(path + i, path + i + 1, MAX_D2PATH_ABSOLUTE);
		}
	}
}

/*
 *	Converts an FS_MODE into something that can be used by fopen, etc
 */
static const char* FS_ModeStr(OpenD2FileModes mode, bool bBinary)
{
	switch (mode)
	{
		case FS_READ:
		{
			if (bBinary)
			{
				return "rb";
			}
			else
			{
				return "r";
			}
		}
		case FS_WRITE:
		{
			if (bBinary)
			{
				return "wb";
			}
			else
			{
				return "w";
			}
		}
		case FS_READWRITE:
		{
			if (bBinary)
			{
				return "rwb";
			}
			else
			{
				return "rw";
			}
		}
		case FS_APPEND:
		{
			if (bBinary)
			{
				return "ab";
			}
			else
			{
				return "a";
			}
		}
	}
	return "";
}

/*
 *	Open a file with the select mode.
 *	@return	The size of the file
 */
size_t FS_Open(char* filename, fs_handle* f, OpenD2FileModes mode, bool bBinary)
{
	char path[MAX_D2PATH_ABSOLUTE]{ 0 };
	char folder[MAX_D2PATH]{ 0 };
	const char* szModeStr = FS_ModeStr(mode, bBinary);

	FS_SanitizeFilePath(filename);
	if (mode == FS_READ)
	{	// If we're reading, we use the search paths in reverse order
		for (int i = FS_MAXPATH - 1; i >= 0; i--)
		{
			D2_strncpyz(path, pszPaths[i], MAX_D2PATH_ABSOLUTE);
			strcat(path, filename);

			if (*f = (fs_handle)fopen(path, szModeStr))
			{
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < FS_MAXPATH; i++)
		{
			D2_strncpyz(path, pszPaths[i], MAX_D2PATH_ABSOLUTE);
			strcat(path, filename);

			if (*f = (fs_handle)fopen(path, szModeStr))
			{
				break;
			}
		}
	}
	

	if (*f == 0)
	{	// file could not be found
		*f = INVALID_HANDLE;
		return 0;
	}

	// Push this file handle to the linked list
	FSHandleStore* pStore = (FSHandleStore*)malloc(sizeof(FSHandleStore));
	assert(pStore);
	pStore->handle = *f;
	pStore->next = nullptr;
	pStore->mode = mode;
	D2_strncpyz(pStore->szFileName, filename, MAX_D2PATH);

	if (glFileHandles != nullptr)
	{
		FSHandleStore* pPrev = nullptr;
		FSHandleStore* pCurrent = glFileHandles;
		while (pCurrent != nullptr)
		{
			pPrev = pCurrent;
			pCurrent = pCurrent->next;
		}

		pPrev->next = pStore;
	}
	else
	{
		glFileHandles = pStore;
	}

	// Get the length of the file and return it
	size_t dwLen = 0;
	fseek((FILE*)*f, 0, SEEK_END);
	dwLen = ftell((FILE*)*f);
	rewind((FILE*)*f);

	return dwLen;
}

/*
 *	Finds a file record (from the linked list). Returns nullptr if it isn't found.
 */
static FSHandleStore* FS_GetFileRecord(fs_handle f)
{
	FSHandleStore* pRecord = glFileHandles;
	while (pRecord != nullptr)
	{
		if (pRecord->handle == f)
		{
			return pRecord;
		}
		pRecord = pRecord->next;
	}
	return nullptr;
}

/*
 *	Read from the file into a buffer
 *	@return	The number of bytes read from the file
 */
size_t FS_Read(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount)
{
	return fread(buffer, dwBufferLen, dwCount, (FILE*)f);
}

/*
 *	Write to a file
 *	@return	The number of bytes written to the file
 */
size_t FS_Write(fs_handle f, void* buffer, size_t dwBufferLen, size_t dwCount)
{
	if (f == INVALID_HANDLE)
	{
		return 0;
	}

	FSHandleStore* pRecord = FS_GetFileRecord(f);
	if (!pRecord || pRecord->mode == FS_READ)
	{
		// Not allowed to write to something which we opened in read mode
		return 0;
	}

	if (dwBufferLen == 0)
	{
		dwBufferLen = strlen((const char*)buffer);
	}

	return fwrite(buffer, dwCount, dwBufferLen, (FILE*)f);
}

/*
 *	Writes plaintext to a file
 */
size_t FS_WritePlaintext(fs_handle f, char* text)
{
	size_t dwLen = strlen(text);
	return FS_Write(f, text, dwLen);
}

/*
 *	Close a file handle
 */
void FS_CloseFile(fs_handle f)
{
	FSHandleStore* pRecord = glFileHandles;
	FSHandleStore* pPrevRecord = nullptr;

	while (pRecord != nullptr)
	{
		if (pRecord->handle == f)
		{	// found it
			if (pPrevRecord == nullptr)
			{
				glFileHandles = pRecord->next;
			}
			else
			{
				pPrevRecord->next = pRecord->next;
			}
			fclose((FILE*)pRecord->handle);
			free(pRecord);
			break;
		}
		pPrevRecord = pRecord;
		pRecord = pRecord->next;
	}
}

/*
 *	Perform a seek on a file handle
 */
void FS_Seek(fs_handle f, size_t offset, int nSeekType)
{
	fseek((FILE*)f, offset, nSeekType);
}

/*
 *	Perform a tell on a file handle
 */
size_t FS_Tell(fs_handle f)
{
	return ftell((FILE*)f);
}

/*
 *	Finds the (absolute) path of a file, anywhere in our search paths.
 */
bool FS_Find(char* szFileName, char* szBuffer, size_t dwBufferLen)
{
	FILE* f;

	if (dwBufferLen < MAX_D2PATH_ABSOLUTE)
	{
		return false;
	}

	FS_SanitizeFilePath(szFileName);

	for (int i = FS_MAXPATH-1; i >= 0; i--)	// go in reverse since we're reading
	{
		D2_strncpyz(szBuffer, pszPaths[i], dwBufferLen);
		strcat(szBuffer, szFileName);
		f = fopen(szBuffer, "r");
		if (f != nullptr)
		{	// at this point, szBuffer will point to the correct file path
			fclose(f);
			return true;
		}
	}
	return false;	// didn't find it
}

/*
 *	Gets a list of files available in a directory.
 *	The file list must be freed after use with FS_FreeFileList
 *	The extension filter may use an asterisk (*) as a wild card (this is handled by the OS-specific code)
 *	@author	eezstreet
 */
char** FS_ListFilesInDirectory(char* szDirectory, char* szExtensionFilter, int *nFiles)
{
	char szFiles[MAX_FILE_LIST_SIZE][MAX_D2PATH_ABSOLUTE]{ 0 };
	char** szOutFiles;
	int i;
	char szCurrentSearchPath[MAX_D2PATH_ABSOLUTE]{ 0 };

	FS_SanitizeFilePath(szDirectory);

	// cycle through all of the search paths
	for (i = 0; i < FS_MAXPATH; i++)
	{
		snprintf(szCurrentSearchPath, MAX_D2PATH_ABSOLUTE, "%s%s", pszPaths[i], szDirectory);
		Sys_ListFilesInDirectory(szCurrentSearchPath, szExtensionFilter, szDirectory, nFiles, &szFiles);
	}

	if (*nFiles == 0)
	{
		return nullptr;
	}

	// copy the temporary files into the output files
	szOutFiles = (char**)malloc(sizeof(char*) * (*nFiles));
	for (i = 0; i < *nFiles; i++)
	{
		szOutFiles[i] = (char*)malloc(sizeof(char*) * MAX_D2PATH_ABSOLUTE);
		D2_strncpyz(szOutFiles[i], szFiles[i], MAX_D2PATH_ABSOLUTE);
	}

	return szOutFiles;
}

/*
 *	Frees a file list. Don't forget to do this once you're done with a file list!
 *	@author	eezstreet
 */
void FS_FreeFileList(char** pszFileList, int nNumFiles)
{
	for (int i = 0; i < nNumFiles; i++)
	{
		free(pszFileList[i]);
	}
	free(pszFileList);
}

/*
 *	Creates a subdirectory at the first available searchpath
 *	@author	eezstreet
 */
void FS_CreateSubdirectory(char* szSubdirectory)
{
	char szPath[MAX_D2PATH_ABSOLUTE]{ 0 };

	for (int i = 0; i < FS_MAXPATH; i++)
	{
		D2_strncpyz(szPath, pszPaths[i], MAX_D2PATH_ABSOLUTE);
		strcat(szPath, szSubdirectory);
		if (Sys_CreateDirectory(szPath))
		{
			return;
		}
	}
}