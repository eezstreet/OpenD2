#pragma once
#include "../Shared/D2Shared.hpp"
#include "Bitstream.hpp"

#define PAUL_SIRAMY_DCC // Use Paul's decoder
#ifdef PAUL_SIRAMY_DCC
#include "Bitmap.hpp"
struct DCCBitstream
{
	BYTE* data; // pointer to the start of the bitstream
	DWORD size; // size of the bitstream, in bits
	DWORD currentByte; // byte cursor
	DWORD currentBit; // bit cursor in the byte of current byte cursor
					  //    (from 0 to 7 : lowest bit to highest bit)
	DWORD currentBitNum; // bit cursor in the bitstream (not in the byte)
						 //    this also indicate the # of bits already read
};

struct DCCPixelBufferEntry
{
	BYTE val[4];
	int frame, frameCellIndex;
};

typedef DCCPixelBufferEntry* DCCPixelBuffer;

#define DCC_MAX_PB_ENTRY 300000
#endif

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
#ifdef PAUL_SIRAMY_DCC
	long			nBoxW, nBoxH;
	struct DCCCell*		cells;
#endif
	DWORD			dwCellW, dwCellH;
	//////////////////////////////////

	BYTE*			pOptionalByteData;
};

#define MAX_DCC_PIXEL_BUFFER	300000
#define MAX_DCC_FRAMES			2048
#ifndef PAUL_SIRAMY_DCC
struct DCCPixelBuffer
{
	BYTE			pixel[4];
	int				frame;
	int				frameCellIndex;
};
#endif

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
#ifdef PAUL_SIRAMY_DCC
	long			nWidth, nHeight;
	long			dataOffset;
	long			nNumCellsW, nNumCellsH;
	DWORD			paletteBitstreamOffset;
	DCCPixelBuffer  pb;
	Bitmap*			bmp;
	long			nNumPixelBufferEntries;
#endif
	//////////////////////////////////

	DWORD			dwEqualCellStreamSize;	// only present when nCompressionFlag & 0x02
	DWORD			dwPixelMaskStreamSize;
	DWORD			dwEncodingStreamSize;	// only present when nCompressionFlag & 0x01
	DWORD			dwRawPixelStreamSize;	// only present when nCompressionFlag & 0x01

#ifdef PAUL_SIRAMY_DCC
	DWORD			dwEqualCellStreamOffset;
	DWORD			dwPixelMaskStreamOffset;
	DWORD			dwEncodingStreamOffset;
	DWORD			dwRawPixelStreamOffset;
	DWORD			dwPixelCodeStreamOffset;
#endif
	BYTE			nPixelValues[256];

	//////////////////////////////////
	//	Allocated when the DCC direction is created
#ifdef PAUL_SIRAMY_DCC
	DCCBitstream	EqualCellStream;
	DCCBitstream	PixelMaskStream;
	DCCBitstream	EncodingTypeStream;
	DCCBitstream	RawPixelStream;
	DCCBitstream	PixelCodeDisplacementStream;
	struct DCCCell*		CellBuffer;
#else
	Bitstream*		EqualCellStream;
	Bitstream*		PixelMaskStream;
	Bitstream*		EncodingTypeStream;
	Bitstream*		RawPixelStream;
	Bitstream*		PixelCodeDisplacementStream;
#endif

	//////////////////////////////////
	//	Functions to make our lives a bit easier
#ifndef PAUL_SIRAMY_DCC
	void RewindAllStreams()
	{
		if (EqualCellStream) { EqualCellStream->Rewind(); }
		if (PixelMaskStream) { PixelMaskStream->Rewind(); }
		if (EncodingTypeStream) { EncodingTypeStream->Rewind(); }
		if (RawPixelStream) { RawPixelStream->Rewind(); }
		if (PixelCodeDisplacementStream) { PixelCodeDisplacementStream->Rewind(); }
	}
#endif
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

#ifdef PAUL_SIRAMY_DCC
	bool			headerLoaded;

	int nXMin, nXMax, nYMin, nYMax;
	int width, height;
#endif
};

// Each frame in the DCC is composed of cells.
// Cells are (roughly) 4x4 blocks of pixels.
// The size is flexible; sometimes you can have a 5x4, etc.
struct DCCCell
{
#ifdef PAUL_SIRAMY_DCC
	int x0, y0, w, h, lastW, lastH, lastX0, lastY0;
	SubBitmap bmp;
#else
	int nX, nY, nW, nH;
	BYTE clrmap[4];
#endif
};

#pragma pack(pop, enter_include)

typedef void (*DCCDirectionFrameDecodeCallback)(BYTE* bitmap, uint32_t frameNum, int32_t frameX, int32_t frameY,
		uint32_t frameW, uint32_t frameH);
#ifdef PAUL_SIRAMY_DCC
typedef void(*DCCDirectionAllocateCallback)(unsigned int width, unsigned int height);
#endif

// DCC.cpp
namespace DCC
{
	void GlobalInit();
	void GlobalShutdown();
	void LoadAnimation(const char* szPath, DCCFile* file);
	void UnloadAnimation(DCCFile* animation);
#ifdef PAUL_SIRAMY_DCC
	void DecodeDirection(DCCFile* animation, uint32_t direction, DCCDirectionAllocateCallback alloc, DCCDirectionFrameDecodeCallback decode);
#else
	void DecodeDirection(DCCFile* animation, uint32_t direction, DCCDirectionFrameDecodeCallback callback);
	void GetDirectionSize(DCCFile* animation, uint32_t direction, uint32_t* frameSizeW, uint32_t* frameSizeH);
#endif

	DWORD GetCellCount(int pos, int& sz); 
};
