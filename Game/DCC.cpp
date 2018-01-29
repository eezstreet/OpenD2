#include "Diablo2.hpp"

#define MAX_DCC_HASH		32768
#define MAX_DCC_NAMELEN		32

struct DCCHash
{
	DCCFile*	pFile;
	char		name[MAX_DCC_NAMELEN];
	int			useCount;
};

static DCCHash DCCHashTable[MAX_DCC_HASH]{ 0 };
static int gnNumHashesUsed = 0;

static const DWORD gdwDCCBitTable[] = {
	0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32
};

/*
 *	Inits the DCC code globally. We should call this before doing any DCC calls.
 *	@author	eezstreet
 */
void DCC_GlobalInit()
{
	// do nothing atm
}

/*
 *	Shuts down the DCC code globally. We should call this once the game is shutting down.
 *	@author	eezstreet
 */
void DCC_GlobalShutdown()
{
	DCC_FreeAll();
}

/*
 *	Is responsible for reading the header of the DCC file.
 *	@author	eezstreet
 */
static void DCC_ReadHeader(DCCHash& dcc, Bitstream* pBits)
{
	pBits->ReadByte(dcc.pFile->header.nSignature);
	pBits->ReadByte(dcc.pFile->header.nVersion);
	pBits->ReadByte(dcc.pFile->header.nNumberDirections);
	pBits->ReadDWord(dcc.pFile->header.dwFramesPerDirection);
	pBits->ReadDWord(dcc.pFile->header.dwTag);
	pBits->ReadDWord(dcc.pFile->header.dwFinalDC6Size);

	for (int i = 0; i < dcc.pFile->header.nNumberDirections; i++)
	{
		pBits->ReadDWord(dcc.pFile->header.dwDirectionOffset[i]);
	}
}

/*
 *	Is reponsible for reading the header that starts each direction of the DCC.
 *	@author	eezstreet
 */
static void DCC_ReadDirectionHeader(DCCHeader& fileHeader, DCCDirection& dir, int dirNum, Bitstream* pBits)
{
	pBits->SetCurrentPosition(fileHeader.dwDirectionOffset[dirNum]);
	pBits->ReadBits(&dir.dwOutsizeCoded, 32);
	pBits->ReadBits(dir.nCompressionFlag, 2);
	pBits->ReadBits(dir.nVar0Bits, 4);
	pBits->ReadBits(dir.nWidthBits, 4);
	pBits->ReadBits(dir.nHeightBits, 4);
	pBits->ReadBits(dir.nXOffsetBits, 4);
	pBits->ReadBits(dir.nYOffsetBits, 4);
	pBits->ReadBits(dir.nOptionalBytesBits, 4);
	pBits->ReadBits(dir.nCodedBytesBits, 4);
}

/*
 *	Is responsible for reading the frame header on every DCC frame
 *	@author	eezstreet
 */
static void DCC_ReadFrameHeader(DCCFrame& frame, DCCDirection& direction, Bitstream* pBits)
{
	int width;
	void *ptr;
	size_t size = 4;

	memset(&frame, 0, sizeof(DCCFrame));

	for (int i = 0; i < 8; i++)
	{
		switch (i)
		{
			case 0:
				width = direction.nVar0Bits;
				ptr = &frame.dwVariable0;
				break;
			case 1:
				width = direction.nWidthBits;
				ptr = &frame.dwWidth;
				break;
			case 2:
				width = direction.nHeightBits;
				ptr = &frame.dwHeight;
				break;
			case 3:
				width = direction.nXOffsetBits;
				ptr = &frame.nXOffset;
				break;
			case 4:
				width = direction.nYOffsetBits;
				ptr = &frame.nYOffset;
				break;
			case 5:
				width = direction.nOptionalBytesBits;
				ptr = &frame.dwOptionalBytes;
				break;
			case 6:
				width = direction.nCodedBytesBits;
				ptr = &frame.dwCodedBytes;
				break;
			case 7:
				width = 1;
				ptr = &frame.dwFlipped;
				break;
		}

		if (gdwDCCBitTable[width] != 0)
		{
			pBits->ReadBits(ptr, size, gdwDCCBitTable[width]);
		}
	}

	// Calculate mins/maxs
	frame.nMinX = frame.nXOffset;
	frame.nMaxX = frame.nMinX + frame.dwWidth - 1;
	if (frame.dwFlipped)
	{
		frame.nMinY = frame.nYOffset;
		frame.nMaxY = frame.nMinY + frame.dwHeight - 1;
	}
	else
	{
		frame.nMaxY = frame.nYOffset;
		frame.nMinY = frame.nMaxY - frame.dwHeight + 1;
	}
}

