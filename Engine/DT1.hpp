#pragma once
#include "../Shared/D2Shared.hpp"

#pragma pack(push,enter_include)
#pragma pack(1)
/**
 *	Main header of the DT1 file
 *	@author	Paul Siramy
 */
struct DT1Header
{
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	BYTE reserved[260];
	DWORD dwNumTiles;
	DWORD dwTileHeaderOffset;
};

/**
 *	DT1 subtile flags.
 */
struct DT1SubtileFlags
{
	bool bBlockWalk : 1;
	bool bBlockLOS : 1;
	bool bBlockJump : 1;
	bool bBlockOnlyPlayerWalk : 1;
	bool bUnk1 : 1;
	bool bBlockLight : 1;
	bool bUnk2 : 1;
	bool bUnk3 : 1;
};

/**
 *	Header for a file.
 *	@author	Paul Siramy, Necrolis
 */
struct DT1TileHeader
{
	DWORD dwDirection;
	WORD roofHeight;
	BYTE soundIndex;
	BYTE isAnimated;
	int32_t height;
	int32_t width;
	DWORD dwUnk1;
	DWORD dwTileType;
	DWORD dwTileStyle;
	DWORD dwTileSequence;
	DWORD dwRarity;
	DWORD dwUnk2;
	DT1SubtileFlags subtileFlags[32]; // the last 7 of these go unused
	DWORD dwBlockHeaderOffset;
	DWORD dwBlockLength;
	DWORD dwNumBlocks;
	DWORD dwBlockNumber;
	BYTE padding[8];
};

/**
 *	Header for a block.
 *	@author Paul Siramy
 */
struct DT1BlockHeader
{
	WORD xPosition;
	WORD yPosition;
	WORD padding1;
	BYTE gridX;
	BYTE gridY;
	WORD encodingFormat;
	DWORD encodingLength;
	WORD padding2;
	DWORD fileOffset;	// offset to point to raw file bytes
};
#pragma pack(pop, enter_include)

/**
 *	DT1 file itself
 *	@author eezstreet
 */
struct DT1File
{
	DT1Header header;
	DT1TileHeader* tileHeaders;
	DT1BlockHeader* blockHeaders;
	BYTE* fileBytes;
};

typedef void(*TileDecodeCallback)(BYTE* bitmap, uint32_t width, uint32_t height, uint32_t bufferWidth, uint32_t bufferHeight, int32_t tileNumber, DT1TileHeader* tileHeader);

namespace DT1
{
	void GlobalInit();
	void GlobalShutdown();
	void LoadDT1(DT1File* file, const char* fileName);
	void UnloadDT1(DT1File* file);
	void DecodeDT1(DT1File* file, int32_t startTile, int32_t endTile, TileDecodeCallback callback);
}
