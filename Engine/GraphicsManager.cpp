#include "GraphicsManager.hpp"
#include "DC6.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"
#include "Palette.hpp"

GraphicsManager* graphicsManager;

/**
 *	DCCGraphicsHandle is the IGraphicsHandle implementation for DCC files.
 */

DCCGraphicsHandle::DCCGraphicsHandle(const char* fileName)
{
	FS::Open(fileName, &fileHandle, FS_READ);
}

size_t DCCGraphicsHandle::GetTotalSizeInBytes(int32_t frame)
{
	return 0;
}

size_t DCCGraphicsHandle::GetNumberOfFrames()
{
	return 0;
}

void DCCGraphicsHandle::GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height)
{

}

void DCCGraphicsHandle::GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

}

void DCCGraphicsHandle::IterateFrames(AtlassingCallback callback, int32_t start, int32_t end)
{

}


/**
 *	DC6GraphicsHandle is the IGraphicsHandle implementation for DC6 files.
 */

DC6GraphicsHandle::DC6GraphicsHandle(const char* fileName)
{
	D2Lib::strncpyz(filePath, fileName, sizeof(filePath));
}

DC6GraphicsHandle::~DC6GraphicsHandle()
{
	DC6::UnloadImage(&image);
}

size_t DC6GraphicsHandle::GetTotalSizeInBytes(int32_t frame)
{
	if (!bLoaded)
	{
		DC6::LoadImage(filePath, &image);
		bLoaded = true;
	}

	if (frame < 0 || frame > image.header.dwFrames)
	{
		return image.dwTotalWidth * image.dwTotalHeight;
	}
	return image.pFrames[frame].fh.dwWidth * image.pFrames[frame].fh.dwHeight;
}

size_t DC6GraphicsHandle::GetNumberOfFrames()
{
	if (!bLoaded)
	{
		DC6::LoadImage(filePath, &image);
		bLoaded = true;
	}

	return image.header.dwFrames;
}

void DC6GraphicsHandle::GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height)
{
	if (!bLoaded)
	{
		DC6::LoadImage(filePath, &image);
		bLoaded = true;
	}

	if (frame < 0 || frame > image.header.dwFrames)
	{
		return;
	}

	if (width)
	{
		*width = image.pFrames[frame].fh.dwWidth;
	}

	if (height)
	{
		*height = image.pFrames[frame].fh.dwHeight;
	}

	if (pixels)
	{
		*pixels = DC6::GetPixelsAtFrame(&image, 0, frame, nullptr);
	}
}

void DC6GraphicsHandle::GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{
	if (!bLoaded)
	{
		DC6::LoadImage(filePath, &image);
		bLoaded = true;
	}

	if (end < 0)
	{
		end = image.header.dwFrames - 1;
	}

	DWORD dwTotalWidth, dwTotalHeight, dwFrameWidth, dwFrameHeight;
	DC6::StitchStats(&image, start, end, &dwFrameWidth, &dwFrameHeight, &dwTotalWidth, &dwTotalHeight);

	if (width)
	{
		*width = dwTotalWidth;
	}

	if (height)
	{
		*height = dwTotalHeight;
	}
}

void DC6GraphicsHandle::IterateFrames(AtlassingCallback callback, int32_t start, int32_t end)
{
	if (!bLoaded)
	{
		DC6::LoadImage(filePath, &image);
		bLoaded = true;
	}

	if (end < 0)
	{
		end = image.header.dwFrames - 1;
	}

	DWORD dwTotalWidth, dwTotalHeight, dwFrameWidth, dwFrameHeight;
	DC6::StitchStats(&image, start, end, &dwFrameWidth, &dwFrameHeight, &dwTotalWidth, &dwTotalHeight);

	for (int i = 0; i <= end - start; i++)
	{
		DC6Frame* pFrame = &image.pFrames[start + i];

		int dwBlitToX = (i % dwFrameWidth) * 256;
		int dwBlitToY = (int)floor(i / (float)dwFrameWidth) * 255;

		if (callback)
		{
			callback(DC6::GetPixelsAtFrame(&image, 0, start + i, nullptr), start + i,
				dwBlitToX, dwBlitToY, pFrame->fh.dwWidth, pFrame->fh.dwHeight);
		}
	}
}