/*
 *	Calculates information about cells in a frame: the number of cells present, how wide they are, etc.
 *	@author	eezstreet
 */
static void DCC_CalculateFrameCellData(DCCDirection& dir, DCCFrame& frame)
{
	int nCellWidth = 4 - ((frame.nMinX - dir.nMinX) % 4);
	int nCellHeight = 4 - ((frame.nMinY - dir.nMinY) % 4);
	DWORD dwCellsWide = 0;
	DWORD dwCellsHigh = 0;
	int temp;

	if (frame.dwWidth - nCellWidth <= 1)
	{	// the frame is very small, it can only be one cell wide
		dwCellsWide = 1;
	}
	else
	{	// give us some wiggle room on either side of the cell
		temp = frame.dwWidth - nCellWidth - 1;
		dwCellsWide = 2 + (temp / 4);
		if ((temp % 4) == 0)
		{
			dwCellsWide--;
		}
	}

	if (frame.dwHeight - nCellHeight <= 1)
	{	// the frame is very small, it can only be one cell high
		dwCellsHigh = 1;
	}
	else
	{	// give us some wiggle room on either side of the cell
		temp = frame.dwHeight - nCellHeight - 1;
		dwCellsHigh = 2 + (temp / 4);
		if ((temp % 4) == 0)
		{
			dwCellsHigh--;
		}
	}

	frame.dwCellW = dwCellsWide;
	frame.dwCellH = dwCellsHigh;
}

/*
 *	Is responsible for reading raw pixels on a direction
 *	@author	eezstreet
 */
static void DCC_ReadDirectionPixelMapping(DCCDirection& dir, Bitstream* pBits)
{
	int index = 0;
	BYTE value = 0;

	for (int i = 0; i < 256; i++)
	{
		pBits->ReadBits(value, 1);

		if (value)
		{
			dir.nPixelValues[index++] = value;
		}
	}
}

/*
 *	Is responsible for creating the DCC bitstreams
 *	@author	eezstreet
 */
static void DCC_CreateDirectionBitstreams(DCCDirection& dir, Bitstream* pBits)
{
	// Initialize everything to nullptr to begin with
	dir.EqualCellStream = nullptr;
	dir.PixelMaskStream = nullptr;
	dir.EncodingTypeStream = nullptr;
	dir.RawPixelStream = nullptr;
	dir.PixelCodeDisplacementStream = nullptr;

	if (dir.nCompressionFlag & 0x02)
	{
		dir.EqualCellStream = new Bitstream();
		dir.EqualCellStream->SplitFrom(pBits, dir.dwEqualCellStreamSize);
	}

	// Corresponds to `ColorMask` in SVR's code
	dir.PixelMaskStream = new Bitstream();
	dir.PixelMaskStream->SplitFrom(pBits, dir.dwPixelMaskStreamSize);

	if (dir.nCompressionFlag & 0x01)
	{
		dir.EncodingTypeStream = new Bitstream();
		dir.RawPixelStream = new Bitstream();	// Corresponds to `RawColors` in SVR's code
		dir.EncodingTypeStream->SplitFrom(pBits, dir.dwEncodingStreamSize);
		dir.RawPixelStream->SplitFrom(pBits, dir.dwRawPixelStreamSize);
	}

	// Read the remainder into the pixel code displacement
	// Corresponds to `PixelData` in SVR's code
	dir.PixelCodeDisplacementStream = new Bitstream();
	dir.PixelCodeDisplacementStream->SplitFrom(pBits, pBits->GetRemainingReadBits());
}

