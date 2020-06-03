#include "DT1.hpp"
#include "FileSystem.hpp"
#include "Logging.hpp"

namespace DT1
{
	static BYTE* gGlobalDecodeBuffer = nullptr;
	static uint32_t gGlobalDecodeBufferWidth = 0;
	static uint32_t gGlobalDecodeBufferHeight = 0;

	void GlobalInit()
	{
		// do nothing here
	}

	void GlobalShutdown()
	{
		// free the decode buffer if it was allocated
		if (gGlobalDecodeBuffer != nullptr)
		{
			free(gGlobalDecodeBuffer);
		}
	}
	void LoadDT1(DT1File* file, const char* fileName)
	{
		if (!file || !fileName)
		{
			return;
		}

		fs_handle f;
		size_t fileSize = FS::Open(fileName, &f, FS_READ, true);
		file->fileBytes = (BYTE*)malloc(fileSize);

		FS::Read(f, file->fileBytes, fileSize);
		memcpy(&file->header, file->fileBytes, sizeof(file->header));

		FS::CloseFile(f);

		file->tileHeaders = (DT1TileHeader*)(file->fileBytes + file->header.dwTileHeaderOffset);
		DWORD numBlocks = 0;

		file->blockHeaders = (DT1BlockHeader*)(file->fileBytes + (file->header.dwTileHeaderOffset * sizeof(DT1TileHeader)));
		for (int i = 0; i < file->header.dwNumTiles; i++)
		{
			file->tileHeaders[i].dwBlockNumber = numBlocks;
			numBlocks += file->tileHeaders[i].dwNumBlocks;
		}
	}

	void UnloadDT1(DT1File* file)
	{
		if (!file)
		{
			return;
		}

		free(file->fileBytes);
	}

	void DecodeDT1(DT1File* file, int32_t startTile, int32_t endTile, TileDecodeCallback callback)
	{
		if (!file)
		{
			return;
		}

		if (endTile == -1)
		{	// if endTile == -1, use the last tile
			endTile = file->header.dwNumTiles - 1;
		}

		if (startTile > endTile)
		{	// swap start and end if start > end
			int32_t swap = endTile;
			endTile = startTile;
			startTile = swap;
		}

		uint32_t largestWidth = 0, largestHeight = 0;
		for (int32_t i = startTile; i <= endTile; i++)
		{
			// figure out how many pixels we need to allocate ahead of time
			if (file->tileHeaders[i].width > largestWidth)
			{
				largestWidth = file->tileHeaders[i].width;
				largestHeight = file->tileHeaders[i].height;
			}
		}

		if (largestWidth > gGlobalDecodeBufferWidth ||
			largestHeight > gGlobalDecodeBufferHeight)
		{
			gGlobalDecodeBufferWidth = D2Lib::max(largestWidth, gGlobalDecodeBufferWidth);
			gGlobalDecodeBufferHeight = D2Lib::max(largestHeight, gGlobalDecodeBufferHeight);

			if (gGlobalDecodeBuffer != nullptr)
			{
				free(gGlobalDecodeBuffer);
			}
			
			gGlobalDecodeBuffer = (BYTE*)malloc(gGlobalDecodeBufferWidth * gGlobalDecodeBufferHeight);
		}

		// decode the actual DT1 blocks
		for (int32_t i = startTile; i <= endTile; i++)
		{	// iterate through all tiles
			DT1TileHeader* tileHeader = &file->tileHeaders[i];
			memset(gGlobalDecodeBuffer, 0, gGlobalDecodeBufferWidth * gGlobalDecodeBufferHeight);

			for (uint32_t j = 0; j < tileHeader->dwNumBlocks; j++)
			{	// iterate through all blocks
				uint32_t blockIndex = j + tileHeader->dwBlockNumber;
				DT1BlockHeader* block = &file->blockHeaders[blockIndex];
				BYTE* inputBuffer = file->fileBytes + tileHeader->dwBlockHeaderOffset + block->fileOffset;
				DWORD length = block->encodingLength;
				int x = 0, y = 0;

				if (block->encodingFormat == 1)
				{
					// 3D-isometric floor block (RAW format, no transparency) -- Paul Siramy
					Log_WarnAssertVoidReturn(length == 256);

					int n;
					int xjump[15] = { 14, 12, 10,  8,  6,  4,  2,  0,  2,  4,  6,  8, 10, 12, 14 };
					int nbpix[15] = {  4,  8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12,  8,  4 };
					

					while (length > 0)
					{
						x = xjump[y];
						n = nbpix[y];
						length -= n;
						while (n)
						{
							gGlobalDecodeBuffer[x + (y * gGlobalDecodeBufferWidth)] = *inputBuffer;
							inputBuffer++;
							x++;
							n--;
						}
						y++;
					}
				}
				else
				{
					// RLE format, 32x32 pixels -- Paul Siramy
					while (length > 0)
					{
						BYTE b1, b2;
						b1 = *inputBuffer;
						b2 = *(inputBuffer + 1);
						inputBuffer += 2;
						length -= 2;
						if (b1 || b2)
						{
							x += b1;
							length -= b2;
							while (b2)
							{
								gGlobalDecodeBuffer[x + (y * gGlobalDecodeBufferWidth)] = *inputBuffer;
								inputBuffer++;
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

			if (callback)
			{	// what use is there in calling this if there's no callback?
				callback(gGlobalDecodeBuffer, tileHeader->width, tileHeader->height, 
					gGlobalDecodeBufferWidth, gGlobalDecodeBufferHeight, i, tileHeader);
			}
		}
	}
}
