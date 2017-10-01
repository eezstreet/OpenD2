#include "DRLG.hpp"

/*
 *	Creates a DRLGGrid with the appropriate size. Allocates the grid sectors also
 *	@author	eezstreet
 */
void DRLG_CreateGrid(DRLGGrid* pGrid, int nWidth, int nHeight)
{
	size_t dwTotalSectorSize = sizeof(DWORD) * nHeight * (nWidth + 1);
	int i, j;

	pGrid->nWidth = nWidth;
	pGrid->nHeight = nHeight;

	// allocate the grid sectors and initialize them
	pGrid->pGridSectors = (DWORD*)malloc(dwTotalSectorSize);
	memset(pGrid->pGridSectors, 0, dwTotalSectorSize);
	pGrid->pGridFlags = &pGrid->pGridSectors[nHeight];

	for (i = 0, j = 0; i < nHeight; i++)
	{
		pGrid->pGridSectors[i] = j;
		j += nWidth;
	}

	pGrid->bInited = false;	// ?
}

/*
 *	Creates a DRLGGrid from static memory (no allocations)
 *	@author	eezstreet
 */
void DRLG_CreateGrid_NoAllocations(DRLGGrid* pGrid, int nWidth, int nHeight, DWORD* pGridFlags, DWORD* pGridSectors)
{
	size_t dwTotalSectorSize = sizeof(DWORD) * nWidth * nHeight;

	pGrid->nWidth = nWidth;
	pGrid->nHeight = nHeight;
	pGrid->pGridFlags = pGridFlags;
	pGrid->pGridSectors = pGridSectors;

	for (int i = 0, j = 0; i < nHeight; i++, j += nWidth)
	{
		pGrid->pGridSectors[i] = j;
	}

	pGrid->bInited = false; // ?
}

/*
 *	Gets the grid flags at a specific x/y coordinate
 *	@author	eezstreet
 */
DWORD DRLG_GetGridFlags(DRLGGrid* pGrid, int x, int y)
{
	return pGrid->pGridFlags[x + pGrid->pGridSectors[y]];
}

/*
 *	Performs a math operation on grid flags
 *	@author	eezstreet
 */
void DRLG_DoGridFlagsMath(DRLGGrid* pGrid, int nOperand, D2MathFunc nOperator)
{
	int nCounter = 0;
	for (int i = 0; i < pGrid->nHeight; i++)
	{
		for (int j = 0; j < pGrid->nWidth; j++)
		{
			Math_Perform(nOperator, &pGrid->pGridFlags[pGrid->pGridSectors[nCounter++]], nOperand);
		}
	}
}