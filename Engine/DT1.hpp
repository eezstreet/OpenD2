#pragma once
#include "../Shared/D2Shared.hpp"

typedef void (*DT1AllocateCallback)(unsigned int width, unsigned int height);
typedef void (*DT1TileDecodeCallback)(BYTE* bitmap, unsigned int tileNum, unsigned int width, unsigned int height);

struct DT1File 
{
	DWORD x1, x2;
	DWORD numberOfBlocks;

	struct DT1Block
	{
		long direction;
		WORD roofY;
		BYTE sound;
		BYTE animated;
		long sizeY;
		long sizeX;
		long orientation;
		long mainIndex;
		long subIndex;
		long rarity;
		BYTE subtileFlags[25];
		long tilePtr;
		long tileLen;
		long tileNumber;
	};

	struct DT1Subtile
	{
		WORD xPosition;
		WORD yPosition;
		WORD unk1;
		BYTE xGrid;
		BYTE yGrid;
		WORD format;
		long length;
		WORD unk2;
		long dataOffset;
	};

	DT1Block* blocks;
	BYTE* fileBuffer;
	BYTE* decodedBitmap;
	size_t decodedSize;

	void* DecodeBlock(int32_t blockNum, uint32_t& width, uint32_t& height, int32_t& x, int32_t& y);
	DT1Subtile* GetSubtileAt(const DT1Block& block, int subtileNum);

	DT1File(const char* path);
	~DT1File();
};

namespace DT1
{
	handle LoadDT1(const char* path);
	DWORD GetNumBlocks(handle dt1Handle);
	const DT1File::DT1Block* GetBlock(handle dt1Handle, int blockNum);
	void* DecodeDT1Block(handle dt1Handle, int32_t blockNum, uint32_t& width, uint32_t& height, int32_t& x, int32_t& y);
}