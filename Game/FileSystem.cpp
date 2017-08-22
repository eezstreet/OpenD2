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
 *	homepath: First read, first write. Corresponds to ~ on Linux and My Documents/My Games/OpenD2 on Windows.
 *	basepath: Second read, second write. Corresponds to the working directory.
 *	modpath: Last read, last write. Defaults to the working directory.
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
		path[dwPathLen] = '/';
		path[dwPathLen + 1] = '\0';
	}
}

/*
 *	Initializes the OpenD2 filesystem
 *	@author	eezstreet
 */
void FS_Init(OpenD2ConfigStrc* pConfig)
{
	// Copy paths from the config to the FS
	D2_strncpyz(gszHomePath, pConfig->szHomePath, MAX_D2PATH_ABSOLUTE);
	D2_strncpyz(gszBasePath, pConfig->szBasePath, MAX_D2PATH_ABSOLUTE);
	D2_strncpyz(gszModPath, pConfig->szModPath, MAX_D2PATH_ABSOLUTE);

	// Make sure there's no garbage in the strings
	FS_SanitizeSearchPath(gszHomePath);
	FS_SanitizeSearchPath(gszBasePath);
	FS_SanitizeSearchPath(gszModPath);
}

/*
 *	Shuts down the file system
 *	@author	eezstreet
 */
void FS_Shutdown()
{
	FSHandleStore* pRecord = glFileHandles;
	FSHandleStore* pPrev = nullptr;

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
static void FS_SanitizeFilePath(char** path)
{
	// Remove leading slashes
	if (**path == '/')
	{
		(*path)++;
	}

	// Remove double slashes (//)
	for (int i = 0; i < strlen(*path); i++)
	{
		if (*path[i] == '/' && *path[i + 1] == '/')
		{	// found em
			D2_strncpyz(*path + i, *path + i + 1, MAX_D2PATH_ABSOLUTE);
		}
	}
}

/*
 *	Converts an FS_MODE into something that can be used by fopen, etc
 */
static const char* FS_ModeStr(OpenD2FileModes mode)
{
	switch (mode)
	{
		case FS_READ:
			return "r";
		case FS_WRITE:
			return "w";
		case FS_READWRITE:
			return "rw";
		case FS_APPEND:
			return "a";
	}
	return "";
}

/*
 *	Open a file with the select mode
 *	@return	The size of the file
 */
size_t FS_Open(char* filename, fs_handle* f, OpenD2FileModes mode)
{
	char path[MAX_D2PATH_ABSOLUTE]{ 0 };
	const char* szModeStr = FS_ModeStr(mode);

	FS_SanitizeFilePath(&filename);
	for (int i = 0; i < FS_MAXPATH; i++)
	{
		D2_strncpyz(path, pszPaths[i], MAX_D2PATH_ABSOLUTE);
		strcat(path, filename);

		if (*f = (fs_handle)fopen(path, szModeStr))
		{
			break;
		}
	}

	if (*f == 0)
	{	// file could not be found
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
	fseek((FILE*)f, 0, SEEK_END);
	dwLen = ftell((FILE*)f);
	rewind((FILE*)f);

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
size_t FS_Read(fs_handle f, char* buffer, size_t dwBufferLen)
{
	if (!FS_GetFileRecord(f))
	{
		// It's not something we read, so we aren't responsible for it!
		return 0;
	}

	return fread(buffer, 1, dwBufferLen, (FILE*)f);
}

/*
 *	Write to a file
 *	@return	The number of bytes written to the file
 */
size_t FS_Write(fs_handle f, char* buffer, size_t dwBufferLen)
{
	FSHandleStore* pRecord = FS_GetFileRecord(f);
	if (!pRecord || pRecord->mode == FS_READ)
	{
		// Not allowed to write to something which we opened in read mode
		return 0;
	}

	return fwrite(buffer, 1, dwBufferLen, (FILE*)f);
}

/*
 *	Writes to a file, more simply than the above.
 *	@return	The number of bytes written to the file.
 */
size_t FS_WriteSimple(fs_handle f, char* buffer)
{
	FSHandleStore* pRecord = FS_GetFileRecord(f);
	if (!pRecord || pRecord->mode == FS_READ)
	{
		// Not allowed to write to something which we opened in read mode
		return 0;
	}

	return fwrite(buffer, 1, strlen(buffer), (FILE*)f);
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