/**
 *	DT1GraphicsHandle is the IGraphicsHandle implementation for DT1 files.
 */

DT1GraphicsHandle::DT1GraphicsHandle(const char* fileName)
{
	FS::Open(fileName, &fileHandle, FS_READ);
}

size_t DT1GraphicsHandle::GetTotalSizeInBytes(int32_t frame)
{
	return 0;
}

size_t DT1GraphicsHandle::GetNumberOfFrames()
{
	return 0;
}

void DT1GraphicsHandle::GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height)
{

}

void DT1GraphicsHandle::GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

}

void DT1GraphicsHandle::IterateFrames(AtlassingCallback callback, int32_t start, int32_t end)
{

}


/**
 *	PL2GraphicsHandle is the IGraphicsHandle implementation for PL2 files.
 */

PL2GraphicsHandle::PL2GraphicsHandle(const char* fileName)
{
	size_t size = FS::Open(fileName, &fileHandle, FS_READ);
	Log_WarnAssert(size == sizeof(PL2File));
	FS::Read(fileHandle, &file, sizeof(PL2File));
	FS::CloseFile(fileHandle);
}

size_t PL2GraphicsHandle::GetTotalSizeInBytes(int32_t frame)
{
	return sizeof(PL2File);
}

size_t PL2GraphicsHandle::GetNumberOfFrames()
{
	return PL2_NumEntries;
}

void PL2GraphicsHandle::GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height)
{
	if (height)
	{
		*height = 1;
	}

	if (frame == PL2_RGBAPalette)
	{
		if (pixels)
		{
			*pixels = file.pRGBAPalette;
		}
		if (width)
		{
			*width = sizeof(file.pRGBAPalette);
		}
	}
	else if (frame >= PL2_Shadows && frame < PL2_Light)
	{
		if (pixels)
		{
			*pixels = file.pShadows[frame - PL2_Shadows];
		}
		if (width)
		{
			*width = sizeof(file.pShadows[0]);
		}
	}
	else if (frame >= PL2_Light && frame < PL2_Gamma)
	{
		if (pixels)
		{
			*pixels = file.pLight[frame - PL2_Light];
		}
		if (width)
		{
			*width = sizeof(file.pLight[0]);
		}
	}
	else if (frame == PL2_Gamma)
	{
		if (pixels)
		{
			*pixels = file.pGamma;
		}
		if (width)
		{
			*width = sizeof(file.pGamma);
		}
	}
	else if (frame >= PL2_Trans25 && frame < PL2_Trans50)
	{
		if (pixels)
		{
			*pixels = file.pTrans25[frame - PL2_Trans25];
		}
		if (width)
		{
			*width = sizeof(file.pTrans25[0]);
		}
	}
	else if (frame >= PL2_Trans50 && frame < PL2_Trans75)
	{
		if (pixels)
		{
			*pixels = file.pTrans50[frame - PL2_Trans50];
		}
		if (width)
		{
			*width = sizeof(file.pTrans50[0]);
		}
	}
	else if (frame >= PL2_Trans75 && frame < PL2_Screen)
	{
		if (pixels)
		{
			*pixels = file.pTrans75[frame - PL2_Trans75];
		}
		if (width)
		{
			*width = sizeof(file.pTrans75[0]);
		}
	}
	else if (frame >= PL2_Screen && frame < PL2_Luminance)
	{
		if (pixels)
		{
			*pixels = file.pScreen[frame - PL2_Screen];
		}
		if (width)
		{
			*width = sizeof(file.pScreen[0]);
		}
	}
	else if (frame >= PL2_Luminance && frame < PL2_States)
	{
		if (pixels)
		{
			*pixels = file.pLuminance[frame - PL2_Luminance];
		}
		if (width)
		{
			*width = sizeof(file.pLuminance[0]);
		}
	}
	else if (frame >= PL2_States && frame < PL2_DarkBlend)
	{
		if (pixels)
		{
			*pixels = file.pStates[frame - PL2_States];
		}
		if (width)
		{
			*width = sizeof(file.pStates[0]);
		}
	}
	else if (frame >= PL2_DarkBlend && frame < PL2_DarkenPalette)
	{
		if (pixels)
		{
			*pixels = file.pDarkBlend[frame - PL2_DarkBlend];
		}
		if (width)
		{
			*width = sizeof(file.pDarkBlend[0]);
		}
	}
	else if (frame == PL2_DarkenPalette)
	{
		if (pixels)
		{
			*pixels = file.pDarkenPalette;
		}
		if (width)
		{
			*width = sizeof(file.pDarkenPalette);
		}
	}
	else if (frame >= PL2_StandardColors && frame < PL2_StandardShifts)
	{
		if (pixels)
		{
			*pixels = file.pStandardColors[frame - PL2_StandardColors];
		}
		if (width)
		{
			*width = sizeof(file.pStandardColors[0]);
		}
	}
	else if (frame >= PL2_StandardShifts && frame < PL2_NumEntries)
	{
		if (pixels)
		{
			*pixels = file.pStandardShifts[frame - PL2_StandardShifts];
		}
		if (width)
		{
			*width = sizeof(file.pStandardShifts[0]);
		}
	}
}

