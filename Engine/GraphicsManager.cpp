#include "GraphicsManager.hpp"
#include "DC6.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"
#include "Palette.hpp"
#include "TBL_Font.hpp"

GraphicsManager* graphicsManager;

/**
 *	DCCGraphicsHandle is the IGraphicsHandle implementation for DCC files.
 */

DCCReference::DCCReference(const char* fileName, GraphicsUsagePolicy usagePolicy)
	: IGraphicsReference(usagePolicy)
{
	D2Lib::strncpyz(dccHandleName, fileName, sizeof(dccHandleName));
}

size_t DCCReference::GetTotalSizeInBytes(int32_t frame)
{
	return 0;
}

size_t DCCReference::GetNumberOfFrames()
{
	return 0;
}

void DCCReference::GetGraphicsData(void** pixels, int32_t frame,
	uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY)
{

}

void DCCReference::GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

}

void DCCReference::IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback)
{

}

void DCCReference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight)
{

}

void DCCReference::Deallocate()
{
	switch (usagePolicy)
	{
		case UsagePolicy_Permanent:
			graphicsManager->RemovePermanentDCCGraphic(dccHandleName);
			break;
		case UsagePolicy_Temporary:
			// not yet implemented
			break;
	}
}

/**
 *	DC6GraphicsHandle is the IGraphicsHandle implementation for DC6 files.
 */

DC6Reference::DC6Reference(const char* fileName, GraphicsUsagePolicy usagePolicy)
	: IGraphicsReference(usagePolicy)
{
	D2Lib::strncpyz(filePath, fileName, sizeof(filePath));
	bAreGraphicsLoaded = false;
	loadedGraphicsData = nullptr;
}

DC6Reference::~DC6Reference()
{
	DC6::UnloadImage(&image);
}

size_t DC6Reference::GetTotalSizeInBytes(int32_t frame)
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

size_t DC6Reference::GetNumberOfFrames()
{
	if (!bLoaded)
	{
		DC6::LoadImage(filePath, &image);
		bLoaded = true;
	}

	return image.header.dwFrames;
}

void DC6Reference::GetGraphicsData(void** pixels, int32_t frame,
	uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY)
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

	if (offsetX)
	{
		*offsetX = image.pFrames[frame].fh.dwOffsetX;
	}

	if (offsetY)
	{
		*offsetY = image.pFrames[frame].fh.dwOffsetY;
	}

	if (pixels)
	{
		*pixels = DC6::GetPixelsAtFrame(&image, 0, frame, nullptr);
	}
}

void DC6Reference::GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
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


	DWORD dwTotalWidth, dwTotalHeight;
	
	if (bAtlassing)
	{
		DC6::AtlasStats(&image, start, end, &dwTotalWidth, &dwTotalHeight);
	}
	else
	{
		DWORD dwFrameWidth, dwFrameHeight;
		DC6::StitchStats(&image, start, end, &dwFrameWidth, &dwFrameHeight, &dwTotalWidth, &dwTotalHeight);
	}
	

	if (width)
	{
		*width = dwTotalWidth;
	}

	if (height)
	{
		*height = dwTotalHeight;
	}
}

void DC6Reference::IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback)
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

	if (bAtlassing)
	{
		int currentX = 0;
		int currentY = 0;
		for (int i = 0; i <= end - start; i++)
		{
			DC6Frame* pFrame = &image.pFrames[start + i];
			if (callback)
			{
				callback(DC6::GetPixelsAtFrame(&image, 0, start + i, nullptr), start + i,
					currentX, currentY, pFrame->fh.dwWidth, pFrame->fh.dwHeight);
			}

			currentX += image.dwMaxFrameWidth;
			if (currentX + image.dwMaxFrameWidth > 4096) // arbitrary cutoff
			{
				currentX = 0;
				currentY += image.dwMaxFrameHeight;
			}
		}
	}
	else
	{
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
	
}

