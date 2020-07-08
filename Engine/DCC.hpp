#pragma once
#include "../Shared/D2Shared.hpp"
#include "Bitstream.hpp"

/*
 *	DCC Files
 *	Pieced together with code from Necrolis, SVR, and Paul Siramy
 */
#pragma pack(push,enter_include)
#pragma pack(1)
struct DCCHeader
{
	BYTE			nSignature;
	BYTE			nVersion;
	BYTE			nNumberDirections;
	DWORD			dwFramesPerDirection;
	DWORD			dwTag;
	DWORD			dwFinalDC6Size;
	DWORD			dwDirectionOffset[MAX_DIRECTIONS];
};

struct DCCFrame
{
	DWORD			dwVariable0;
	DWORD			dwWidth;
	DWORD			dwHeight;
	long			nXOffset;
	long			nYOffset;
	DWORD			dwOptionalBytes;
	DWORD			dwCodedBytes;
	DWORD			dwFlipped;

	//////////////////////////////////
	// Not actually in the file, these are calculated
	long			nMinX, nMaxX;
	long			nMinY, nMaxY;
	DWORD			dwCellW, dwCellH;
	//////////////////////////////////

	BYTE*			pOptionalByteData;
};

#define MAX_DCC_PIXEL_BUFFER	300000
#define MAX_DCC_FRAMES			2048
struct DCCPixelBuffer
{
	BYTE			pixel[4];
	int				frame;
	int				frameCellIndex;
};

struct DCCDirection
{
	DWORD			dwOutsizeCoded;
	BYTE			nCompressionFlag;
	BYTE			nVar0Bits;
	BYTE			nWidthBits;
	BYTE			nHeightBits;
	BYTE			nXOffsetBits;
	BYTE			nYOffsetBits;
	BYTE			nOptionalBytesBits;
	BYTE			nCodedBytesBits;

	DCCFrame		frames[MAX_FRAMES];

	//////////////////////////////////
	// Not actually in the file, these are calculated
	long			nMinX, nMaxX;
	long			nMinY, nMaxY;
	//////////////////////////////////

	DWORD			dwEqualCellStreamSize;	// only present when nCompressionFlag & 0x02
	DWORD			dwPixelMaskStreamSize;
	DWORD			dwEncodingStreamSize;	// only present when nCompressionFlag & 0x01
	DWORD			dwRawPixelStreamSize;	// only present when nCompressionFlag & 0x01
	BYTE			nPixelValues[256];

	//////////////////////////////////
	//	Allocated when the DCC direction is created
	Bitstream*		EqualCellStream;
	Bitstream*		PixelMaskStream;
	Bitstream*		EncodingTypeStream;
	Bitstream*		RawPixelStream;
	Bitstream*		PixelCodeDisplacementStream;

	//////////////////////////////////
	//	Functions to make our lives a bit easier
	void RewindAllStreams()
	{
		if (EqualCellStream) { EqualCellStream->Rewind(); }
		if (PixelMaskStream) { PixelMaskStream->Rewind(); }
		if (EncodingTypeStream) { EncodingTypeStream->Rewind(); }
		if (RawPixelStream) { RawPixelStream->Rewind(); }
		if (PixelCodeDisplacementStream) { PixelCodeDisplacementStream->Rewind(); }
	}
};

struct DCCFile
{
	// Part of the file structure
	DCCHeader		header;
	DCCDirection	directions[MAX_DIRECTIONS];

	// Other stuff used by OpenD2
	DWORD			dwFileSize;
	BYTE*			pFileBytes;
	int				nDirectionW[MAX_DIRECTIONS];
	int				nDirectionH[MAX_DIRECTIONS];
};

// Each frame in the DCC is composed of cells.
// Cells are (roughly) 4x4 blocks of pixels.
// The size is flexible; sometimes you can have a 5x4, etc.
struct DCCCell
{
	int nX, nY, nW, nH;
	BYTE clrmap[4];
};

#pragma pack(pop, enter_include)

typedef void (*DCCDirectionFrameDecodeCallback)(BYTE* bitmap, uint32_t frameNum, int32_t frameX, int32_t frameY,
		uint32_t frameW, uint32_t frameH);

// DCC.cpp
namespace DCC
{
	void GlobalInit();
	void GlobalShutdown();
	void LoadAnimation(const char* szPath, DCCFile* file);
	void UnloadAnimation(DCCFile* animation);
	void DecodeDirection(DCCFile* animation, uint32_t direction, DCCDirectionFrameDecodeCallback callback);
	void GetDirectionSize(DCCFile* animation, uint32_t direction, uint32_t* frameSizeW, uint32_t* frameSizeH);

	DWORD GetCellCount(int pos, int& sz); 
};
