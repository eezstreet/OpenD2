#include "DCC.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"
#include <Windows.h>

#define MAX_DCC_HASH		32768
#define MAX_DCC_NAMELEN		32

static const BYTE gdwDCCBitTable[] = {
	0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32
};

#ifdef PAUL_SIRAMY_DCC
namespace DCC
{
	void GlobalInit()
	{
	}

	void GlobalShutdown()
	{
	}

	void LoadAnimation(const char* szPath, DCCFile* file)
	{
		fs_handle f;

		if (file == nullptr)
		{
			return;
		}

		memset(file, 0, sizeof(DCCFile));

		file->dwFileSize = FS::Open(szPath, &f, OpenD2FileModes::FS_READ, true);
		if (f == INVALID_HANDLE)
		{
			return;
		}

		file->pFileBytes = (BYTE*)malloc(file->dwFileSize);
		if (file->pFileBytes == nullptr)
		{
			FS::CloseFile(f);
		}

		FS::Read(f, file->pFileBytes, file->dwFileSize);
		FS::CloseFile(f);
	}

	void UnloadAnimation(DCCFile* animation)
	{

	}

	static bool ReadBytes(DCCBitstream* bs, int bytesNumber, void* dest)
	{
		if (bs->currentBit)
		{
			return false;
		}

		for (int i = 0; i < bytesNumber; i++)
		{
			if (bs->currentBitNum >= bs->size)
			{
				return false;
			}

			*(((BYTE*)dest) + i) = bs->data[bs->currentByte];
			bs->currentByte++;
			bs->currentBitNum += 8;
		}

		return true;
	}

	static bool ReadBits(DCCBitstream* bs, int bitsNumber, bool isSigned, DWORD* dest)
	{
		int destBit = 0, destByte = 0;

		*dest = 0;
		if (bitsNumber < 0 || bitsNumber > 32)
		{
			return false;
		}

		for (int b = 0; b < bitsNumber; b++)
		{
			if (bs->currentBitNum >= bs->size)
			{
				return false;
			}

			if (bs->data[bs->currentByte] & (1 << bs->currentBit))
			{
				*(((BYTE*)dest) + destByte) |= (1 << destBit);
			}

			destBit++;
			if (destBit >= 8)
			{
				destBit = 0;
				destByte++;
			}

			bs->currentBit++;
			bs->currentBitNum++;
			if (bs->currentBit >= 8)
			{
				bs->currentBit = 0;
				bs->currentByte++;
			}
		}

		if (!isSigned)
		{
			return true;
		}

		if ((*dest) & (1 << (bitsNumber - 1)))
		{
			(*dest) |= ~((1 << bitsNumber) - 1);
		}

		return true;
	}

	static void LoadHeader(DCCFile* animation)
	{
		if (animation->headerLoaded)
		{
			return;
		}

		DCCBitstream bs;
		bs.currentBit = 0;
		bs.currentBitNum = 0;
		bs.currentByte = 0;
		bs.data = animation->pFileBytes;
		bs.size = 8 * animation->dwFileSize;

		if (!ReadBytes(&bs, 1, &animation->header.nSignature))
		{
			return;
		}
		if (!ReadBytes(&bs, 1, &animation->header.nVersion))
		{
			return;
		}
		if (!ReadBytes(&bs, 1, &animation->header.nNumberDirections))
		{
			return;
		}
		if (!ReadBytes(&bs, 4, &animation->header.dwFramesPerDirection))
		{
			return;
		}
		if (!ReadBytes(&bs, 4, &animation->header.dwTag))
		{
			return;
		}
		if (!ReadBytes(&bs, 4, &animation->header.dwFinalDC6Size))
		{
			return;
		}

		for (int i = 0; i < animation->header.nNumberDirections; i++)
		{
			if (!ReadBytes(&bs, 4, &animation->header.dwDirectionOffset[i]))
			{
				return;
			}
		}

		animation->headerLoaded = true;
	}

	static void FrameHeaderBitstream(DCCFile* animation, DCCBitstream* bs, int d, int f)
	{
		DCCDirection* dir = &animation->directions[d];
		DCCFrame* frame = &dir->frames[f];
		int sign, width = 0;
		void* ptr = nullptr;

		for (int i = 0; i < 8; i++)
		{
			sign = false;
			switch (i)
			{
				case 0:
					width = dir->nVar0Bits;
					ptr = &frame->dwVariable0;
					break;
				case 1:
					width = dir->nWidthBits;
					ptr = &frame->dwWidth;
					break;
				case 2:
					width = dir->nHeightBits;
					ptr = &frame->dwHeight;
					break;
				case 3:
					sign = true;
					width = dir->nXOffsetBits;
					ptr = &frame->nXOffset;
					break;
				case 4:
					sign = true;
					width = dir->nYOffsetBits;
					ptr = &frame->nYOffset;
					break;
				case 5:
					width = dir->nOptionalBytesBits;
					ptr = &frame->dwOptionalBytes;
					break;
				case 6:
					width = dir->nCodedBytesBits;
					ptr = &frame->dwCodedBytes;
					break;
				case 7:
					width = 1;
					ptr = &frame->dwFlipped;
					break;
			}

			if (!ReadBits(bs, gdwDCCBitTable[width], sign, (DWORD*)ptr))
			{
				return;
			}
		}

		frame->nMinX = frame->nXOffset;
		frame->nMaxX = frame->nMinX + frame->dwWidth - 1;

		if (frame->dwFlipped)
		{
			frame->nMinY = frame->nYOffset;
			frame->nMaxY = frame->nMinY + frame->dwHeight - 1;
		}
		else
		{
			frame->nMaxY = frame->nYOffset;
			frame->nMinY = frame->nMaxY - frame->dwHeight + 1;
		}

		frame->nBoxW = frame->nMaxX - frame->nMinX + 1;
		frame->nBoxH = frame->nMaxY - frame->nMinY + 1;
	}

