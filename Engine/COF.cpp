#include "Diablo2.hpp"

#define MAX_COFFILE_NAMELEN	16
#define MAX_COF_TYPELEN		16
#define MAX_COF_HASHLEN		2048

const char* COFLayerNames[COMP_MAX] = {
	"HD", "TR", "LG", "RA", "LA", "RH", "LH", "SH",
	"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8"
};

namespace COF
{
	struct COFHash
	{
		char szCOFName[MAX_COFFILE_NAMELEN];
		char szCOFType[MAX_COF_TYPELEN];
		COFFile*	pFile;
		BYTE*		pCOFContents;
	};

	static COFHash COFHashTable[MAX_COF_HASHLEN]{ 0 };
	static int nCOFHashUsed = 0;

	/*
	*	Registers a COF (component object file).
	*	Type can either be: "chars", "monsters", or "objects"
	*	@author	eezstreet
	*/
	cof_handle Register(char* type, char* token, char* animation, char* hitclass)
	{
		char path[MAX_D2PATH]{ 0 };
		char cof[MAX_COFFILE_NAMELEN]{ 0 };
		COFHash* pHash;
		cof_handle outHandle;
		fs_handle file;
		size_t dwFileSize;
		int i;

		// Make sure we aren't hitting the maximum size of the hash table
		Log_ErrorAssertReturn(nCOFHashUsed < MAX_COF_HASHLEN, INVALID_HANDLE);

		snprintf(cof, MAX_COFFILE_NAMELEN, "%s%s%s.cof", token, animation, hitclass);
		snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\COF\\%s", type, token, cof);

		// Try and load the COF first because it's possible we could be wasting time with the below code
		dwFileSize = FS::Open(path, &file, FS_READ, true);
		if (file == INVALID_HANDLE)
		{
			Log::Print(PRIORITY_MESSAGE, "Couldn't load %s\n", cof);
			return INVALID_HANDLE;
		}

		if (dwFileSize == 0)
		{
			Log::Print(PRIORITY_MESSAGE, "Bad COF file: %s\n", cof);
			FS::CloseFile(file);
			return INVALID_HANDLE;
		}

		// Find the handle. If we collide in the hash table, just use linear probing
		outHandle = D2Lib::strhash(cof, MAX_COFFILE_NAMELEN, MAX_COF_HASHLEN);
		pHash = &COFHashTable[outHandle];
		while (pHash->pFile != nullptr && D2Lib::stricmp(pHash->szCOFName, cof))
		{
			outHandle++;
			outHandle %= MAX_COF_HASHLEN;
			pHash = &COFHashTable[outHandle];
		}

		// Allocate the file.
		pHash->pFile = (COFFile*)malloc(sizeof(COFFile));

		D2Lib::strncpyz(pHash->szCOFName, cof, MAX_COFFILE_NAMELEN);
		D2Lib::strncpyz(pHash->szCOFType, type, MAX_COF_TYPELEN);

		pHash->pCOFContents = (BYTE*)malloc(dwFileSize);
		Log_ErrorAssertReturn(pHash->pCOFContents != nullptr, INVALID_HANDLE);

		FS::Read(file, pHash->pCOFContents, dwFileSize);
		FS::CloseFile(file);

		// Copy the header over
		memcpy(&pHash->pFile->header, pHash->pCOFContents, sizeof(COFHeader));

		// Set the pointers for both the layers and the keyframes
		pHash->pFile->layers = (COFLayer*)(pHash->pCOFContents + sizeof(COFHeader));
		pHash->pFile->keyframes = (BYTE*)(pHash->pFile->layers + pHash->pFile->header.nLayers);

		// Iterate over the layers and mark off the components which are active.
		// By doing this once we can save a bit of time when trying to register the DCCs later
		pHash->pFile->dwLayersPresent = 0;
		for (i = 0; i < pHash->pFile->header.nLayers; i++)
		{
			pHash->pFile->dwLayersPresent |= (1 << pHash->pFile->layers[i].nComponent);
		}

		return outHandle;
	}

	/*
	*	Deregisters a COF file.
	*	@author	eezstreet
	*/
	void Deregister(cof_handle cof)
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
	void DeregisterType(char* type)
	{
		for (int i = 0; i < MAX_COF_HASHLEN; i++)
		{
			COFHash* pHash = &COFHashTable[i];
			if (pHash->pFile && pHash->szCOFType[0] && !D2Lib::stricmp(type, pHash->szCOFType))
			{
				Deregister((cof_handle)i);
			}
		}
	}

	/*
	*	Deregisters ALL COFs.
	*	@author	eezstreet
	*/
	void DeregisterAll()
	{
		for (int i = 0; i < MAX_COF_HASHLEN; i++)
		{
			COFHash* pHash = &COFHashTable[i];
			if (pHash->pFile)
			{
				Deregister((cof_handle)i);
			}
		}
	}

	/*
	*	Returns true if a layer is active on this COF.
	*	@author	eezstreet
	*/
	bool LayerPresent(cof_handle cof, int layer)
	{
		COFHash* pHash;

		if (cof == INVALID_HANDLE || cof >= MAX_COF_HASHLEN)
		{	// invalid COF handle
			return false;
		}

		if (layer < 0 || layer >= COMP_MAX)
		{	// of course it's not active! it's not a valid layer!
			return false;
		}

		pHash = &COFHashTable[cof];
		if (!pHash->pFile)
		{	// COF file didn't load correctly?
			return false;
		}

		return pHash->pFile->dwLayersPresent & (1 << layer);
	}

	/*
	*	Retrieves data about a particular COF file
	*	@author	eezstreet
	*/
	COFFile* GetFileData(cof_handle handle)
	{
		COFHash* pHash;

		if (handle == INVALID_HANDLE)
		{
			return nullptr;
		}

		pHash = &COFHashTable[handle];
		return pHash->pFile;
	}
}