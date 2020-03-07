#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	DC6 Files
 *	Pieced together from code by Paul Siramy and SVR.
 *	@author eezstreet
 */
#define MAX_DC6_CELL_SIZE	256

#pragma pack(push,enter_include)
#pragma pack(1)
struct DC6Frame
{
	struct DC6FrameHeader
	{
		DWORD	dwFlip;				// If true, it's encoded top to bottom instead of bottom to top
		DWORD	dwWidth;			// Width of this frame
		DWORD	dwHeight;			// Height of this frame
		DWORD	dwOffsetX;			// X offset
		DWORD	dwOffsetY;			// Y offset
		DWORD	dwUnknown;
		DWORD	dwNextBlock;
		DWORD	dwLength;			// Number of blocks to decode
	};
	DC6FrameHeader fh;

	DWORD dwDeltaY; // delta from previous frame (0 on first frame in direction)
};

struct DC6ImageHeader
{
	DWORD	dwVersion;			// Version; always '6'
	DWORD	dwUnk1;				// Unknown; always '1'
	DWORD	dwUnk2;				// Unknown; always '0'
	DWORD	dwTermination;		// Termination code - usually 0xEEEEEEEE or 0xCDCDCDCD
	DWORD	dwDirections;		// Number of directions
	DWORD	dwFrames;			// Number of frames per direction
};
#pragma pack(pop, enter_include)

struct DC6Image
{
	void*			mpq;			// The MPQ we found it in
	fs_handle		f;				// The file handle from within the MPQ
	DC6ImageHeader	header;
	DC6Frame*		pFrames;
	BYTE*			pPixels;
	DWORD			dwDirectionHeights[32];
	DWORD			dwTotalWidth;
	DWORD			dwTotalHeight;
	bool			bPixelsFreed;
	DWORD			dwMaxFrameWidth;
	DWORD			dwMaxFrameHeight;
};

// TODO: fix this
#ifdef LoadImage
#undef LoadImage
#endif // LoadImage

// DC6.cpp
namespace DC6
{
	void LoadImage(const char* szPath, DC6Image* pImage);
	void UnloadImage(DC6Image* pImage);
	BYTE* GetPixelsAtFrame(DC6Image* pImage, int nDirection, int nFrame, size_t* pNumPixels);
	void PollFrame(DC6Image* pImage, DWORD nDirection, DWORD nFrame,
		DWORD* dwWidth, DWORD* dwHeight, DWORD* dwOffsetX, DWORD* dwOffsetY);
	void StitchStats(DC6Image* pImage,
		DWORD dwStart, DWORD dwEnd, DWORD* pWidth, DWORD* pHeight, DWORD* pTotalWidth, DWORD* pTotalHeight);
	void AtlasStats(DC6Image* pImage, DWORD dwStart, DWORD dwEnd, DWORD* pTotalWidth, DWORD* pTotalHeight);
	void FreePixels(DC6Image* pImage);
};