	static void OptionalData(DCCFile* animation, DCCBitstream* bs, int direction)
	{
		DWORD size = 0;
		for (int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			size += animation->directions[direction].frames[f].dwOptionalBytes;
		}

		if (size == 0)
		{
			return;
		}

		if (bs->currentBit)
		{
			bs->currentBitNum += 8 - bs->currentBit;
			bs->currentBit = 0;
			bs->currentByte++;
		}

		for (int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			size = animation->directions[direction].frames[f].dwOptionalBytes;
			if (size)
			{
				animation->directions[direction].frames[f].pOptionalByteData = (BYTE*)malloc(size);
				if (animation->directions[direction].frames[f].pOptionalByteData == nullptr)
				{
					return;
				}

				if (!ReadBytes(bs, size, animation->directions[direction].frames[f].pOptionalByteData))
				{
					return;
				}
			}
		}
	}

	static void OtherBitstreamSize(DCCFile* animation, DCCBitstream* bs, int d)
	{
		if (animation->directions[d].nCompressionFlag & 0x02)
		{
			if (!ReadBits(bs, 20, false, &animation->directions[d].dwEqualCellStreamSize))
			{
				return;
			}
		}

		if (!ReadBits(bs, 20, false, &animation->directions[d].dwPixelMaskStreamSize))
		{
			return;
		}

		if (animation->directions[d].nCompressionFlag & 0x01)
		{
			if (!ReadBits(bs, 20, false, &animation->directions[d].dwEncodingStreamSize))
			{
				return;
			}

			if (!ReadBits(bs, 20, false, &animation->directions[d].dwRawPixelStreamSize))
			{
				return;
			}
		}
	}

	static void PixelValuesKey(DCCFile* animation, DCCBitstream* bs, int d)
	{
		animation->directions[d].paletteBitstreamOffset = (animation->header.dwDirectionOffset[d] * 8) + bs->currentBitNum;
		int idx = 0;
		DWORD tmp;

		for (int i = 0; i < 256; i++)
		{
			if (!ReadBits(bs, 1, false, &tmp))
			{
				return;
			}

			if (tmp)
			{
				animation->directions[d].nPixelValues[idx++] = (BYTE)i;
			}
		}
	}

	static void InitDirectionBitstream(DCCFile* animation, DCCBitstream* bs, int d)
	{
		DCCBitstream* equalCell, *pixelMask, *encodingType, *rawPixel, *pixelCodeDisplacement;
		DWORD glb_bit = (bs->currentByte * 8) + bs->currentBit;

		DCCDirection* dir = &animation->directions[d];
		equalCell = &dir->EqualCellStream;
		pixelMask = &dir->PixelMaskStream;
		encodingType = &dir->EncodingTypeStream;
		rawPixel = &dir->RawPixelStream;
		pixelCodeDisplacement = &dir->PixelCodeDisplacementStream;

		if (dir->nCompressionFlag & 0x02)
		{
			equalCell->data = bs->data + (glb_bit / 8);
			equalCell->currentBit = (BYTE)(glb_bit % 8);
			equalCell->currentBitNum = 0;
			equalCell->currentByte = 0;
			equalCell->size = dir->dwEqualCellStreamSize;
			dir->dwEqualCellStreamOffset = (animation->header.dwDirectionOffset[d] * 8) + glb_bit;
			glb_bit += equalCell->size;
		}

		pixelMask->data = bs->data + (glb_bit / 8);
		pixelMask->currentBit = (BYTE)(glb_bit % 8);
		pixelMask->currentBitNum = 0;
		pixelMask->currentByte = 0;
		pixelMask->size = dir->dwPixelMaskStreamSize;
		dir->dwPixelMaskStreamOffset = (animation->header.dwDirectionOffset[d] * 8) + glb_bit;
		glb_bit += pixelMask->size;

		if (dir->nCompressionFlag & 0x01)
		{
			encodingType->data = bs->data + (glb_bit / 8);
			encodingType->currentBit = (BYTE)(glb_bit % 8);
			encodingType->currentBitNum = 0;
			encodingType->currentByte = 0;
			encodingType->size = dir->dwEncodingStreamSize;
			dir->dwEncodingStreamOffset = (animation->header.dwDirectionOffset[d] * 8) + glb_bit;
			glb_bit += encodingType->size;

			rawPixel->data = bs->data + (glb_bit / 8);
			rawPixel->currentBit = (BYTE)(glb_bit % 8);
			rawPixel->currentBitNum = 0;
			rawPixel->currentByte = 0;
			rawPixel->size = dir->dwRawPixelStreamSize;
			dir->dwRawPixelStreamOffset = (animation->header.dwDirectionOffset[d] * 8) + glb_bit;
			glb_bit += rawPixel->size;
		}

		pixelCodeDisplacement->data = bs->data + (glb_bit / 8);
		pixelCodeDisplacement->currentBit = (BYTE)(glb_bit % 8);
		pixelCodeDisplacement->currentBitNum = 0;
		pixelCodeDisplacement->currentByte = 0;
		pixelCodeDisplacement->size = bs->size - glb_bit;
		dir->dwPixelCodeStreamOffset = (animation->header.dwDirectionOffset[d] * 8) + glb_bit;
	}