/*
 *	Is responsible for filling up the pixel buffer
 *	Unlike Paul Siramy's code, we use a global buffer to cut down on the number of allocations
 */
static DCCPixelBuffer gPixelBuffer[MAX_DCC_PIXEL_BUFFER]{ 0 };
static void DCC_FillPixelBuffer(DCCHeader* pHeader, DCCDirection* pDirection)
{
	DWORD dwCellsW, dwCellsH;
	DWORD dwSkipCell = 0, dwPixelMask = 0;

	memset(gPixelBuffer, 0, sizeof(DCCPixelBuffer) * MAX_DCC_PIXEL_BUFFER);

	// Each frame is divided up into a grid of "cells." These cells are normally 4x4 pixels wide.
	// HOWEVER, some of the time the cell may need to be extended. Normally this would be along the edge.

	for (int f = 0; f < pHeader->dwFramesPerDirection; f++)
	{
		dwCellsW = pDirection->frames[f].dwCellW;
		dwCellsH = pDirection->frames[f].dwCellH;

		for (int y = 0; y < dwCellsH; y++)
		{
			for (int x = 0; x < dwCellsW; x++)
			{
				// If the EqualCell bitstream is present, read a bit from it.
				dwSkipCell = 0;
				if (pDirection->dwEqualCellStreamSize > 0)
				{
					pDirection->EqualCellStream->ReadBits(dwSkipCell, 1);
				}
				
				// If EqualCell bitstream contained a '1' bit, then we can skip the current cell.
				if (dwSkipCell)
				{
					continue;
				}

				// Read the pixel mask
				pDirection->PixelMaskStream->ReadBits(dwPixelMask, 4);
			}
		}
	}
}

/*
 *	Is responsible for the actual reading of the DCC, from an fs_handle and a hash entry pointer.
 *	@author	eezstreet
 */