void DC6Reference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight)
{
	*totalWidth = 0;
	*totalHeight = 0;

	int currentX = 0;
	int maxX = 0;
	int currentY = 0;
	for (int i = 0; i < image.header.dwFrames; i++)
	{
		if (i == frame)
		{
			*x = currentX;
			*y = currentY;
		}

		currentX += image.dwMaxFrameWidth;
		if (currentX + image.dwMaxFrameWidth > 4096)
		{
			maxX = currentX - image.dwMaxFrameWidth;
			currentX = 0;
			currentY += image.dwMaxFrameHeight;
		}
	}

	if (maxX == 0)
	{
		maxX = currentX;
	}

	*totalWidth = maxX + image.dwMaxFrameWidth;
	*totalHeight = currentY + image.dwMaxFrameHeight;
}

void DC6Reference::Deallocate()
{
	switch (usagePolicy)
	{
		case UsagePolicy_Permanent:
			graphicsManager->RemovePermanentDC6Graphic(filePath);
			break;
		case UsagePolicy_Temporary:
			// not implemented yet
			break;
	}
}

/**
 *	DT1GraphicsHandle is the IGraphicsHandle implementation for DT1 files.
 */

DT1Reference::DT1Reference(const char* fileName, GraphicsUsagePolicy usagePolicy)
	: IGraphicsReference(usagePolicy)
{
	D2Lib::strncpyz(filePath, fileName, sizeof(filePath));
}

size_t DT1Reference::GetTotalSizeInBytes(int32_t frame)
{
	return 0;
}

size_t DT1Reference::GetNumberOfFrames()
{
	return 0;
}

void DT1Reference::GetGraphicsData(void** pixels, int32_t frame,
	uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY)
{

}

void DT1Reference::GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

}

void DT1Reference::IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback)
{

}

void DT1Reference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight)
{

}

void DT1Reference::Deallocate()
{
	switch (usagePolicy)
	{
		case UsagePolicy_Permanent:
			graphicsManager->RemovePermanentDT1Graphic(filePath);
			break;
		case UsagePolicy_Temporary:
			// not implemented
			break;
	}
}

/**
 *	Font handles are used to load fonts.
 */
FontReference::FontReference(const char* graphicsFile, const char* tbl, GraphicsUsagePolicy usagePolicy)
	: IGraphicsReference(usagePolicy)
{
	tblHandle = TBLFont::RegisterFont(tbl);
	DC6::LoadImage(graphicsFile, &image);
	D2Lib::strncpyz(handleName, tbl, sizeof(handleName));
}

FontReference::~FontReference()
{
	DC6::UnloadImage(&image);
}

size_t FontReference::GetTotalSizeInBytes(int32_t frame)
{
	return 0;
}

size_t FontReference::GetNumberOfFrames()
{
	return 0;
}

void FontReference::GetGraphicsData(void** pixels, int32_t frame, uint32_t* width,
	uint32_t* height, int32_t* offsetX, int32_t* offsetY)
{
	TBLFontFile* fontFile = TBLFont::GetPointerFromHandle(tblHandle);
	if(!fontFile || frame >= 256)
	{
		return;
	}

	if(width)
	{
		*width = fontFile->glyphs[frame].nWidth;
	}
	if(height)
	{
		//*height = fontFile->glyphs[frame].nHeight;
		*height = image.pFrames[frame].fh.dwHeight;
	}
	if(offsetX)
	{
		*offsetX = fontFile->glyphs[frame].dwUnknown4;
	}
	if(offsetY)
	{
		*offsetY = fontFile->glyphs[frame].wUnknown3;
	}
}