	static void PrepareBufferCells(DCCFile* animation, int direction)
	{
		DCCDirection* dir = &animation->directions[direction];
		DCCCell* cell;
		int bufferW, bufferH, tmp, numCellsW, numCellsH, numCells, size, x0, y0;
		int *cellW, * cellH;

		bufferW = dir->nWidth;
		bufferH = dir->nHeight;

		tmp = bufferW - 1;
		numCellsW = 1 + (tmp / 4);

		tmp = bufferH - 1;
		numCellsH = 1 + (tmp / 4);

		numCells = numCellsW * numCellsH;
		size = numCells * sizeof(DCCCell);
		dir->CellBuffer = (DCCCell*)malloc(size);
		if (dir->CellBuffer == nullptr)
		{
			return;
		}

		memset(dir->CellBuffer, 0, size);
		size = numCellsW * sizeof(int);
		cellW = (int*)malloc(size);
		if (cellW == nullptr)
		{
			free(dir->CellBuffer);
			return;
		}

		size = numCellsH * sizeof(int);
		cellH = (int*)malloc(size);
		if (cellH == nullptr)
		{
			free(dir->CellBuffer);
			free(cellW);
		}

		if (numCellsW == 1)
		{
			cellW[0] = bufferW;
		}
		else
		{
			for (int i = 0; i < numCellsW - 1; i++)
			{
				cellW[i] = 4;
			}

			cellW[numCellsW - 1] = bufferW - (4 * (numCellsW - 1));
		}

		if (numCellsH == 1)
		{
			cellH[0] = bufferH;
		}
		else
		{
			for (int i = 0; i < numCellsH - 1; i++)
			{
				cellH[i] = 4;
			}

			cellH[numCellsH - 1] = bufferH - (4 * (numCellsH - 1));
		}

		dir->nNumCellsW = numCellsW;
		dir->nNumCellsH = numCellsH;

		y0 = 0;
		for (int y = 0; y < numCellsH; y++)
		{
			x0 = 0;
			for (int x = 0; x < numCellsW; x++)
			{
				cell = &dir->CellBuffer[x + (y * numCellsW)];
				cell->w = cellW[x];
				cell->h = cellH[y];
				cell->bmp = SubBitmap(dir->bmp, x0, y0, cell->w, cell->h);
				x0 += 4;
			}

			y0 += 4;
		}

		free(cellW);
		free(cellH);
	}

	static void PrepareFrameCells(DCCFile* animation, int direction, int f)
	{
		DCCDirection* dir = &animation->directions[direction];
		DCCFrame* frame = &dir->frames[f];
		DCCCell* cell = nullptr;
		int frameW, frameH, w, h, tmp, numCellsW, numCellsH, numCells, size, x0, y0;
		int* cellW, *cellH;

		frameW = frame->nBoxW;
		frameH = frame->nBoxH;
		w = 4 - ((frame->nMinX - dir->nMinX) % 4);

		if (frameW - w <= 1)
		{
			numCellsW = 1;
		}
		else
		{
			tmp = frameW - w - 1;
			numCellsW = 2 + (tmp / 4);
			if ((tmp % 4) == 0)
			{
				numCellsW--;
			}
		}

		h = 4 - ((frame->nMinY - dir->nMinY) % 4);
		if (frameH - h <= 1)
		{
			numCellsH = 1;
		}
		else
		{
			tmp = frameH - h - 1;
			numCellsH = 2 + (tmp / 4);
			if ((tmp % 4) == 0)
			{
				numCellsH--;
			}
		}

		numCells = numCellsW * numCellsH;
		size = numCells * sizeof(DCCCell);
		frame->cells = (DCCCell*)malloc(size);
		if (frame->cells == nullptr)
		{
			return;
		}

		memset(frame->cells, 0, size);
		size = numCellsW * sizeof(int);
		cellW = (int*)malloc(size);
		if (cellW == nullptr)
		{
			return;
		}

		size = numCellsH * sizeof(int);
		cellH = (int*)malloc(size);
		if (cellH == nullptr)
		{
			free(cellW);
			return;
		}

		if (numCellsW == 1)
		{
			cellW[0] = frameW;
		}
		else
		{
			cellW[0] = w;
			for (int i = 1; i < numCellsW - 1; i++)
			{
				cellW[i] = 4;
			}
			cellW[numCellsW - 1] = frameW - w - (4 * (numCellsW - 2));
		}

		if (numCellsH == 1)
		{
			cellH[0] = frameH;
		}
		else
		{
			cellH[0] = h;
			for (int i = 1; i < numCellsH - 1; i++)
			{
				cellH[i] = 4;
			}
			cellH[numCellsH - 1] = frameH - h - (4 * (numCellsH - 2));
		}

		frame->dwCellW = numCellsW;
		frame->dwCellH = numCellsH;
		y0 = frame->nMinY - dir->nMinY;
		for (int y = 0; y < numCellsH; y++)
		{
			x0 = frame->nMinX - dir->nMinX;
			for (int x = 0; x < numCellsW; x++)
			{
				cell = &frame->cells[x + (y * numCellsW)];
				cell->x0 = x0;
				cell->y0 = y0;
				cell->w = cellW[x];
				cell->h = cellH[y];
				cell->bmp = SubBitmap(dir->bmp, x0, y0, cell->w, cell->h);
				x0 += cell->w;
			}

			y0 += cell->h;
		}

		free(cellW);
		free(cellH);
	}

