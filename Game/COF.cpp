#include "Diablo2.hpp"

#define MAX_COFFILE_NAMELEN	16
#define MAX_COF_TYPELEN		16
#define MAX_COF_HASHLEN		2048

struct COFHash
{
	char szCOFName[MAX_COFFILE_NAMELEN];
	char szCOFType[MAX_COF_TYPELEN];
	COFFile*	pFile;
	BYTE*		pCOFContents;
};

static COFHash COFHashTable[MAX_COF_HASHLEN]{ 0 };
static int nCOFHashUsed = 0;

const char* COFLayerNames[COMP_MAX] = {
	"HD", "TR", "LG", "RA", "LA", "RH", "LH", "SH",
	"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8"
};

/*
 *	Registers a COF (component object file).
 *	Type can either be: "chars", "monsters", or "objects"
 *	@author	eezstreet
 */
cof_handle COF_Register(char* type, char* token, char* animation, char* hitclass)
{
	char path[MAX_D2PATH]{ 0 };
	char cof[MAX_COFFILE_NAMELEN]{ 0 };
	COFHash* pHash;
	cof_handle outHandle;
	fs_handle file;
	D2MPQArchive* pArchive;
	size_t dwFileSize;

	// Make sure we aren't hitting the maximum size of the hash table
	Log_ErrorAssert(nCOFHashUsed < MAX_COF_HASHLEN, INVALID_HANDLE);

	snprintf(cof, MAX_COFFILE_NAMELEN, "%s%s%s.cof", token, animation, hitclass);
	snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\COF\\%s", type, token, cof);

	// Try and load the COF first because it's possible we could be wasting time with the below code
	file = FSMPQ_FindFile(path, nullptr, &pArchive);
	if (file == INVALID_HANDLE)
	{
		Log_Print(PRIORITY_MESSAGE, "Couldn't load %s\n", cof);
		return INVALID_HANDLE;
	}

	// Find the handle. If we collide in the hash table, just use linear probing
	outHandle = D2_strhash(cof, MAX_COFFILE_NAMELEN, MAX_COF_HASHLEN);
	pHash = &COFHashTable[outHandle];
	while (pHash->pFile != nullptr && D2_stricmp(pHash->szCOFName, cof))
	{
		outHandle++;
		outHandle %= MAX_COF_HASHLEN;
		pHash = &COFHashTable[outHandle];
	}

	// Allocate the file.
	pHash->pFile = (COFFile*)malloc(sizeof(COFFile));

	D2_strncpyz(pHash->szCOFName, cof, MAX_COFFILE_NAMELEN);
	D2_strncpyz(pHash->szCOFType, type, MAX_COF_TYPELEN);
	
	// Read the whole file at once
	dwFileSize = MPQ_FileSize(pArchive, file);
	Log_WarnAssert(dwFileSize != 0, INVALID_HANDLE);

	pHash->pCOFContents = (BYTE*)malloc(dwFileSize);
	Log_ErrorAssert(pHash->pCOFContents != nullptr, INVALID_HANDLE);

	MPQ_ReadFile(pArchive, file, pHash->pCOFContents, dwFileSize);

	// Copy the header over
	memcpy(&pHash->pFile->header, pHash->pCOFContents, sizeof(COFHeader));

	// Set the pointers for both the layers and the keyframes
	pHash->pFile->layers = (COFLayer*)(pHash->pCOFContents + sizeof(COFHeader));
	pHash->pFile->keyframes = (BYTE*)(pHash->pFile->layers + pHash->pFile->header.nLayers);

	return outHandle;
}

/*
 *	Deregisters a COF file.
 *	@author	eezstreet
 */
void COF_Deregister(cof_handle cof)
{
	COFHash* pHash = &COFHashTable[cof];

	if (pHash->pFile == nullptr)
	{
		// already deregistered
		return;
	}

	free(pHash->pFile);
	free(pHash->pCOFContents);
	pHash->pFile = nullptr;
	pHash->pCOFContents = nullptr;
	pHash->szCOFName[0] = '\0';
	pHash->szCOFType[0] = '\0';
}

/*
 *	Deregisters all COF of a specific category.
 *	We do this during inter-act loading to deregister all monster and object COFs but NOT on the player.
 *	@author	eezstreet
 */
void COF_DeregisterType(char* type)
{
	for (int i = 0; i < MAX_COF_HASHLEN; i++)
	{
		COFHash* pHash = &COFHashTable[i];
		if (pHash->pFile && pHash->szCOFType[0] && !D2_stricmp(type, pHash->szCOFType))
		{
			COF_Deregister((cof_handle)i);
		}
	}
}

/*
 *	Deregisters ALL COFs.
 *	@author	eezstreet
 */
void COF_DeregisterAll()
{
	for (int i = 0; i < MAX_COF_HASHLEN; i++)
	{
		COFHash* pHash = &COFHashTable[i];
		if (pHash->pFile)
		{
			COF_Deregister((cof_handle)i);
		}
	}
}