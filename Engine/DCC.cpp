#include "DCC.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"

#define MAX_DCC_HASH		32768
#define MAX_DCC_NAMELEN		32

namespace DCC
{
	struct DCCHash
	{
		DCCFile*	pFile;
		char		name[MAX_DCC_NAMELEN];
		int			useCount;
	};

	static DCCHash DCCHashTable[MAX_DCC_HASH]{ 0 };
	static int gnNumHashesUsed = 0;

	static const BYTE gdwDCCBitTable[] = {
		0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32
	};

	/*
	*	Inits the DCC code globally. We should call this before doing any DCC calls.
	*	@author	eezstreet
	*/
	void GlobalInit()
	{
		// do nothing atm
	}

	/*
	*	Shuts down the DCC code globally. We should call this once the game is shutting down.
	*	@author	eezstreet
	*/
	void GlobalShutdown()
	{
		FreeAll();
	}

	/*
	*	Is responsible for reading the header of the DCC file.
	*	@author	eezstreet
	*/
	static void ReadHeader(DCCHash& dcc, Bitstream* pBits)
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
	static void ReadDirectionHeader(DCCHeader& fileHeader, DCCDirection& dir, int dirNum, Bitstream* pBits)
	{
		pBits->ReadBits(&dir.dwOutsizeCoded, 32);
		pBits->ReadBits(dir.nCompressionFlag, 2);
		pBits->ReadBits(dir.nVar0Bits, 4);
		pBits->ReadBits(dir.nWidthBits, 4);
		pBits->ReadBits(dir.nHeightBits, 4);
		pBits->ReadBits(dir.nXOffsetBits, 4);
		pBits->ReadBits(dir.nYOffsetBits, 4);
		pBits->ReadBits(dir.nOptionalBytesBits, 4);
		pBits->ReadBits(dir.nCodedBytesBits, 4);

		dir.nVar0Bits = gdwDCCBitTable[dir.nVar0Bits];
		dir.nWidthBits = gdwDCCBitTable[dir.nWidthBits];
		dir.nHeightBits = gdwDCCBitTable[dir.nHeightBits];
		dir.nXOffsetBits = gdwDCCBitTable[dir.nXOffsetBits];
		dir.nYOffsetBits = gdwDCCBitTable[dir.nYOffsetBits];
		dir.nOptionalBytesBits = gdwDCCBitTable[dir.nOptionalBytesBits];
		dir.nCodedBytesBits = gdwDCCBitTable[dir.nCodedBytesBits];
	}