	static const int gnNumPixTable[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

	static void FillPixelBuffer(DCCFile* animation, int direction, DCCDirectionAllocateCallback alloc)
	{
		DWORD tmp;
		DWORD encodingType = 0, pixelMask = 0;
		DWORD readPixel[4], lastPixel, numPixels, decodedPix, pixDisplace;
		int currentIdx;
		bool nextCell = false;
		DCCDirection* dir = &animation->directions[direction];
		size_t size = DCC_MAX_PB_ENTRY * sizeof(DCCPixelBufferEntry);
		DCCPixelBufferEntry* oldEntry, *newEntry;
		int pbIdx = -1;
		dir->pb = (DCCPixelBuffer)malloc(size);
		if (dir->pb == nullptr)
		{
			return;
		}

		memset(dir->pb, 0, size);
		for (int i = 0; i < DCC_MAX_PB_ENTRY; i++)
		{
			dir->pb[i].frame = -1;
			dir->pb[i].frameCellIndex = -1;
		}

		dir->bmp = new Bitmap(dir->nWidth, dir->nHeight);
		dir->bmp->Clear();

		alloc(dir->nWidth * animation->header.dwFramesPerDirection, dir->nHeight);

		PrepareBufferCells(animation, direction);

		int bufferW = dir->nNumCellsW;
		int bufferH = dir->nNumCellsH;
		int totalCells = bufferW * bufferH;
		size = totalCells * sizeof(DCCPixelBuffer);
		DCCPixelBuffer* cellBuffer = (DCCPixelBuffer*)malloc(size);
		if (cellBuffer == nullptr)
		{
			free(dir->pb);
			return;
		}

		memset(cellBuffer, 0, size);
		for (int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			PrepareFrameCells(animation, direction, f);

			DCCFrame* frame = &dir->frames[f];
			auto cellWidth = frame->dwCellW;
			auto cellHeight = frame->dwCellH;
			auto cell0X = (frame->nMinX - dir->nMinX) / 4;
			auto cell0Y = (frame->nMinY - dir->nMinY) / 4;

			for (int y = 0; y < cellHeight; y++)
			{
				auto currentCellY = cell0Y + y;
				for (int x = 0; x < cellWidth; x++)
				{
					auto currentCellX = cell0X + x;
					auto currentCell = currentCellX + (currentCellY * bufferW);
					if (currentCell >= totalCells)
					{
						return; // catchme
					}

					nextCell = false;
					if (cellBuffer[currentCell] != nullptr)
					{
						if (dir->dwEqualCellStreamSize)
						{
							if (!ReadBits(&dir->EqualCellStream, 1, false, &tmp))
							{
								return; //catchme
							}
						}
						else
						{
							tmp = 0;
						}

						if (tmp == 0)
						{
							if (!ReadBits(&dir->PixelMaskStream, 4, false, &pixelMask))
							{
								return; // catchme
							}
						}
						else
						{
							nextCell = true;
						}
					}
					else
					{
						pixelMask = 0x0F;
					}

					if (nextCell == false)
					{
						readPixel[0] = readPixel[1] = 0;
						readPixel[2] = readPixel[3] = 0;
						lastPixel = 0;
						numPixels = gnNumPixTable[pixelMask];
						if (numPixels && dir->dwEncodingStreamSize)
						{
							if (!ReadBits(&dir->EncodingTypeStream, 1, false, &encodingType))
							{
								return; // catchme
							}
						}
						else
						{
							encodingType = 0;
						}

						decodedPix = 0;
						for (int i = 0; i < numPixels; i++)
						{
							if (encodingType)
							{
								if (!ReadBits(&dir->RawPixelStream, 8, false, &readPixel[i]))
								{
									return; // catchme
								}
							}
							else
							{
								readPixel[i] = lastPixel;
								do
								{
									if (!ReadBits(&dir->PixelCodeDisplacementStream, 4, false, &pixDisplace))
									{
										return; // catchme
									}

									readPixel[i] += pixDisplace;
								} while (pixDisplace == 15);
							}

							if (readPixel[i] == lastPixel)
							{
								readPixel[i] = 0; // discard this pixel
								i = numPixels; // and stop decoding
							}
							else
							{
								lastPixel = readPixel[i];
								decodedPix++;
							}
						}

						oldEntry = cellBuffer[currentCell];
						pbIdx++;
						if (pbIdx >= DCC_MAX_PB_ENTRY)
						{
							free(cellBuffer);
							return;
						}

						newEntry = &dir->pb[pbIdx];
						currentIdx = decodedPix - 1;
						for (int i = 0; i < 4; i++)
						{
							if (pixelMask & (1 << i))
							{
								if (currentIdx >= 0)
								{
									newEntry->val[i] = (BYTE)readPixel[currentIdx--];
								}
								else
								{
									newEntry->val[i] = 0;
								}
							}
							else
							{
								newEntry->val[i] = oldEntry->val[i];
							}
						}

						cellBuffer[currentCell] = newEntry;
						newEntry->frame = f;
						newEntry->frameCellIndex = x + (y * cellWidth);
					}
				}
			}
		}

		for (int i = 0; i <= pbIdx; i++)
		{
			for (int x = 0; x < 4; x++)
			{
				int y = dir->pb[i].val[x];
				dir->pb[i].val[x] = dir->nPixelValues[y];
			}
		}

		dir->nNumPixelBufferEntries = pbIdx + 1;
		free(cellBuffer);
	}

	static void MakeFrames(DCCFile* dcc, int direction, bool minimize, DCCDirectionFrameDecodeCallback decoder)
	{
		DCCDirection* dir = &dcc->directions[direction];
		DCCPixelBuffer pbe = dir->pb;
		DCCFrame* frame;
		DCCCell* bufferCell, *cell;
		DWORD pix;
		int numCell, numBit, cellX, cellY, cellIdx;
		Bitmap* frameBitmap = new Bitmap(dir->nWidth, dir->nHeight);

		for (int c = 0; c < dir->nNumCellsW * dir->nNumCellsH; c++)
		{
			dir->CellBuffer[c].lastW = -1;
			dir->CellBuffer[c].lastH = -1;
		}

		for (int f = 0; f < dcc->header.dwFramesPerDirection; f++)
		{
			frameBitmap->Clear();
			frame = &dir->frames[f];
			numCell = frame->dwCellW * frame->dwCellH;

			for (int c = 0; c < numCell; c++)
			{	// after cell 16, things start to go awry.
				// why?
				cell = &frame->cells[c];
				cellX = cell->x0 / 4;
				cellY = cell->y0 / 4;
				cellIdx = cellX + (cellY * dir->nNumCellsW);
				bufferCell = &dir->CellBuffer[cellIdx];
				if (pbe->frame != f || pbe->frameCellIndex != c)
				{
					if (cell->w != bufferCell->lastW || cell->h != bufferCell->lastH)
					{
						cell->bmp.Clear();
					}
					else
					{
						Bitmap::Blit(dir->bmp, dir->bmp, cell->x0, cell->y0, bufferCell->lastX0, bufferCell->lastY0, cell->w, cell->h);
						SubBitmap::Blit(frameBitmap, cell->bmp, cell->x0, cell->y0, 0, 0, cell->w, cell->h);
					}
				}
				else
				{
					if (pbe->val[0] == pbe->val[1])
					{
						cell->bmp.Clear(pbe->val[0]);
					}
					else
					{
						if (pbe->val[1] == pbe->val[2])
						{
							numBit = 1;
						}
						else
						{
							numBit = 2;
						}

						for (int y = 0; y < cell->h; y++)
						{
							for (int x = 0; x < cell->w; x++)
							{
								ReadBits(&dir->PixelCodeDisplacementStream, numBit, false, &pix);
								*cell->bmp[{x, y}] = pbe->val[pix];
							}
						}
					}

					SubBitmap::Blit(frameBitmap, cell->bmp, cell->x0, cell->y0, 0, 0, cell->w, cell->h);
					pbe++;
				}

				bufferCell->lastW = cell->w;
				bufferCell->lastH = cell->h;
				bufferCell->lastX0 = cell->x0;
				bufferCell->lastY0 = cell->y0;
			}

			decoder(frameBitmap->Get(), f, dir->nWidth * f, 0, dir->bmp->GetW(), dir->bmp->GetH());
		}

		delete frameBitmap;
	}

	static void DirectionBitstream(DCCFile* animation, int direction, bool minimize, DCCDirectionAllocateCallback alloc, DCCDirectionFrameDecodeCallback decode)
	{
		DCCBitstream bs;

		bs.currentBit = 0;
		bs.currentBitNum = 0;
		bs.currentByte = 0;
		bs.data = animation->pFileBytes + animation->header.dwDirectionOffset[direction];
		if (direction == animation->header.nNumberDirections - 1)
		{
			bs.size = 8 * (animation->dwFileSize - animation->header.dwDirectionOffset[direction]);
		}
		else
		{
			bs.size = 8 * (animation->header.dwDirectionOffset[direction + 1] - animation->header.dwDirectionOffset[direction]);
		}

		auto dccDir = &animation->directions[direction];
		if (!ReadBits(&bs, 32, false, &dccDir->dwOutsizeCoded))
		{
			return;
		}
		if (!ReadBits(&bs, 2, false, (DWORD*)&dccDir->nCompressionFlag))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nVar0Bits))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nWidthBits))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nHeightBits))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nXOffsetBits))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nYOffsetBits))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nOptionalBytesBits))
		{
			return;
		}
		if (!ReadBits(&bs, 4, false, (DWORD*)&dccDir->nCodedBytesBits))
		{
			return;
		}

		dccDir->nMinX = dccDir->nMinY = 0x7FFFFFFFL;
		dccDir->nMaxX = dccDir->nMaxY = 0x80000000L;
		for (int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			FrameHeaderBitstream(animation, &bs, direction, f);
			if (dccDir->frames[f].nMinX < dccDir->nMinX)
			{
				dccDir->nMinX = dccDir->frames[f].nMinX;
			}

			if (dccDir->frames[f].nMinY < dccDir->nMinY)
			{
				dccDir->nMinY = dccDir->frames[f].nMinY;
			}

			if (dccDir->frames[f].nMaxX > dccDir->nMaxX)
			{
				dccDir->nMaxX = dccDir->frames[f].nMaxX;
			}

			if (dccDir->frames[f].nMaxY > dccDir->nMaxY)
			{
				dccDir->nMaxY = dccDir->frames[f].nMaxY;
			}
		}

		dccDir->nWidth = dccDir->nMaxX - dccDir->nMinX + 1;
		dccDir->nHeight = dccDir->nMaxY - dccDir->nMinY + 1;
		OptionalData(animation, &bs, direction);

		dccDir->dataOffset = (animation->header.dwDirectionOffset[direction] * 8) + bs.currentBitNum;
		OtherBitstreamSize(animation, &bs, direction);
		PixelValuesKey(animation, &bs, direction);
		InitDirectionBitstream(animation, &bs, direction);
		FillPixelBuffer(animation, direction, alloc);
		MakeFrames(animation, direction, false, decode);

		if (dccDir->pb != nullptr)
		{
			free(dccDir->pb);
			dccDir->pb = nullptr;
		}

		Log_WarnAssertReturn(dccDir->dwEqualCellStreamSize == dccDir->EqualCellStream.currentBitNum);
		Log_WarnAssertReturn(dccDir->dwPixelMaskStreamSize == dccDir->PixelMaskStream.currentBitNum);
		Log_WarnAssertReturn(dccDir->dwEncodingStreamSize == dccDir->EncodingTypeStream.currentBitNum);
		Log_WarnAssertReturn(dccDir->PixelCodeDisplacementStream.currentBitNum + 7 >= dccDir->PixelCodeDisplacementStream.size);
	}

	void DecodeDirection(DCCFile* animation, uint32_t direction, DCCDirectionAllocateCallback alloc, DCCDirectionFrameDecodeCallback decode)
	{
		animation->nXMin = animation->nYMin = 0x7FFFFFFFL; // highest long number
		animation->nXMax = animation->nYMax = 0x80000000L; // lowest  long number
		LoadHeader(animation);

		DirectionBitstream(animation, direction, true, alloc, decode);

		animation->width = animation->nXMax - animation->nXMin + 1;
		animation->height = animation->nYMax - animation->nYMin + 1;
	}
}
#else
namespace DCC
{

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
		// do nothing atm
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
		ReadHeaderBits(frame.dwOptionalBytes, direction.nOptionalBytesBits);
		ReadHeaderBits(frame.dwCodedBytes, direction.nCodedBytesBits);
		ReadHeaderBits(frame.dwFlipped, 1);
#undef ReadHeaderBits

