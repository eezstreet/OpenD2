#include "DCC.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"

#define MAX_DCC_HASH		32768
#define MAX_DCC_NAMELEN		32

namespace DCC
{
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
	static void ReadHeader(DCCFile* pFile, Bitstream* pBits)
	{
		pBits->ReadByte(pFile->header.nSignature);
		pBits->ReadByte(pFile->header.nVersion);
		pBits->ReadByte(pFile->header.nNumberDirections);
		pBits->ReadDWord(pFile->header.dwFramesPerDirection);
		pBits->ReadDWord(pFile->header.dwTag);
		pBits->ReadDWord(pFile->header.dwFinalDC6Size);

		for (int i = 0; i < pFile->header.nNumberDirections; i++)
		{
			pBits->ReadDWord(pFile->header.dwDirectionOffset[i]);
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

	/**
	 *	Loads an animation from a file path.
	 *	@author eezstreet, SVR, Necrolis
	 */
	void LoadAnimation(const char* szPath, DCCFile* file)
	{
		Bitstream* pBits;
		DWORD dwFileSize;
		fs_handle f;
		memset(file, 0, sizeof(DCCFile));

		// Open the file
		file->dwFileSize = FS::Open(szPath, &f, FS_READ, true);

		Log_WarnAssert(f != INVALID_HANDLE);
		
		file->pFileBytes = (BYTE*)malloc(dwFileSize);
		FS::Read(f, file->pFileBytes, file->dwFileSize);
		FS::Close(f); // we don't actually need the file handle open any more

		pBits = new Bitstream();
		pBits->LoadStream(file->pFileBytes, file->dwFileSize);

		// Read the header
		ReadHeader(file, pBits);

		// Read each direction
		for(int i = 0; i < file->header.nNumberDirections; i++)
		{
			DCCDirection& dir = &file->directions[i];
			size_t optionalSize = 0;

			pBits->SetCurrentPosition(file->header.dwDirectionOffsets[i]);

			dir.nMinX = INT_MAX;
			dir.nMinY = INT_MAX;
			dir.nMaxX = INT_MIN;
			dir.nMaxY = INT_MIN;

			// Direction header
			ReadDirectionHeader(file->header, dir, i, pBits);

			// Read frames
			for(int j = 0; j < file->header.dwFramesPerDirection; j++)
			{
				// Read the header for this frame
				ReadFrameHeader(dir.frames[j], dir, pBits);

				// Recalculate box for this frame
				dir.nMinX = D2Lib::min<long>(dir.nMinX, dir.frames[j].nMinX);
				dir.nMinY = D2Lib::min<long>(dir.nMinY, dir.frames[j].nMinY);
				dir.nMaxX = D2Lib::max<long>(dir.nMaxX, dir.frames[j].nMaxX);
				dir.nMaxY = D2Lib::max<long>(dir.nMaxY, dir.frames[j].nMaxY);

				// Add to optional bytes size
				optionalSize += dir.frames[j].dwOptionalBytes;
			}

			// Read direction optional data
			if(optionalSize > 0)
			{
				for(int j = 0; j < file->header.dwFramesPerDirection; j++)
				{
					dir.frames[j].pOptionalByteData = (BYTE*)malloc(dir.frames[j].dwOptionalBytes);
					pBits->ReadData(dir.frames[j].pOptionalByteData, dir.frames[j].dwOptionalBytes);
				}
			}

			// Read size for pixel bitstream
			if(dir.nCompressionFlag & 0x02)
			{
				pBits->ReadBits(dir.dwEqualCellStreamSize, 20);
			}
			pBits->ReadBits(dir.dwPixelMaskStreamSize, 20);
			if(dir.nCompressionFlag & 0x01)
			{
				pBits->ReadBits(dir.dwEncodingStreamSize, 20);
				pBits->ReadBits(dir.dwRawPixelStreamSize, 20);
			}
			
			// Read pixel mapping
			ReadDirectionPixelMapping(dir, pBits);
			
			// Initiate the bitstreams.
			CreateDirectionBitstreams(dir, pBits);

			// We don't do any direction decoding for right now. These are gathered on-the-fly.
		}

		delete pBits;
	}

	/**
	 *	Unloads an animation.
	 *	@author eezstreet, SVR, Necrolis
	 */
	void UnloadAnimation(DCCFile* animation)
	{
		if(animation == nullptr)
		{
			return;
		}

		for(int i = 0; i < animation->header.nNumberDirections; i++)
		{
			for(int j = 0; j < animation->header.dwFramesPerDirection; j++)
			{
				if(animation->directions[i].frames[j].pOptionalByteData)
				{
					free(animation->directions[i].frames[j].pOptionalByteData);
				}
			}
		}
		free(animation->pFileBytes);
	}

	/**
	 *	Decodes a direction.
	 *	@author	eezstreet, SVR, Paul Siramy, Necrolis, Bilian Belchev
	 */
	void DecodeDirection(DCCFile* animation, uint32_t direction, DCCDirectionFrameDecodeCallback callback)
	{
		if(animation == nullptr)
		{
			return;
		}

		if(direction >= animation->header.nNumberDirections)
		{
			return;
		}

		DCCDirection* pDirection = &animation->directions[direction];

		// Create a buffer containing the cells for this direction
		int nDirectionW = pDirection->nMaxX - pDirection->nMinX + 1;
		int nDirectionH = pDirection->nMaxY - pDirection->nMinY + 1;
		int nDirCellW = (nDirectionW >> 2) + 10;
		int nDirCellH = (nDirectionH >> 2) + 10;
		DWORD dwNumCellsThisDir = nDirCellW * nDirCellH;

		// Global cell buffer
		DCCCell** ppCellBuffer = new DCCCell*[dwNumCellsThisDir];
		memset(ppCellBuffer, 0, sizeof(ppCellBuffer));

		// Cell buffer for all frames
		DCCCell* pFrameCells[MAX_DCC_FRAMES];
		memset(pFrameCells, 0, sizeof(pFrameCells));

		// Rewind all of the associated streams
		pDir->RewindAllStreams();

		// First part: iterate through the frames and get the colors
		for(int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			DCCFrame* frame = &pDir->frames[f];

			// Calculate the frame size, and number of cells this frame
			int nFrameW = frame->dwWidth;
			int nFrameH = frame->dwHeight;
			int nFrameX = frame->nXOffset - pDirection->nMinX;
			int nFrameY = frame->nYOffset - pDirection->nMinY + 1;

			int nNumCellsW = DCC::GetCellCount(nFrameX, nFrameW);
			int nNumCellsH = DCC::GetCellCount(nFrameY, nFrameH);

		}
	}
}
