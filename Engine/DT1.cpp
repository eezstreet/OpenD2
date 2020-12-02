#include "DT1.hpp"
#include "../Shared/D2HashMap.hpp"
#include "FileSystem.hpp"

namespace DT1
{
	HashMap<char, DT1File*> loadedDT1Files;

	handle LoadDT1(const char* path)
	{
		handle returnValue;
		bool isFull = false;
		if (loadedDT1Files.Contains(path, &returnValue, &isFull))
		{
			return returnValue;
		}
		else if (isFull)
		{
			return INVALID_HANDLE;
		}

		returnValue = loadedDT1Files.NextFree(path);
		loadedDT1Files.Insert(returnValue, path, new DT1File(path));
		return returnValue;
	}

	DWORD GetNumBlocks(handle dt1Handle)
	{
		if (dt1Handle == INVALID_HANDLE)
		{
			return 0;
		}

		DT1File* file = loadedDT1Files[dt1Handle];
		if (file == nullptr)
		{
			return 0;
		}

		return file->numberOfBlocks;
	}

	const DT1File::DT1Block* GetBlock(handle dt1Handle, int blockNum)
	{
		if (dt1Handle == INVALID_HANDLE || blockNum < 0)
		{
			return nullptr;
		}
		
		DT1File* file = loadedDT1Files[dt1Handle];
		if (file == nullptr)
		{
			return nullptr;
		}

		if (blockNum >= file->numberOfBlocks)
		{
			return nullptr;
		}

		return &file->blocks[blockNum];
	}

	void* DecodeDT1Block(handle dt1Handle, int32_t blockNum, uint32_t& width, uint32_t& height, int32_t& x, int32_t& y)
	{
		if (dt1Handle == INVALID_HANDLE || blockNum < 0)
		{
			return nullptr;
		}

		DT1File* file = loadedDT1Files[dt1Handle];
		if (file == nullptr)
		{
			return nullptr;
		}

		if (blockNum >= file->numberOfBlocks)
		{
			return nullptr;
		}

		return file->DecodeBlock(blockNum, width, height, x, y);
	}
}

const int indexTable[25] = {
	20, 21, 22, 23, 24,
	15, 16, 17, 18, 19,
	10, 11, 12, 13, 14,
	 5,  6,  7,  8,  9,
	 0,  1,  2,  3,  4,
};

DT1File::DT1File(const char* path) : 
	blocks(nullptr), 
	fileBuffer(nullptr), 
	numberOfBlocks(0), 
	x1(0), 
	x2(0), 
	decodedBitmap(nullptr),
	decodedSize(0)
{
	fs_handle f;
	size_t dt1Size = FS::Open(path, &f, FS_READ, true);
	if (f == INVALID_HANDLE || dt1Size <= 0)
	{
		return;
	}

	fileBuffer = (BYTE*)malloc(dt1Size);
	if (!fileBuffer)
	{
		return;
	}
	FS::Read(f, fileBuffer, dt1Size);
	FS::CloseFile(f);

	x1 = *(DWORD*)fileBuffer;
	x2 = *(DWORD*)((BYTE*)fileBuffer + 4);
	numberOfBlocks = *(DWORD*)((BYTE*)fileBuffer + 268);

	DWORD blockHeaderOffset = *(DWORD*)((BYTE*)fileBuffer + 272);
	size_t blockHeaderSize = sizeof(DT1Block) * numberOfBlocks;
	blocks = (DT1Block*)malloc(blockHeaderSize);
	if (blocks == nullptr)
	{
		free(fileBuffer);
		return;
	}

	// read block headers
	BYTE* readHead = (BYTE*)fileBuffer + blockHeaderOffset;
	DT1Block* blockWriteHead = blocks;
	for (DWORD b = 0; b < numberOfBlocks; b++)
	{
		blockWriteHead->direction = *(DWORD*)readHead;
		blockWriteHead->roofY = *(WORD*)(readHead + 4);
		blockWriteHead->sound = *(readHead + 6);
		blockWriteHead->animated = *(readHead + 7);
		blockWriteHead->sizeY = *(DWORD*)(readHead + 8);
		blockWriteHead->sizeX = *(DWORD*)(readHead + 12);
		// 4 bytes worth of zeroes
		blockWriteHead->orientation = *(DWORD*)(readHead + 20);
		blockWriteHead->mainIndex = *(DWORD*)(readHead + 24);
		blockWriteHead->subIndex = *(DWORD*)(readHead + 28);
		blockWriteHead->rarity = *(DWORD*)(readHead + 32);
		// skip 4 bytes
		for (int t = 0; t < 25; t++)
		{
			blockWriteHead->subtileFlags[indexTable[t]] = *(readHead + 40 + t);
		}
		// skip 7 bytes
		blockWriteHead->tilePtr = *(DWORD*)(readHead + 72);
		blockWriteHead->tileLen = *(DWORD*)(readHead + 76);
		blockWriteHead->tileNumber = *(DWORD*)(readHead + 80);
		// skip 12 bytes

		blockWriteHead++;
		readHead += 96;
	}
}