void PL2GraphicsHandle::GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{
	// should not be used?
}

void PL2GraphicsHandle::IterateFrames(AtlassingCallback callback, int32_t start, int32_t end)
{

}

GraphicsManager::GraphicsManager()
{

}

GraphicsManager::~GraphicsManager()
{

}

IGraphicsHandle* GraphicsManager::LoadGraphic(const char* graphicsFile, GraphicsUsagePolicy policy)
{
	// Check the extension on the file
	char* ext = D2Lib::fnext(graphicsFile);
	handle theHandle;
	bool bFull;

	if (!D2Lib::stricmp(ext, ".dc6"))
	{
		// DC6 loading
		switch (policy)
		{
			case UsagePolicy_SingleUse:
				return new DC6GraphicsHandle(graphicsFile);

			case UsagePolicy_Permanent:
				if (!DC6Graphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					DC6Graphics.Insert(theHandle, graphicsFile, new DC6GraphicsHandle(graphicsFile));
				}
				return DC6Graphics[theHandle];

			case UsagePolicy_Temporary:
				// Not yet implemented
				break;
		}
		
	}
	else if (!D2Lib::stricmp(ext, ".pl2"))
	{
		// PL2 loading
		switch (policy)
		{
			case UsagePolicy_SingleUse:
				return new PL2GraphicsHandle(graphicsFile);

			case UsagePolicy_Permanent:
				if (!PL2Graphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					PL2Graphics.Insert(theHandle, graphicsFile, new PL2GraphicsHandle(graphicsFile));
				}
				return PL2Graphics[theHandle];

			case UsagePolicy_Temporary:
				// Not yet implemented
				break;
		}
		
	}
	else if (!D2Lib::stricmp(ext, ".dcc"))
	{
		// DCC loading
		switch (policy)
		{
			case UsagePolicy_SingleUse:
				return new DCCGraphicsHandle(graphicsFile);

			case UsagePolicy_Permanent:
				if (!DCCGraphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					DCCGraphics.Insert(theHandle, graphicsFile, new DCCGraphicsHandle(graphicsFile));
				}
				return DCCGraphics[theHandle];

			case UsagePolicy_Temporary:
				// Not yet implemented
				break;
		}
	}
	else if (!D2Lib::stricmp(ext, ".dt1"))
	{
		// DT1 loading
		switch (policy)
		{
			case UsagePolicy_SingleUse:
				return new DT1GraphicsHandle(graphicsFile);

			case UsagePolicy_Permanent:
				if (!DT1Graphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					DT1Graphics.Insert(theHandle, graphicsFile, new DT1GraphicsHandle(graphicsFile));
				}
				return DT1Graphics[theHandle];

			case UsagePolicy_Temporary:
				// Not yet implemented
				break;
		}
		
	}
	else
	{
		Log_ErrorAssertReturn(graphicsFile, nullptr);
	}
	return nullptr;
}

void GraphicsManager::UnloadGraphic(IGraphicsHandle* graphic)
{
	delete graphic;
}