#include "Diablo2.hpp"
#include <memory>

#define DC6_HEADER_VERSION	6
#define DECODE_BUFFER_SIZE	2048 * 2048

static BYTE gpDecodeBuffer[DECODE_BUFFER_SIZE];
static BYTE gpReadBuffer[DECODE_BUFFER_SIZE];

/*
 *	Decode a DC6 frame in place
 *	@author	Necrolis/eezstreet
 *	TODO: optimize this a lot
 */
static void DC6_DecodeFrame(BYTE* pPixels, BYTE* pOutPixels, DC6Frame* pFrame)
{
	DWORD x = 0, y;

	if (pFrame->fh.dwFlip > 0)
	{
		y = 0;
	}
	else
	{
		y = pFrame->fh.dwHeight;
	}

	for (size_t i = 0; i < pFrame->fh.dwLength; i++)
	{
		BYTE pixel = pPixels[i];
		if (pixel == 0x80)
		{	// pixel row termination
			x = 0;
			if (pFrame->fh.dwFlip > 0)
			{
				y++;
			}
			else
			{
				y--;
			}
		}
		else if (pixel & 0x80)
		{	// Write PIXEL & 0x7F transparent pixels
			x += pixel & 0x7F;
		}
		else
		{
			while (pixel--)
			{
				pOutPixels[(y * pFrame->fh.dwWidth) + x++] = pPixels[++i];
			}
		}
	}
}

/*
 *	Loads a DC6 from an MPQ
 *	@author	eezstreet
 */
void DC6_LoadImage(char* szPath, DC6Image* pImage)
{
	int i, j;
	size_t dwFileSize;
	BYTE* pByteReadHead = gpReadBuffer;
	DWORD* pFramePointers;
	DWORD dwFramePos, dwNumFrames;
	DWORD dwOffset = 0, dwTotalPixels = 0, dwWidth = 0, dwHeight = 0, dwDirectionWidth = 0, dwDirectionHeight = 0;

	memset(pImage, 0, sizeof(DC6Image));
	memset(gpDecodeBuffer, 0, sizeof(BYTE) * DECODE_BUFFER_SIZE);

	pImage->f = FSMPQ_FindFile(szPath, nullptr, (D2MPQArchive**)&pImage->mpq);
	Log_WarnAssert(pImage->f != (fs_handle)-1);

	// Now comes the fun part: reading and decoding the actual thing
	dwFileSize = MPQ_FileSize((D2MPQArchive*)pImage->mpq, pImage->f);

	Log_WarnAssert(MPQ_FileSize((D2MPQArchive*)pImage->mpq, pImage->f) < DECODE_BUFFER_SIZE);
	MPQ_ReadFile((D2MPQArchive*)pImage->mpq, pImage->f, gpReadBuffer, dwFileSize);

	memcpy(&pImage->header, pByteReadHead, sizeof(pImage->header));
	pByteReadHead += sizeof(pImage->header);

	// Validate the header
	Log_WarnAssert(pImage->header.dwVersion == DC6_HEADER_VERSION);

	// Table of pointers
	pFramePointers = (DWORD*)pByteReadHead;
	dwNumFrames = pImage->header.dwDirections * pImage->header.dwFrames;
	pByteReadHead += sizeof(DWORD) * dwNumFrames;

	pImage->pFrames = (DC6Frame*)malloc(sizeof(DC6Frame) * dwNumFrames);
	Log_ErrorAssert(pImage->pFrames != nullptr);

	// Read each frame's header, and then decode the blocks
	for (i = 0; i < pImage->header.dwDirections; i++)
	{
		for (j = 0; j < pImage->header.dwFrames; j++)
		{
			DC6Frame* pFrame;

			dwFramePos = (i * pImage->header.dwFrames) + j;
			pFrame = &pImage->pFrames[dwFramePos];
			memcpy(&pFrame->fh, gpReadBuffer + pFramePointers[dwFramePos], sizeof(DC6Frame::DC6FrameHeader));

			pFrame->fh.dwNextBlock = dwOffset;

			if (j > 0)
			{
				pFrame->dwDeltaY = pImage->pFrames[j - 1].fh.dwHeight - pFrame->fh.dwHeight;
			}
			else
			{
				pFrame->dwDeltaY = 0;
			}

			pByteReadHead = gpReadBuffer + pFramePointers[dwFramePos] + sizeof(DC6Frame::DC6FrameHeader);
			DC6_DecodeFrame(pByteReadHead, gpDecodeBuffer + dwOffset, pFrame);

			dwTotalPixels += pFrame->fh.dwWidth * pFrame->fh.dwHeight;
			dwOffset += pFrame->fh.dwWidth * pFrame->fh.dwHeight;

			dwDirectionWidth += pFrame->fh.dwWidth;
			if (pFrame->fh.dwHeight > dwDirectionHeight)
			{
				dwDirectionHeight = pFrame->fh.dwHeight;
			}
		}

		if (dwDirectionWidth > dwWidth)
		{
			dwWidth = dwDirectionWidth;
		}

		dwHeight += dwDirectionHeight;

		pImage->dwDirectionHeights[i] = dwDirectionHeight;

		dwDirectionWidth = 0;
		dwDirectionHeight = 0;
	}

	pImage->dwTotalWidth = dwWidth;
	pImage->dwTotalHeight = dwHeight;

	// Allocate the pixels that we need
	pImage->pPixels = (BYTE*)malloc(dwTotalPixels);
	Log_ErrorAssert(pImage->pPixels != nullptr);

	memcpy(pImage->pPixels, gpDecodeBuffer, dwTotalPixels);
}