	/*
	*	Is responsible for reading the frame header on every DCC frame
	*	@author	eezstreet
	*/
	static void ReadFrameHeader(DCCFrame& frame, DCCDirection& direction, Bitstream* pBits)
	{
		memset(&frame, 0, sizeof(DCCFrame));

#define ReadHeaderBits(x, y)	if(y != 0) pBits->ReadBits((DWORD*)&x, y);
		ReadHeaderBits(frame.dwVariable0, direction.nVar0Bits);
		ReadHeaderBits(frame.dwWidth, direction.nWidthBits);
		ReadHeaderBits(frame.dwHeight, direction.nHeightBits);
		ReadHeaderBits(frame.nXOffset, direction.nXOffsetBits);
		ReadHeaderBits(frame.nYOffset, direction.nYOffsetBits);
		ReadHeaderBits(*frame.pOptionalByteData, frame.dwOptionalBytes);
		ReadHeaderBits(frame.dwCodedBytes, direction.nCodedBytesBits);
		ReadHeaderBits(frame.dwFlipped, 1);
#undef ReadHeaderBits

		pBits->ConvertFormat(&frame.nXOffset, direction.nXOffsetBits);
		pBits->ConvertFormat(&frame.nYOffset, direction.nYOffsetBits);

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
	*	Is responsible for reading raw pixels on a direction
	*	@author	eezstreet
	*/
	static void ReadDirectionPixelMapping(DCCDirection& dir, Bitstream* pBits)
	{
		int index = 0;
		BYTE value = 0;

		memset(dir.nPixelValues, 0, sizeof(BYTE) * 256);
		for (int i = 0; i < 256; i++)
		{
			pBits->ReadBits(value, 1);

			if (value)
			{
				dir.nPixelValues[index++] = i;
			}
		}
	}

	/*
	*	Is responsible for creating the DCC bitstreams
	*	@author	eezstreet
	*/
	static void CreateDirectionBitstreams(DCCDirection& dir, Bitstream* pBits)
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
	*	Is responsible for the actual reading of the DCC, from an fs_handle and a hash entry pointer.
	*	@author	eezstreet
	*/
	void Read(DCCHash& dcc, fs_handle fileHandle, DWORD fileSize)
	{
		Bitstream* pBits;
		int i, j;

		// Allocate memory for everything
		dcc.pFile = (DCCFile*)malloc(sizeof(DCCFile));
		Log_ErrorAssert(dcc.pFile);

		dcc.pFile->dwFileSize = fileSize;
		dcc.pFile->pFileBytes = (BYTE*)malloc(dcc.pFile->dwFileSize);
		Log_ErrorAssert(dcc.pFile->pFileBytes);

		// Read into filebytes.
		FS::Read(fileHandle, dcc.pFile->pFileBytes, dcc.pFile->dwFileSize);

		// Create the bitstream.
		pBits = new Bitstream();
		pBits->LoadStream(dcc.pFile->pFileBytes, dcc.pFile->dwFileSize);

		// Read the header.
		ReadHeader(dcc, pBits);

		// Read the direction bitstream
		for (i = 0; i < dcc.pFile->header.nNumberDirections; i++)
		{
			DCCDirection& dir = dcc.pFile->directions[i];
			size_t optionalSize = 0;

			pBits->SetCurrentPosition(dcc.pFile->header.dwDirectionOffset[i]);

			dir.nMinX = INT_MAX;
			dir.nMinY = INT_MAX;
			dir.nMaxX = INT_MIN;
			dir.nMaxY = INT_MIN;

			// Direction header
			ReadDirectionHeader(dcc.pFile->header, dir, i, pBits);

			// Read frames
			for (j = 0; j < dcc.pFile->header.dwFramesPerDirection; j++)
			{
				// Read header
				ReadFrameHeader(dir.frames[j], dir, pBits);

				// Recalculate box frame
				dir.nMinX = D2Lib::min<long>(dir.nMinX, dir.frames[j].nMinX);
				dir.nMaxX = D2Lib::max<long>(dir.nMaxX, dir.frames[j].nMaxX);
				dir.nMinY = D2Lib::min<long>(dir.nMinY, dir.frames[j].nMinY);
				dir.nMaxY = D2Lib::max<long>(dir.nMaxY, dir.frames[j].nMaxY);

				// Add to the optional bytes size
				optionalSize += dir.frames[j].dwOptionalBytes;
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
			ReadDirectionPixelMapping(dir, pBits);

			// Initiate the bitstreams
			CreateDirectionBitstreams(dir, pBits);

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
	anim_handle Load(char* szPath, char* szName)
	{
		anim_handle outHandle;
		fs_handle fileHandle;
		DWORD dwNameHash;

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
		DWORD fileSize = FS::Open(szPath, &fileHandle, FS_READ, true);
		if (fileHandle == INVALID_HANDLE)
		{
			Log::Print(PRIORITY_DEBUG, "Couldn't load DCC file: %s (%s)\n", szPath, szName);
			return INVALID_HANDLE;
		}
		if (fileSize == 0)
		{
			Log::Print(PRIORITY_DEBUG, "Blank DCC file: %s (%s)\n", szPath, szName);
			FS::CloseFile(fileHandle);
			return INVALID_HANDLE;
		}

		// Find a free slot in the hash table
		dwNameHash = D2Lib::strhash(szName, 0, MAX_DCC_HASH);
		outHandle = (anim_handle)dwNameHash;
		while (DCCHashTable[outHandle].pFile != nullptr)
		{
			if (!D2Lib::stricmp(szName, DCCHashTable[outHandle].name))
			{
				return outHandle;
			}
			outHandle++;
			outHandle %= MAX_DCC_HASH;
		}

		// Now that we've got a free slot and a file handle, let's go ahead and load the DCC itself
		Read(DCCHashTable[outHandle], fileHandle, fileSize);
		D2Lib::strncpyz(DCCHashTable[outHandle].name, szName, MAX_DCC_NAMELEN);
		DCCHashTable[outHandle].useCount = 0;
		FS::CloseFile(fileHandle);

		return outHandle;
	}

	/*
	*	Increment the use count of a DCC. Parameter can be negative to decrement.
	*	@author	eezstreet
	*/
	void IncrementUseCount(anim_handle dccHandle, int amount)
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
	DCCFile* GetContents(anim_handle dccHandle)
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
	static void FreeMiscData(DCCFile* pFile)
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
	void FreeHandle(anim_handle dcc)
	{
		if (dcc == INVALID_HANDLE || dcc >= MAX_DCC_HASH)
		{
			return;
		}

		if (DCCHashTable[dcc].pFile != nullptr)
		{
			DCCHashTable[dcc].name[0] = '\0';
			FreeMiscData(DCCHashTable[dcc].pFile);
			free(DCCHashTable[dcc].pFile->pFileBytes);
			free(DCCHashTable[dcc].pFile);
			DCCHashTable[dcc].pFile = nullptr;
		}
	}

	/*
	*	Frees a DCC if it is inactive
	*	@author	eezstreet
	*/
	void FreeIfInactive(anim_handle handle)
	{
		if (DCCHashTable[handle].useCount <= 0)
		{
			FreeHandle(handle);
		}
	}

	/*
	*	Free all DCCs that aren't in use
	*	@author	eezstreet
	*/
	void FreeInactive()
	{
		for (anim_handle i = 0; i < MAX_DCC_HASH; i++)
		{
			FreeIfInactive(i);
		}
	}

	/*
	*	Free a specific DCC file by name
	*	@author	eezstreet
	*/
	void FreeByName(char* name)
	{
		DWORD dwHash;
		DWORD dwHashesTried = 0;

		if (!name)
		{
			return;
		}

		dwHash = D2Lib::strhash(name, 0, MAX_DCC_HASH);
		while (dwHashesTried < gnNumHashesUsed && D2Lib::stricmp(DCCHashTable[dwHash].name, name))
		{
			dwHash++;
			dwHash %= MAX_DCC_HASH;
			dwHashesTried++; // Increment number of hashes tried. If we go over the amount of hashes used, its wrong
		}

		if (dwHashesTried >= gnNumHashesUsed)
		{
			// Not found
			Log::Print(PRIORITY_DEBUG, "DCC not freed: %s\n", name);
			return;
		}

		FreeHandle((anim_handle)dwHash);
	}

	/*
	*	Frees all DCC files
	*	@author	eezstreet
	*/
	void FreeAll()
	{
		for (anim_handle i = 0; i < MAX_DCC_HASH; i++)
		{
			FreeHandle(i);
		}
	}

	//////////////////////////////////////////////////
	//
	//	Helper functions for renderer decoding

	/*
	*	Get the number of cells in a direction
	*	@author	SVR
	*/
	DWORD GetCellCount(int pos, int& sz)
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
}