void DCC_Read(DCCHash& dcc, fs_handle fileHandle, D2MPQArchive* pArchive)
{
	Bitstream* pBits;
	int i, j;

	// Allocate memory for everything
	dcc.pFile = (DCCFile*)malloc(sizeof(DCCFile));
	Log_ErrorAssert(dcc.pFile);

	dcc.pFile->dwFileSize = MPQ_FileSize(pArchive, fileHandle);
	dcc.pFile->pFileBytes = (BYTE*)malloc(dcc.pFile->dwFileSize);
	Log_ErrorAssert(dcc.pFile->pFileBytes);

	// Read into filebytes. 
	MPQ_ReadFile(pArchive, fileHandle, dcc.pFile->pFileBytes, dcc.pFile->dwFileSize);

	// Create the bitstream.
	pBits = new Bitstream();
	pBits->LoadStream(dcc.pFile->pFileBytes, dcc.pFile->dwFileSize);

	// Read the header.
	DCC_ReadHeader(dcc, pBits);

	// Read the direction bitstream
	for (i = 0; i < dcc.pFile->header.nNumberDirections; i++)
	{
		DCCDirection& dir = dcc.pFile->directions[i];
		size_t optionalSize = 0;

		dir.nMinX = INT_MAX;
		dir.nMinY = INT_MAX;

		// Direction header
		DCC_ReadDirectionHeader(dcc.pFile->header, dir, i, pBits);

		// Read frames
		for (j = 0; j < dcc.pFile->header.dwFramesPerDirection; j++)
		{
			// Read header
			DCC_ReadFrameHeader(dir.frames[j], dir, pBits);

			// Recalculate box frame
			dir.nMinX = D2_min<long>(dir.nMinX, dir.frames[j].nMinX);
			dir.nMaxX = D2_max<long>(dir.nMaxX, dir.frames[j].nMaxX);
			dir.nMinY = D2_min<long>(dir.nMinY, dir.frames[j].nMinY);
			dir.nMaxY = D2_max<long>(dir.nMaxY, dir.frames[j].nMaxY);

			// Add to the optional bytes size
			optionalSize += dir.frames[j].dwOptionalBytes;
		}

		// Calculate width
		dir.nWidth = dir.nMaxX - dir.nMinX + 1;
		dir.nHeight = dir.nMaxY - dir.nMinY + 1;

		// Calculate frame cell width
		for (j = 0; j < dcc.pFile->header.dwFramesPerDirection; j++)
		{
			DCC_CalculateFrameCellData(dir, dir.frames[j]);
		}

		// Read direction optional data
		if (optionalSize > 0)
		{
			for (j = 0; j < dcc.pFile->header.dwFramesPerDirection; j++)
			{
				// Allocate memory and read
				dir.frames[j].pOptionalByteData = (BYTE*)malloc(dir.frames[j].dwOptionalBytes);
				pBits->ReadData(dir.frames[j].pOptionalByteData, dir.frames[j].dwOptionalBytes);
			}
		}

		// Read size for the pixel bitstreams
		if (dir.nCompressionFlag & 0x02)
		{
			pBits->ReadBits(dir.dwEqualCellStreamSize, 20);
		}
		pBits->ReadBits(dir.dwPixelMaskStreamSize, 20);
		if (dir.nCompressionFlag & 0x01)
		{
			pBits->ReadBits(dir.dwEncodingStreamSize, 20);
			pBits->ReadBits(dir.dwRawPixelStreamSize, 20);
		}

		// Read pixel mapping
		DCC_ReadDirectionPixelMapping(dir, pBits);

		// Initiate the bitstreams
		DCC_CreateDirectionBitstreams(dir, pBits);

		// That's all we need to do for now. 
		// The LRU on the renderer will be responsible for decoding the DCCs as we need them.
	}

	// Clear out the bitstream
	delete pBits;
}

/*
 *	Opens and reads a DCC file from an MPQ.
 *	Returns an anim_handle to the DCC in question.
 *	@author	eezstreet
 */
anim_handle DCC_Load(char* szPath, char* szName)
{
	anim_handle outHandle;
	fs_handle fileHandle;
	DWORD dwNameHash;
	D2MPQArchive* pArchive = nullptr;

	if (!szPath || !szName)
	{
		return INVALID_HANDLE;
	}

	if (gnNumHashesUsed >= MAX_DCC_HASH)
	{
		return INVALID_HANDLE;
	}

	// Make sure that the file actually exists first before we start poking the hash table.
	// That way, we can root out issues of not finding DCCs immediately
	fileHandle = FSMPQ_FindFile(szPath, nullptr, &pArchive);
	if (fileHandle == INVALID_HANDLE)
	{
		Log_Print(PRIORITY_DEBUG, "Couldn't load DCC file: %s (%s)\n", szPath, szName);
		return INVALID_HANDLE;
	}

	// Find a free slot in the hash table
	dwNameHash = D2_strhash(szName, 0, MAX_DCC_HASH);
	outHandle = (anim_handle)dwNameHash;
	while (DCCHashTable[outHandle].pFile != nullptr)
	{
		if (!D2_stricmp(szName, DCCHashTable[outHandle].name))
		{
			return outHandle;
		}
		outHandle++;
		outHandle %= MAX_DCC_HASH;
	}

	// Now that we've got a free slot and a file handle, let's go ahead and load the DCC itself
	DCC_Read(DCCHashTable[outHandle], fileHandle, pArchive);
	D2_strncpyz(DCCHashTable[outHandle].name, szName, MAX_DCC_NAMELEN);
	DCCHashTable[outHandle].useCount = 0;

	return outHandle;
}

/*
 *	Increment the use count of a DCC. Parameter can be negative to decrement.
 *	@author	eezstreet
 */
