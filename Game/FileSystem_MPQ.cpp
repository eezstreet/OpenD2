#include "Diablo2.hpp"
#include <memory>

/*
 *	The MPQ file system is an extension of the original filesystem.
 *	Here, we need to load all of the MPQs that the game *needs* to use in order to run.
 *	Mods however can add additional MPQs to the search path.
 */

struct MPQSearchPath
{
	char szName[MAX_D2PATH];
	char szPath[MAX_D2PATH];
	D2MPQArchive* pArchive;
	MPQSearchPath* pNext;
};

// gpMPQSearchPaths is a stack, not a list.
// The first element of gpMPQSearchPaths is what gets searched for assets first (if no name is specified)
static MPQSearchPath* gpMPQSearchPaths;

/*
 *	Initializes the MPQ filesystem
 */
void FSMPQ_Init()
{

}

/*
 *	Shuts down the MPQ filesystem
 */
void FSMPQ_Shutdown()
{
	MPQSearchPath* pCurrent = gpMPQSearchPaths;
	MPQSearchPath* pPrev = nullptr;

	while (pCurrent != nullptr)
	{
		pPrev = pCurrent;
		pCurrent = pPrev->pNext;

		pPrev->pNext = nullptr;
		if (pPrev->pArchive != nullptr)
		{
			MPQ_CloseMPQ(pPrev->pArchive);
		}
		free(pPrev->pArchive);
		free(pPrev);
		pPrev = nullptr;
	}
}

/*
 *	Adds a single MPQ to the search path.
 *	@return	A pointer to the D2MPQArchive that got loaded
 */
D2MPQArchive* FSMPQ_AddSearchPath(char* szMPQName, char* szMPQPath)
{
	if (szMPQName == nullptr || szMPQPath == nullptr)
	{
		return nullptr;
	}

	MPQSearchPath* pNew = (MPQSearchPath*)malloc(sizeof(MPQSearchPath));
	if (pNew == nullptr)
	{	// couldn't allocate memory - die?
		return nullptr;
	}

	pNew->pArchive = (D2MPQArchive*)malloc(sizeof(D2MPQArchive));
	if (pNew->pArchive == nullptr)
	{	// couldn't allocate memory - die?
		return nullptr;
	}

	D2_strncpyz(pNew->szName, szMPQName, MAX_D2PATH);
	D2_strncpyz(pNew->szPath, szMPQPath, MAX_D2PATH);
	MPQ_OpenMPQ(szMPQPath, szMPQName, pNew->pArchive);
	if (pNew->pArchive == nullptr)
	{	// couldn't open MPQ
		free(pNew->pArchive);
		free(pNew);
		return nullptr;
	}

	pNew->pNext = gpMPQSearchPaths;
	gpMPQSearchPaths = pNew;

	return pNew->pArchive;
}

/*
 *	Look for a file from an archive
 *	We can either explicitly say which MPQ we want to look from, or pass in nullptr to look in all of them
 */
fs_handle FSMPQ_FindFile(char* szFileName, char* szMPQName, D2MPQArchive** pArchiveOut)
{
	MPQSearchPath* pCurrent = gpMPQSearchPaths;
	fs_handle f;

	while (pCurrent != nullptr)
	{
		if (szMPQName == nullptr || !D2_stricmp(szMPQName, pCurrent->szName))
		{
			if (f = MPQ_FetchHandle(pCurrent->pArchive, szFileName))
			{
				if (pArchiveOut != nullptr)
				{
					*pArchiveOut = pCurrent->pArchive;
				}
				return f;
			}
		}
		pCurrent = pCurrent->pNext;
	}
	return 0;	// invalid handle
}