		pBits->ConvertFormat(&frame.nXOffset, direction.nXOffsetBits);
		pBits->ConvertFormat(&frame.nYOffset, direction.nYOffsetBits);

		// Calculate mins/maxs
		frame.nMinX = frame.nXOffset;
		frame.nMaxX = frame.nMinX + frame.dwWidth;
		if (frame.dwFlipped)
		{
			frame.nMinY = frame.nYOffset;
			frame.nMaxY = frame.nYOffset + frame.dwHeight;
		}
		else
		{
			frame.nMinY = frame.nYOffset - frame.dwHeight + 1;
			frame.nMaxY = frame.nYOffset + 1;
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
#ifdef _WIN32
		else
		{
			printf("Direction bitstream created without EqualCellStream!\n");
		}
#endif

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
#ifdef _WIN32
		else
		{
			printf("Direction bitstream created without EncodingType or RawPixelStream !\n");
		}
#endif

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
		// Paul Siramy's method
		int w = 4 - (pos % 4);

		if ((sz - w) <= 1) {
			return 1;
		}

		int tmp = sz - w - 1;
		int out = 2 + (tmp / 4);
		if ((tmp % 4) == 0) {
			out--;
		}
		return out;
		// Necrolis' method
		/*int first = 4 - (pos & 3);
		if (sz <= first + 1)
			return 1;

		sz -= first;
		int nCells = (sz - 2) / 4;
		sz -= nCells * 4;
		return nCells + 2;*/

		// SVR's method
		/*int nCells = 0;
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
		return nCells;*/
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
		dwFileSize = file->dwFileSize;

		Log_WarnAssert(f != INVALID_HANDLE);
		
		file->pFileBytes = (BYTE*)malloc(dwFileSize);
		FS::Read(f, file->pFileBytes, file->dwFileSize);
		FS::CloseFile(f); // we don't actually need the file handle open any more

		pBits = new Bitstream();
		pBits->LoadStream(file->pFileBytes, file->dwFileSize);

		// Read the header
		ReadHeader(file, pBits);

		// Read each direction
		for(int i = 0; i < file->header.nNumberDirections; i++)
		{
			DCCDirection& dir = file->directions[i];
			size_t optionalSize = 0;

			pBits->SetCurrentPosition(file->header.dwDirectionOffset[i]);

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
				pBits->Align();
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
	static DCCCell g_decodeCells[6][6];
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

		animation->nDirectionW[direction] = nDirectionW;
		animation->nDirectionH[direction] = nDirectionH;

		// Global cell buffer
		int nFrames = animation->header.dwFramesPerDirection * animation->header.nNumberDirections;
		DCCCell** ppCellBuffer = new DCCCell*[dwNumCellsThisDir + nFrames];
		memset(ppCellBuffer, 0, sizeof(DCCCell*) * (dwNumCellsThisDir + nFrames));

		// Cell buffer for all frames
		DCCCell* pFrameCells[MAX_DCC_FRAMES];
		memset(pFrameCells, 0, sizeof(DCCCell*) * MAX_DCC_FRAMES);

		// Rewind all of the associated streams
		pDirection->RewindAllStreams();

		// First part: iterate through the frames and get the colors
		for(int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			DCCFrame* frame = &pDirection->frames[f];

			// Calculate the frame size, and number of cells this frame
			int nFrameW = frame->dwWidth;
			int nFrameH = frame->dwHeight;
			int nFrameX = frame->nXOffset - pDirection->nMinX;
			int nFrameY = frame->nYOffset - pDirection->nMinY;

			int nNumCellsW = DCC::GetCellCount(nFrameX, nFrameW);
			int nNumCellsH = DCC::GetCellCount(nFrameY, nFrameH);

			// Allocate cells
			DCCCell* pCell = pFrameCells[f] = new DCCCell[nNumCellsW * nNumCellsH];

			// Process cells left -> right / top -> bottom --SVR
			int nStartX = nFrameX >> 2;
			int nStartY = nFrameY >> 2;

			// Grab the four pixels (clrcode) color for each cell
			for(int y = 0; y < nNumCellsH; y++)
			{
				for(int x = 0; x < nNumCellsW; x++)
				{
					DCCCell* pCurCell = pCell++;
					DCCCell* pPrevCell = ppCellBuffer[(y * nNumCellsW) + x];
					DWORD dwClrMask = 0xF;

					*(DWORD*)(pCurCell->clrmap) = 0;

					// If we have a previous cell, read the contents of EqualCellBitstream and ColorMask
					if(pPrevCell)
					{
						if(pDirection->EqualCellStream != nullptr)
						{
							BYTE bit = 0;
							pDirection->EqualCellStream->ReadBits(bit, 1);
							if(bit)
							{	// Skip if we had a '1' from EqualCells (indicates that the cell is to be copied)
								continue;
							}
						}
						if(pDirection->PixelMaskStream != nullptr)
						{	// read the color mask
							pDirection->PixelMaskStream->ReadBits(&dwClrMask, 4);
						}
					}

					DWORD dwEncodingType = 0;
					DWORD dwClrCode = 0;
					DWORD dwUnencoded = 0;
					DWORD dwLastColor = 0;
					DWORD dwTemp = 0;

					// Mask off the appropriate colors
					if (dwClrMask != 0)
					{
						if (pDirection->EncodingTypeStream != nullptr)
						{
							pDirection->EncodingTypeStream->ReadBits(&dwEncodingType, 1);
						}

						for (int n = 0; n < 4; n++)
						{
							if (dwClrMask & (0x1 << n))
							{
								if (dwEncodingType != 0)
								{	// if encoding is 1, read it from the raw pixel stream
									pDirection->RawPixelStream->ReadBits(&dwClrCode, 8);
								}
								else
								{	// read difference from the pixel data and add it to the color
									do
									{
										pDirection->PixelCodeDisplacementStream->ReadBits(&dwUnencoded, 4);
										dwClrCode += dwUnencoded;
									} while (dwUnencoded == 15);
								}

								// Check to see if the same color was fetched.
								// If so, stop decoding (it's probably a transparent pixel)
								if (dwLastColor == dwClrCode)
								{
									break;
								}

								dwTemp <<= 8;
								dwTemp |= dwClrCode;
								dwLastColor = dwClrCode;
							}
						}
					}

					// Merge previous colors
					for (int n = 0; n < 4; n++)
					{
						if (dwClrMask & (0x1 << n))
						{
							pCurCell->clrmap[n] = (BYTE)(dwTemp & 0xFF);
							dwTemp >>= 8;
						}
						else
						{	// copy the previous color
							pCurCell->clrmap[n] = pPrevCell->clrmap[n];
						}
					}

					ppCellBuffer[(y * nNumCellsW) + x] = pCurCell;
				}
			}
		}

		// Second part: build a bitmap based on the cells data
		BYTE* bitmap = new BYTE[nDirectionW * nDirectionH];

		// ? clear all cells in the frame buffer list
		memset(ppCellBuffer, 0, sizeof(DCCCell*) * dwNumCellsThisDir);

		// Rewind the equal cell stream
		if(pDirection->EqualCellStream != nullptr)
		{
			pDirection->EqualCellStream->Rewind();
		}

		// Render each frame's cells
		for(int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			DCCFrame* frame = &pDirection->frames[f];

			// Clear out the bitmap
			memset(bitmap, 0, nDirectionW * nDirectionH);

			// Calculate the frame size and the number of cells in this frame
			int nFrameW = frame->dwWidth;
			int nFrameH = frame->dwHeight;
			int nFrameX = frame->nXOffset - pDirection->nMinX;
			int nFrameY = frame->nYOffset - pDirection->nMinY - nFrameH + 1;

			int nNumCellsW = DCC::GetCellCount(nFrameX, nFrameW);
			int nNumCellsH = DCC::GetCellCount(nFrameY, nFrameH);

			int nStartX = nFrameX >> 2;
			int nStartY = nFrameY >> 2;

			int nFirstColumnW = 4 - (nFrameX & 3);
			int nFirstRowH = 4 - (nFrameY & 3);

			int nCountI = nFirstRowH; // height counter
			int nCountJ;
			int nYPos = 0;
			int nXPos;

			DCCCell* pCells = pFrameCells[f];
			for(int y = 0; y < nNumCellsH; y++)
			{
				nXPos = 0;
				nCountJ = nFirstColumnW;

				if(y == nNumCellsH - 1)
				{	// If it's the last row, use the last height
					nCountI = nFrameH;
				}

				for(int x = 0; x < nNumCellsW; x++)
				{
					bool bTransparent = false;
					DCCCell* pCurCell = pCells++;
					DCCCell* pPrevCell = ppCellBuffer[(y * nDirCellW) + x];

					if(x == nNumCellsW - 1)
					{	// last cell = nFrameW
						nCountJ = nFrameW;
					}

					pCurCell->nH = nCountI;
					pCurCell->nW = nCountJ;
					pCurCell->nX = nFrameX + nXPos;
					pCurCell->nY = nFrameY + nYPos;

					// Check for equal cell
					if(pPrevCell && pDirection->EqualCellStream != nullptr)
					{
						DWORD dwEqualCell = 0;
						pDirection->EqualCellStream->ReadBits(&dwEqualCell, 1);

						if(dwEqualCell)
						{
							if(pPrevCell->nH == pCurCell->nH && pPrevCell->nW == pCurCell->nW)
							{	// same-sized cell = it is definitely the same one
								// check x/y - if they are not the same then we copy it
								if(pPrevCell->nX != pCurCell->nX || pPrevCell->nY != pCurCell->nY)
								{
									// source and destination rectangles
									int dY = pCurCell->nY;

									for(int i = pPrevCell->nY; i < pPrevCell->nY + pPrevCell->nH; i++)
									{
										int dX = pCurCell->nX;
										for(int j = pPrevCell->nX; j < pPrevCell->nX + pPrevCell->nW; j++)
										{
											bitmap[(dY * nDirectionW) + dX] = bitmap[(i * nDirectionW) + j];
											dX++;
										}
										dY++;
									}

								}
								ppCellBuffer[(y * nDirCellW) + x] = pCurCell;
								nXPos += nCountJ;
								nCountJ = 4;
								continue;
							}
							//else
							//{
							//	bTransparent = true;
							//}
						}
					}

					/*int n;
					for(n = 0; n < 2; n++)
					{
						// try to find a zero
						if(!pCurCell->clrmap[n])
						{
							break;
						}
					}*/

					// fill the cell
					//if(bTransparent || !n)
					if(pCurCell->clrmap[0] == pCurCell->clrmap[1])
					{	// if all of them are transparent, fill with zeroes
						for(int i = nFrameY + nYPos; i < nFrameY + nYPos + nCountI; i++)
						{
							for(int j = nFrameX + nXPos; j < nFrameX + nXPos + nCountJ; j++)
							{
								bitmap[(i * nDirectionW) + j] = 0;
							}
						}
					}
					else
					{
						// Write the color pixels one by one
						int n = pCurCell->clrmap[1] == pCurCell->clrmap[2] ? 1 : 2;
						for(int i = 0; i < nCountI; i++)
						{
							for(int j = 0; j < nCountJ; j++)
							{
								DWORD dwPixelData = 0;
								DWORD dwPixelPos = ((nFrameY + nYPos + i) * (frame->dwWidth)) + (nFrameX + nXPos + j);
								int nPalettePos;

								pDirection->PixelCodeDisplacementStream->ReadBits(&dwPixelData, n);
								nPalettePos = pCurCell->clrmap[dwPixelData];
								bitmap[dwPixelPos] = pDirection->nPixelValues[nPalettePos];
							}
						}
					}

					ppCellBuffer[(y * nDirCellW) + x] = pCurCell;
					nXPos += nCountJ;
					nCountJ = 4;
				}

				nYPos += nCountI;
				nCountI = 4;
			}

			// Handle the individual frame
			if(callback)
			{
				callback(bitmap, f, nFrameX, nFrameY, frame->dwWidth, frame->dwHeight);
			}
		}

		// Delete all of the data that we don't need any more.
		// FIXME: should we really be doing this..? probably more efficient to use a statically-allocated buffer for decoding
		for(int f = 0; f < animation->header.dwFramesPerDirection; f++)
		{
			delete pFrameCells[f];
		}

		delete[] bitmap;
		delete[] ppCellBuffer;
	}

	/**
	 *	Gets the size of a particular direction.
	 *	@author	eezstreet
	 */
	void GetDirectionSize(DCCFile* animation, uint32_t direction, uint32_t* frameSizeW, uint32_t* frameSizeH)
	{
		uint32_t sumW = 0;
		uint32_t sumH = 0;

		Log_ErrorAssertVoidReturn(animation);
		Log_ErrorAssertVoidReturn(direction >= 0 && direction < animation->header.nNumberDirections);
		
		for (int i = 0; i < animation->header.dwFramesPerDirection; i++)
		{
			const DCCFrame& frame = animation->directions[direction].frames[i];
			
			if (frame.dwHeight > sumH)
			{
				sumH = frame.dwHeight;
			}
			sumW += frame.dwWidth;
		}

		if (frameSizeW)
		{
			*frameSizeW = sumW;
		}

		if (frameSizeH)
		{
			*frameSizeH = sumH;
		}
	}
}
#endif