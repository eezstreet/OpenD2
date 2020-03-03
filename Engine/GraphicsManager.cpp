#include "GraphicsManager.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"
#include "Palette.hpp"

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

void DCCGraphicsHandle::GetEntireGraphicsData(void** pixels, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

}


/**
 *	DC6GraphicsHandle is the IGraphicsHandle implementation for DC6 files.
 */

DC6GraphicsHandle::DC6GraphicsHandle(const char* fileName)
{
	FS::Open(fileName, &fileHandle, FS_READ);
}

size_t DC6GraphicsHandle::GetTotalSizeInBytes(int32_t frame)
{
	return 0;
}

size_t DC6GraphicsHandle::GetNumberOfFrames()
{
	return 0;
}

void DC6GraphicsHandle::GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height)
{

}

void DC6GraphicsHandle::GetEntireGraphicsData(void** pixels, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

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

void DT1GraphicsHandle::GetEntireGraphicsData(void** pixels, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
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
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pRGBAPalette, sizeof(file.pRGBAPalette));
		}
		if (width)
		{
			*width = sizeof(file.pRGBAPalette);
		}
	}
	else if (frame >= PL2_Shadows && frame < PL2_Light)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pShadows[frame - PL2_Shadows], sizeof(file.pShadows[0]));
		}
		if (width)
		{
			*width = sizeof(file.pShadows[0]);
		}
	}
	else if (frame >= PL2_Light && frame < PL2_Gamma)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pLight[frame - PL2_Light], sizeof(file.pLight[0]));
		}
		if (width)
		{
			*width = sizeof(file.pLight[0]);
		}
	}
	else if (frame == PL2_Gamma)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pGamma, sizeof(file.pGamma));
		}
		if (width)
		{
			*width = sizeof(file.pGamma);
		}
	}
	else if (frame >= PL2_Trans25 && frame < PL2_Trans50)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pTrans25[frame - PL2_Trans25], sizeof(file.pTrans25[0]));
		}
		if (width)
		{
			*width = sizeof(file.pTrans25[0]);
		}
	}
	else if (frame >= PL2_Trans50 && frame < PL2_Trans75)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pTrans50[frame - PL2_Trans50], sizeof(file.pTrans50[0]));
		}
		if (width)
		{
			*width = sizeof(file.pTrans50[0]);
		}
	}
	else if (frame >= PL2_Trans75 && frame < PL2_Screen)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pTrans75[frame - PL2_Trans75], sizeof(file.pTrans75[0]));
		}
		if (width)
		{
			*width = sizeof(file.pTrans75[0]);
		}
	}
	else if (frame >= PL2_Screen && frame < PL2_Luminance)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pScreen[frame - PL2_Screen], sizeof(file.pScreen[0]));
		}
		if (width)
		{
			*width = sizeof(file.pScreen[0]);
		}
	}
	else if (frame >= PL2_Luminance && frame < PL2_States)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pLuminance[frame - PL2_Luminance], sizeof(file.pLuminance[0]));
		}
		if (width)
		{
			*width = sizeof(file.pLuminance[0]);
		}
	}
	else if (frame >= PL2_States && frame < PL2_DarkBlend)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pStates[frame - PL2_States], sizeof(file.pStates[0]));
		}
		if (width)
		{
			*width = sizeof(file.pStates[0]);
		}
	}
	else if (frame >= PL2_DarkBlend && frame < PL2_DarkenPalette)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pDarkBlend[frame - PL2_DarkBlend], sizeof(file.pDarkBlend[0]));
		}
		if (width)
		{
			*width = sizeof(file.pDarkBlend[0]);
		}
	}
	else if (frame == PL2_DarkenPalette)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pDarkenPalette, sizeof(file.pDarkenPalette));
		}
		if (width)
		{
			*width = sizeof(file.pDarkenPalette);
		}
	}
	else if (frame >= PL2_StandardColors && frame < PL2_StandardShifts)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pStandardColors[frame - PL2_StandardColors], sizeof(file.pStandardColors[0]));
		}
		if (width)
		{
			*width = sizeof(file.pStandardColors[0]);
		}
	}
	else if (frame >= PL2_StandardShifts && frame < PL2_NumEntries)
	{
		if (pixels && *pixels)
		{
			memcpy(*pixels, file.pStandardShifts[frame - PL2_StandardShifts], sizeof(file.pStandardShifts[0]));
		}
		if (width)
		{
			*width = sizeof(file.pStandardShifts[0]);
		}
	}
}

void PL2GraphicsHandle::GetEntireGraphicsData(void** pixels, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{
}

GraphicsManager::GraphicsManager()
{

}

GraphicsManager::~GraphicsManager()
{

}

IGraphicsHandle* GraphicsManager::LoadGraphic(const char* graphicsFile)
{
	// Check the extension on the file
	char* ext = D2Lib::fnext(graphicsFile);
	handle theHandle;
	bool bFull;

	if (!D2Lib::stricmp(ext, ".dc6"))
	{
		// DC6 loading
		if (!DC6Graphics.Contains(graphicsFile, &theHandle, &bFull))
		{
			if (bFull)
			{
				return nullptr; // it's full
			}

			DC6Graphics.Insert(theHandle, graphicsFile, DC6GraphicsHandle(graphicsFile));
		}
		return DC6Graphics.GetPointerTo(theHandle);
	}
	else if (!D2Lib::stricmp(ext, ".pl2"))
	{
		// PL2 loading
		if (!PL2Graphics.Contains(graphicsFile, &theHandle, &bFull))
		{
			if (bFull)
			{
				return nullptr; // it's full
			}

			PL2Graphics.Insert(theHandle, graphicsFile, PL2GraphicsHandle(graphicsFile));
		}
		return PL2Graphics.GetPointerTo(theHandle);
	}
	else if (!D2Lib::stricmp(ext, ".dcc"))
	{
		// DCC loading
		if (!DCCGraphics.Contains(graphicsFile, &theHandle, &bFull))
		{
			if (bFull)
			{
				return nullptr; // it's full
			}

			DCCGraphics.Insert(theHandle, graphicsFile, DCCGraphicsHandle(graphicsFile));
		}
		return DCCGraphics.GetPointerTo(theHandle);
	}
	else if (!D2Lib::stricmp(ext, ".dt1"))
	{
		// DT1 loading
		if (!DT1Graphics.Contains(graphicsFile, &theHandle, &bFull))
		{
			if (bFull)
			{
				return nullptr; // it's full
			}

			DT1Graphics.Insert(theHandle, graphicsFile, DT1GraphicsHandle(graphicsFile));
		}
		return DT1Graphics.GetPointerTo(theHandle);
	}
	else
	{
		Log_ErrorAssertReturn(graphicsFile, nullptr);
	}
	return nullptr;
}