DT1File::~DT1File()
{
	if (blocks != nullptr)
	{
		free(blocks);
	}
	if (fileBuffer != nullptr)
	{
		free(fileBuffer);
	}
}

void* DT1File::DecodeBlock(int32_t blockNum, uint32_t& width, uint32_t& height, int32_t& x, int32_t& y)
{
	if (blockNum < 0 || blockNum >= numberOfBlocks || blocks == nullptr)
	{
		return nullptr;
	}

	DT1Block& block = blocks[blockNum];
	width = block.sizeX;
	height = -block.sizeY;

	// some orientations may modify the width and height. do that now.
	int32_t yAdd = 96;
	if (block.orientation == 0 || block.orientation == 15)
	{	// floor or roof
		if (block.sizeY)
		{
			block.sizeY = -80;
			height = 80;
			yAdd = 0;
		}
	}
	else if (block.orientation < 15)
	{	// upper wall, shadow, special
		if (block.sizeY)
		{
			block.sizeY += 32;
			height -= 32;
			yAdd = height;
		}
	}

	if (width == 0 || height == 0)
	{
		return decodedBitmap;
	}

	const size_t newSize = D2Lib::abs<size_t>((size_t)width * (size_t)height);

	if (decodedSize == 0)
	{
		// allocate original size.
		decodedSize = D2Lib::abs<size_t>(newSize);
		decodedBitmap = (BYTE*)malloc(decodedSize);
	}
	else if (newSize > decodedSize && decodedBitmap != nullptr)
	{
		free(decodedBitmap);
		decodedSize = newSize;
		decodedBitmap = (BYTE*)malloc(decodedSize);
	}
	
	if (decodedBitmap == nullptr)
	{
		return nullptr;
	}

	memset(decodedBitmap, 0, decodedSize);

	// Draw each subtile.
	for (int s = 0; s < block.tileNumber; s++)
	{
		DT1Subtile* subtile = GetSubtileAt(block, s);

		auto x0 = subtile->xPosition;
		auto y0 = yAdd + subtile->yPosition;
		BYTE* data = fileBuffer + block.tilePtr + subtile->dataOffset;
		auto length = subtile->length;
		auto format = subtile->format;

		if (format == 0x01)
		{
			// "3D isometric" per Paul Siramy - these are exactly 256 bytes
			if (length != 256)
			{
				continue;
			}

			int x, y = 0, n;
			int xjump[] = { 14, 12, 10, 8, 6, 4, 2, 0, 2, 4, 6, 8, 10, 12, 14 };
			int nbpx[] = { 4, 8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12, 8, 4 };
			while (length > 0)
			{
				x = xjump[y];
				n = nbpx[y];
				length -= n;
				while (n)
				{	// put *data at (x0+x,y0+y)
					decodedBitmap[(width * (y0 + y)) + (x0 + x)] = *data;
					data++;
					x++;
					n--;
				}
				y++;
			}
		}
		else
		{	// "normal" RLE encoding
			int x = 0, y = 0;

			while (length > 0)
			{
				BYTE b1 = *data;
				BYTE b2 = *(data + 1);
				data += 2;
				length -= 2;

				if (b1 || b2)
				{
					x += b1;
					length -= b2;
					while (b2)
					{
						decodedBitmap[(width * (y0 + y)) + (x0 + x)] = *data;
						data++;
						x++;
						b2--;
					}
				}
				else
				{
					x = 0;
					y++;
				}
			}
		}
	}

	if (block.orientation == 10 || block.orientation == 11)
	{
		// special tiles... fixme?
	}
}

DT1File::DT1Subtile* DT1File::GetSubtileAt(const DT1Block& block, int subtileNum)
{
	return (DT1Subtile*)(this->fileBuffer + block.tilePtr + (20 * subtileNum));
}