void DCC_IncrementUseCount(anim_handle dccHandle, int amount)
{
	if (dccHandle == INVALID_HANDLE)
	{
		return;
	}

	DCCHashTable[dccHandle].useCount += amount;
}

/*
 *	Gets the contents of the DCC file from a handle.
 *	@author	eezstreet
 */
DCCFile* DCC_GetContents(anim_handle dccHandle)
{
	if (dccHandle == INVALID_HANDLE)
	{
		return nullptr;
	}

	return DCCHashTable[dccHandle].pFile;
}

/*
 *	Free any DCC misc data (optional bytes, etc) that got allocated
 *	@author	eezstreet
 */
static void DCC_FreeMiscData(DCCFile* pFile)
{
	for (int i = 0; i < pFile->header.nNumberDirections; i++)
	{
		for (int j = 0; j < pFile->header.dwFramesPerDirection; j++)
		{
			if (pFile->directions[i].frames[j].pOptionalByteData)
			{
				free(pFile->directions[i].frames[j].pOptionalByteData);
			}
		}
	}
}

/*
 *	Free DCC at handle
 *	@author	eezstreet
 */
void DCC_FreeHandle(anim_handle dcc)
{
	if (dcc == INVALID_HANDLE || dcc >= MAX_DCC_HASH)
	{
		return;
	}

	if (DCCHashTable[dcc].pFile != nullptr)
	{
		DCCHashTable[dcc].name[0] = '\0';
		DCC_FreeMiscData(DCCHashTable[dcc].pFile);
		free(DCCHashTable[dcc].pFile->pFileBytes);
		free(DCCHashTable[dcc].pFile);
		DCCHashTable[dcc].pFile = nullptr;
	}
}

/*
 *	Frees a DCC if it is inactive
 *	@author	eezstreet
 */
void DCC_FreeIfInactive(anim_handle handle)
{
	if (DCCHashTable[handle].useCount <= 0)
	{
		DCC_FreeHandle(handle);
	}
}

/*
 *	Free all DCCs that aren't in use
 *	@author	eezstreet
 */
void DCC_FreeInactive()
{
	for (anim_handle i = 0; i < MAX_DCC_HASH; i++)
	{
		DCC_FreeIfInactive(i);
	}
}

/*
 *	Free a specific DCC file by name
 *	@author	eezstreet
 */
void DCC_FreeByName(char* name)
{
	DWORD dwHash;
	DWORD dwHashesTried = 0;

	if (!name)
	{
		return;
	}

	dwHash = D2_strhash(name, 0, MAX_DCC_HASH);
	while (dwHashesTried < gnNumHashesUsed && D2_stricmp(DCCHashTable[dwHash].name, name))
	{
		dwHash++;
		dwHash %= MAX_DCC_HASH;
		dwHashesTried++; // Increment number of hashes tried. If we go over the amount of hashes used, its wrong
	}

	if (dwHashesTried >= gnNumHashesUsed)
	{
		// Not found
		Log_Print(PRIORITY_DEBUG, "DCC not freed: %s\n", name);
		return;
	}

	DCC_FreeHandle((anim_handle)dwHash);
}

/*
 *	Frees all DCC files
 *	@author	eezstreet
 */
void DCC_FreeAll()
{
	for (anim_handle i = 0; i < MAX_DCC_HASH; i++)
	{
		DCC_FreeHandle(i);
	}
}

//////////////////////////////////////////////////
//
//	Helper functions for renderer decoding

/*
 *	Get the number of cells in a direction
 *	@author	SVR
 */
DWORD DCC_GetCellCount(int pos, int& sz)
{
	int nCells = 0;
	int first = (4 - (pos & 3));	// calc size of first cell

	if (sz <= (first + 1))	// not crossing boundry (by 2 or more)
		return 1;	// only one cell with size 5 or less

	sz -= first;	// subtract size of first cell
	nCells++;		// add first cell to count

	while (sz > 5)
	{	// count full cells until 5 or less
		sz -= 4;	// pixels remain
		nCells++;
	}

	nCells++;		// last cell width = sz
	return nCells;
}