void FontReference::GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end,
	uint32_t* width, uint32_t* height)
{
	uint32_t maxWidth = 0;
	uint32_t maxHeight = 0;
	uint32_t totalHeight = 0;

	TBLFontFile* fontFile = TBLFont::GetPointerFromHandle(tblHandle);
	
	uint32_t rowWidth = 0;
	for (int i = 0; i < 256; i++)
	{
		if (image.pFrames[i].fh.dwHeight > maxHeight)
		{
			maxHeight = image.pFrames[i].fh.dwHeight;
		}

		fontFile->glyphs[i].dwUnknown4 = rowWidth;
		fontFile->glyphs[i].wUnknown3 = totalHeight;

		rowWidth += image.pFrames[i].fh.dwWidth;
		if ((i + 1) % 16 == 0)
		{
			if (rowWidth > maxWidth)
			{
				maxWidth = rowWidth;
			}
			totalHeight += maxHeight;
			maxHeight = 0;
			rowWidth = 0;
		}
	}

	*width = maxWidth;
	*height = totalHeight;
}

void FontReference::IterateFrames(bool bAtlassing, int32_t start, int32_t end,
	AtlassingCallback callback)
{
	TBLFontFile* fontFile = TBLFont::GetPointerFromHandle(tblHandle);

	for (int i = 0; i < image.header.dwFrames; i++)
	{
		if (callback)
		{
			callback(DC6::GetPixelsAtFrame(&image, 0, i, nullptr),
				i, fontFile->glyphs[i].dwUnknown4,
				fontFile->glyphs[i].wUnknown3,
				image.pFrames[i].fh.dwWidth,
				image.pFrames[i].fh.dwHeight);
		}
	}
}

void FontReference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y,
	uint32_t* totalWidth, uint32_t* totalHeight)
{
	
}

float FontReference::GetCapHeight()
{
	TBLFontFile* fontFile = TBLFont::GetPointerFromHandle(tblHandle);

	return fontFile->nHeight;
}

void FontReference::Deallocate()
{
	graphicsManager->RemoveFont(handleName);
}

/**
 *	The graphics manager.
 */

GraphicsManager::GraphicsManager()
{

}

GraphicsManager::~GraphicsManager()
{
	// TODO: delete all permanent entries, this will leak memory !!
}

IGraphicsReference* GraphicsManager::CreateReference(const char* graphicsFile, GraphicsUsagePolicy policy)
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
				return new DC6Reference(graphicsFile, policy);

			case UsagePolicy_Permanent:
				if (!DC6Graphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					DC6Graphics.Insert(theHandle, graphicsFile, new DC6Reference(graphicsFile, policy));
				}
				return DC6Graphics[theHandle];

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
				return new DCCReference(graphicsFile, policy);

			case UsagePolicy_Permanent:
				if (!DCCGraphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					DCCGraphics.Insert(theHandle, graphicsFile, new DCCReference(graphicsFile, policy));
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
				return new DT1Reference(graphicsFile, policy);

			case UsagePolicy_Permanent:
				if (!DT1Graphics.Contains(graphicsFile, &theHandle, &bFull))
				{
					if (bFull)
					{
						return nullptr; // it's full
					}

					DT1Graphics.Insert(theHandle, graphicsFile, new DT1Reference(graphicsFile, policy));
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

void GraphicsManager::DeleteReference(IGraphicsReference* graphic)
{
	if (!graphic)
	{
		return;
	}
	graphic->Deallocate();
	graphic->UnloadGraphicsData();
	delete graphic;
}

IGraphicsReference* GraphicsManager::LoadFont(const char* fontGraphic,
	const char* fontTBL)
{
	handle theHandle;
	bool bFull;

	if (!Fonts.Contains(fontTBL, &theHandle, &bFull))
	{
		if (bFull)
		{
			return nullptr; // it's full
		}

		Fonts.Insert(theHandle, fontTBL, new FontReference(fontGraphic, fontTBL, UsagePolicy_Permanent));
	}
	return Fonts[theHandle];
}

void GraphicsManager::RemovePermanentDCCGraphic(const char* key)
{
	DCCGraphics.Erase(key);
}

void GraphicsManager::RemovePermanentDC6Graphic(const char* key)
{
	DC6Graphics.Erase(key);
}

void GraphicsManager::RemovePermanentDT1Graphic(const char* key)
{
	DT1Graphics.Erase(key);
}

void GraphicsManager::RemoveFont(const char* key)
{
	Fonts.Erase(key);
}