#include "Diablo2.hpp"
#include <memory>

#define DC6_HEADER_VERSION	6

/*
 *	Decode a DC6 frame in place
 *	@author	Necrolis/eezstreet
 *	TODO: optimize this a lot
 */
static void DC6_DecodeFrame(BYTE* pPixels, BYTE* pOutPixels, DC6Frame* pFrame)
{
	DWORD x = 0, y;

	if (pFrame->dwFlip > 0)
	{
		y = 0;
	}
	else
	{
		y = pFrame->dwHeight;
	}

	for (size_t i = 0; i < pFrame->dwLength; i++)
	{
		BYTE pixel = pPixels[i];
		if (pixel == 0x80)
		{	// pixel row termination
			x = 0;
			if (pFrame->dwFlip > 0)
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
			// No DC6 pixel can physically *be* 0x80 since it's the mapping for pixel row termination.
			// Ergo, we will use that as the color key for transparency
			memset(&pOutPixels[(y * pFrame->dwWidth) + x++], 0x80, pixel & 0x7F);
		}
		else
		{
			while (pixel--)
			{
				pOutPixels[(y * pFrame->dwWidth) + x++] = pPixels[++i];
			}
		}
	}
}

/*
 *	Loads a DC6 from an MPQ
 *	@author	eezstreet
 */
static BYTE gpDecodeBuffer[2048 * 2048];

void DC6_LoadImage(char* szPath, DC6Image* pImage)
{
	pImage->f = FSMPQ_FindFile(szPath, nullptr, (D2MPQArchive**)&pImage->mpq);
	if (pImage->f == (fs_handle)-1)
	{
		return;
	}

	// Now comes the fun part: reading and decoding the actual thing
	size_t dwFileSize = MPQ_FileSize((D2MPQArchive*)pImage->mpq, pImage->f);

	BYTE* pByteReadHead = gpDecodeBuffer;

	MPQ_ReadFile((D2MPQArchive*)pImage->mpq, pImage->f, gpDecodeBuffer, dwFileSize);

	memcpy(&pImage->header, pByteReadHead, sizeof(pImage->header));
	pByteReadHead += sizeof(pImage->header);

	// Validate the header
	if (pImage->header.dwVersion != DC6_HEADER_VERSION)
	{	// bad dc6 - die?
		return;
	}

	// Table of pointers
	DWORD* pFramePointers = (DWORD*)pByteReadHead;
	pByteReadHead += sizeof(DWORD) * pImage->header.dwDirections * pImage->header.dwFrames;

	pImage->pFrames = (DC6Frame*)malloc(sizeof(DC6Frame) * pImage->header.dwDirections * pImage->header.dwFrames);
	if (pImage->pFrames == nullptr)
	{	// couldn't allocate space, die?
		return;
	}

	// Copy frame headers. Compute the total number of pixels that we need to allocate in the process.
	DWORD dwTotalPixels = 0;
	int i, j;
	DWORD dwFramePos;

	for (i = 0; i < pImage->header.dwDirections; i++)
	{
		for (j = 0; j < pImage->header.dwFrames; j++)
		{
			dwFramePos = (i * pImage->header.dwFrames) + j;
			memcpy(&pImage->pFrames[dwFramePos], gpDecodeBuffer + pFramePointers[dwFramePos], sizeof(DC6Frame));
			pImage->pFrames[dwFramePos].dwNextBlock = dwTotalPixels * sizeof(BYTE);
			dwTotalPixels += pImage->pFrames[dwFramePos].dwWidth * pImage->pFrames[dwFramePos].dwHeight;
		}
	}

	pImage->pPixels = (BYTE*)malloc(dwTotalPixels);
	memset(pImage->pPixels, 0, dwTotalPixels);

	// Decode all of the blocks
	for (i = 0; i < pImage->header.dwDirections; i++)
	{
		for (j = 0; j < pImage->header.dwFrames; j++)
		{
			dwFramePos = (i * pImage->header.dwFrames) + j;
			DC6Frame* pFrame = &pImage->pFrames[dwFramePos];
			pByteReadHead = gpDecodeBuffer + pFramePointers[dwFramePos] + sizeof(DC6Frame);
			DC6_DecodeFrame(pByteReadHead, pImage->pPixels + pFrame->dwNextBlock, pFrame);
		}
	}
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
		*pNumPixels = pFrame->dwWidth * pFrame->dwHeight;
	}

	return pImage->pPixels + pFrame->dwNextBlock;
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
		*dwWidth = pFrame->dwWidth;
	}

	if (dwHeight != nullptr)
	{
		*dwHeight = pFrame->dwHeight;
	}

	if (dwOffsetX != nullptr)
	{
		*dwOffsetX = pFrame->dwOffsetX;
	}

	if (dwOffsetY != nullptr)
	{
		*dwOffsetY = pFrame->dwOffsetY;
	}
}