/*
 *	Frees a DC6 resource
 *	@author	eezstreet
 */
void DC6_UnloadImage(DC6Image* pImage)
{
	free(pImage->pPixels);
	free(pImage->pFrames);
}

/*
 *	Get pointer to pixels at specific frame, and how many pixels there are
 */
BYTE* DC6_GetPixelsAtFrame(DC6Image* pImage, int nDirection, int nFrame, size_t* pNumPixels)
{
	if (pImage == nullptr)
	{
		return nullptr;
	}

	if (nDirection >= pImage->header.dwDirections)
	{
		return nullptr;
	}

	if (nFrame >= pImage->header.dwFrames)
	{
		return nullptr;
	}

	DC6Frame* pFrame = &pImage->pFrames[(nDirection * pImage->header.dwFrames) + nFrame];

	if (pNumPixels != nullptr)
	{
		*pNumPixels = pFrame->fh.dwWidth * pFrame->fh.dwHeight;
	}

	return (pImage->pPixels + pFrame->fh.dwNextBlock);
}

/*
 *	Retrieve some data about a DC6 frame
 */
void DC6_PollFrame(DC6Image* pImage, DWORD nDirection, DWORD nFrame, 
	DWORD* dwWidth, DWORD* dwHeight, DWORD* dwOffsetX, DWORD* dwOffsetY)
{
	if (pImage == nullptr)
	{
		return;
	}

	if (nDirection >= pImage->header.dwDirections)
	{
		return;
	}

	if (nFrame >= pImage->header.dwFrames)
	{
		return;
	}

	DC6Frame* pFrame = &pImage->pFrames[(nDirection * pImage->header.dwFrames) + nFrame];
	if (dwWidth != nullptr)
	{
		*dwWidth = pFrame->fh.dwWidth;
	}

	if (dwHeight != nullptr)
	{
		*dwHeight = pFrame->fh.dwHeight;
	}

	if (dwOffsetX != nullptr)
	{
		*dwOffsetX = pFrame->fh.dwOffsetX;
	}

	if (dwOffsetY != nullptr)
	{
		*dwOffsetY = pFrame->fh.dwOffsetY;
	}
}

/*
 *	Retrieve row width and height (for stitched DC6)
 */

void DC6_StitchStats(DC6Image* pImage, 
	DWORD dwStart, DWORD dwEnd, DWORD* pWidth, DWORD* pHeight, DWORD* pTotalWidth, DWORD* pTotalHeight)
{
	DC6Frame* pFrame;

	Log_WarnAssert(dwStart >= 0 && dwStart <= dwEnd && dwStart < pImage->header.dwFrames);
	Log_WarnAssert(dwEnd >= 0 && dwEnd < pImage->header.dwFrames);
	Log_WarnAssert(pWidth && pHeight);
	Log_WarnAssert(pTotalWidth && pTotalHeight);

	*pWidth = 0;
	*pHeight = 0;

	for (int i = dwStart; i <= dwEnd; i++)
	{
		pFrame = &pImage->pFrames[i];

		(*pWidth)++;
		(*pTotalWidth) += pFrame->fh.dwWidth;

		if (pFrame->fh.dwWidth != MAX_DC6_CELL_SIZE)
		{
			break;
		}
	}

	for (int i = dwStart; i < dwEnd; i += (*pWidth))
	{
		pFrame = &pImage->pFrames[i];

		(*pHeight)++;
		(*pTotalHeight) += pFrame->fh.dwHeight;

		if (pFrame->fh.dwHeight != MAX_DC6_CELL_SIZE)
		{
			break;
		}
	}
}