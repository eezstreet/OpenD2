#include "GraphicsManager.hpp"
#include "COF.hpp"
#include "DC6.hpp"
#include "DT1.hpp"
#include "Logging.hpp"
#include "FileSystem.hpp"
#include "Palette.hpp"
#include "TBL_Font.hpp"
#include <atomic>

GraphicsManager* graphicsManager;

/**
 *	DCCGraphicsHandle is the IGraphicsHandle implementation for DCC files.
 */

DCCReference::DCCReference(const char* fileName, GraphicsUsagePolicy usagePolicy)
	: IGraphicsReference(usagePolicy)
{
	D2Lib::strncpyz(dccHandleName, fileName, sizeof(dccHandleName));
	bLoaded = false;
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
	uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY, int direction)
{
	if (direction < 0)
	{
		direction = 0;
	}

	if (width)
	{
		*width = dccFile.directions[direction].frames[frame].dwWidth;
	}

	if (height)
	{
		*height = dccFile.directions[direction].frames[frame].dwHeight;
	}

	// The destination rectangle is oriented from the upper left corner, but whenever we do a draw call,
	// we are orienting from the "base point" of the token's DCC files. So we need to correct that.
	if (offsetX)
	{
		*offsetX = -(dccFile.directions[direction].frames[frame].nMinX - dccFile.directions[direction].nMinX);
		*offsetX += dccFile.directions[direction].frames[frame].nXOffset;
	}

	if (offsetY)
	{
		*offsetY = -(dccFile.directions[direction].frames[frame].nMinY - dccFile.directions[direction].nMinY);
		*offsetY += dccFile.directions[direction].frames[frame].nYOffset;
		*offsetY -= dccFile.nDirectionH[direction] - 1;
	}
}

void DCCReference::GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{

}

void DCCReference::IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback)
{

}

void DCCReference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int direction)
{
	if (direction < 0)
	{
		direction = 0;
	}

	if (x)
	{
		*x = xOffsetForFrame[direction][frame];
	}

	if (y)
	{
		*y = yOffsetForFrame[direction][frame];
	}

	if (totalWidth)
	{
		*totalWidth = dccFile.directions[direction].nWidth * dccFile.header.dwFramesPerDirection;
	}

	if (totalHeight)
	{
		*totalHeight = dccFile.directions[direction].nHeight;
	}
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

void* DCCReference::LoadSingleDirection(unsigned int direction, AnimTextureAllocCallback allocCallback, AnimTextureDecodeCallback decodeCallback)
{
	static AnimTextureDecodeCallback g_decoder;
	static AnimTextureAllocCallback g_allocator;
	static void* g_newData;
	static int g_frameStart;
	static unsigned int* g_frameXOffsets;
	static unsigned int* g_frameYOffsets;
	static unsigned int* g_directionWidth;
	static unsigned int* g_directionHeight;
	uint32_t tempDirectionWidth, tempDirectionHeight;

	if (!bLoaded)
	{
		LoadDCCFile();
	}

	if (directionCount <= 0 || direction == -1)
	{
		if (bAreGraphicsLoaded)
		{
			return loadedGraphicsData;
		}

		direction = 0;
	}
	else if (bAreGraphicsLoadedForDirection[direction])
	{
		return loadedGraphicsForDirection[direction];
	}

#ifdef PAUL_SIRAMY_DCC
	g_allocator = allocCallback;
	g_decoder = decodeCallback;
	g_frameStart = 0;
	g_frameXOffsets = xOffsetForFrame[direction];
	g_frameYOffsets = yOffsetForFrame[direction];
	g_directionWidth = &tempDirectionWidth;
	g_directionHeight = &tempDirectionHeight;
	DCC::DecodeDirection(&dccFile, 0, [](unsigned int width, unsigned int height) {
		g_newData = g_allocator(width, height);
	}, [](BYTE* bitmap, uint32_t frameNum, int32_t frameX, int32_t frameY, uint32_t frameW, uint32_t frameH) {
		g_frameXOffsets[frameNum] = frameX;
		g_frameYOffsets[frameNum] = frameY;
		g_decoder((void*)bitmap, g_newData, frameNum, frameX, frameY, frameW, frameH);
		g_frameStart += frameW;
		*g_directionWidth = g_frameStart;
		if (frameY + frameH > *g_directionHeight)
		{
			*g_directionHeight = frameY + frameH;
		}
	});
#else
	// Do alloc callback?
	DCC::GetDirectionSize(&dccFile, direction, &this->directionWidth[direction], &this->directionHeight[direction]);
	g_newData = allocCallback(this->directionWidth[direction], this->directionHeight[direction]);
	g_decoder = decodeCallback;
	tempDirectionWidth = this->directionWidth[direction];
	tempDirectionHeight = this->directionHeight[direction];

	// Do decode
	g_frameStart = 0;
	g_frameXOffsets = xOffsetForFrame[direction];
	g_frameYOffsets = yOffsetForFrame[direction];
	g_directionWidth = &tempDirectionWidth;
	g_directionHeight = &tempDirectionHeight;
	DCC::DecodeDirection(&dccFile, 0, [](BYTE* bitmap, uint32_t frameNum, int32_t frameX, int32_t frameY,
		uint32_t frameW, uint32_t frameH) {
			g_frameXOffsets[frameNum] = g_frameStart/* + frameX*/;
			g_frameYOffsets[frameNum] = frameY;
			g_decoder((void*)bitmap, g_newData, frameNum, g_frameStart, frameY, frameW, frameH);
			g_frameStart += frameW;
			*g_directionWidth = g_frameStart;
			if (frameY + frameH > *g_directionHeight)
			{
				*g_directionHeight = frameY + frameH;
			}
	});
#endif

	bAreGraphicsLoadedForDirection[direction] = true;
	loadedGraphicsForDirection[direction] = g_newData;

	return g_newData;
}

void DCCReference::LoadDCCFile()
{
	DCC::LoadAnimation(dccHandleName, &dccFile);
	bLoaded = true;
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
	uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY, int direction)
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

void DC6Reference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int direction)
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
	dt1Handle = DT1::LoadDT1(fileName);
	D2Lib::strncpyz(filePath, fileName, MAX_D2PATH);
}

size_t DT1Reference::GetTotalSizeInBytes(int32_t frame)
{
	return 0; // FIXME?
}

size_t DT1Reference::GetNumberOfFrames()
{
	return DT1::GetNumBlocks(dt1Handle);
}

void DT1Reference::GetGraphicsData(void** pixels, int32_t frame,
	uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY, int direction)
{
	// not needed
}

void DT1Reference::GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height)
{
	size_t numBlocks = GetNumberOfFrames();
	uint32_t widthTemp = 0;
	uint32_t heightTemp = 0;

	if (end < 0 || end >= numBlocks)
	{ // bounds-check
		end = numBlocks - 1;
	}
	if (start < 0 || start > numBlocks)
	{ // bound-check
		start = 0;
	}
	if (start > end)
	{	// flip em
		int32_t temp = start;
		start = end;
		end = temp;
	}

	for (int32_t i = start; i <= end; i++)
	{
		const DT1File::DT1Block* block = DT1::GetBlock(dt1Handle, i);
		widthTemp += block->sizeX;
		if (block->sizeY > heightTemp)
		{
			heightTemp = block->sizeY;
		}
	}

	if (width)
	{
		*width = widthTemp;
	}

	if (height)
	{
		*height = heightTemp;
	}
}

void DT1Reference::IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback)
{
	size_t numBlocks = GetNumberOfFrames();

	// do bounds checking and all that
	if (end < 0 || end >= numBlocks)
	{
		end = numBlocks - 1;
	}
	if (start < 0 || start > numBlocks)
	{
		start = 0;
	}
	if (start > end)
	{
		int32_t temp = start;
		start = end;
		end = temp;
	}

	// pretty easy, get each block and go
	for (int32_t i = start; i < end; i++)
	{
		uint32_t width, height;
		int32_t x, y;
		void* pix = DT1::DecodeDT1Block(dt1Handle, i, width, height, x, y);
		callback(pix, i, x, y, width, height);
	}
}

void DT1Reference::GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int direction)
{
	// not needed
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
	uint32_t* height, int32_t* offsetX, int32_t* offsetY, int direction)
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
	uint32_t* totalWidth, uint32_t* totalHeight, int direction)
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
 *	Base class for all token types.
 */
void ITokenReference::SetTokenName(const char* _tokenName)
{
	int i;
	for (i = 0; i < 4 && _tokenName[i]; i++)
	{
		tokenName[i] = _tokenName[i];
	}

	if (i < 4)
	{
		tokenName[i] = '\0';
	}

	memset(cofFiles, INVALID_HANDLE, sizeof(cofFiles));
}

void ITokenReference::LoadCOF(unsigned int mode, unsigned int hitclass)
{
	cofFiles[mode][hitclass] = COF::Register(GetTokenDataFolder(), GetTokenName(), GetModeName(mode), GetHitclassName(hitclass));
	Log_ErrorAssertVoidReturn(cofFiles[mode][hitclass] != INVALID_HANDLE);
}

IGraphicsReference* ITokenReference::GetTokenGraphic(unsigned int component, unsigned int hitclass,
	unsigned int mode, const char* armorClass)
{
	IGraphicsReference* reference;
	char ref[8];
	unsigned short packed;
	bool hasFallback = false;

	// See explanation in ITokenReference for how this is used.
	packed = ((unsigned short)mode << 10) | ((unsigned short)hitclass << 5) | (unsigned short)component;
	ref[0] = (char)((packed & 0xFF00) >> 8) + 1; // this is to make sure we don't hit any null terminators
	ref[1] = (char)(packed & 0x00FF) + 1; // see above
	strncpy(&ref[2], armorClass, sizeof(ref) - 2);

	handle graphicHandle;
	if (m_cachedGraphicsReferences.Contains(ref, &graphicHandle))
	{
		return m_cachedGraphicsReferences[graphicHandle];
	}

	char path[MAX_D2PATH];

	snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s%s%s%s%s.dcc",
		GetTokenDataFolder(), GetTokenName(), GetComponentName(component),
		GetTokenName(), GetComponentName(component), armorClass, GetModeName(mode), GetHitclassName(hitclass));

	reference = graphicsManager->CreateReference(path, UsagePolicy_Permanent); // for now
	if (reference == nullptr && hitclass == WC_HT2)
	{	// freak case; RH always uses HT1. don't ask.
		snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s%s%s%sHT1.dcc",
			GetTokenDataFolder(), GetTokenName(), GetComponentName(component),
			GetTokenName(), GetComponentName(component), armorClass, GetModeName(mode));
		reference = graphicsManager->CreateReference(path, UsagePolicy_Permanent); // for now
	}

	if (reference == nullptr)
	{
		// File not found - try using "HTH" as a fallback for the hitclass
		if (hitclass != WC_HTH)
		{
			snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s%s%s%sHTH.dcc",
				GetTokenDataFolder(), GetTokenName(), GetComponentName(component),
				GetTokenName(), GetComponentName(component), armorClass, GetModeName(mode));
			reference = graphicsManager->CreateReference(path, UsagePolicy_Permanent); // for now
		}

		if (reference == nullptr && hitclass != WC_1HS)
		{
			// Try 1hs as a fallback?
			snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s%s%s%s1HS.dcc",
				GetTokenDataFolder(), GetTokenName(), GetComponentName(component),
				GetTokenName(), GetComponentName(component), armorClass, GetModeName(mode));
			reference = graphicsManager->CreateReference(path, UsagePolicy_Permanent); // for now
		}

		if (reference == nullptr)
		{
			snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s%s%s%s%s.dc6",
				GetTokenDataFolder(), GetTokenName(), GetComponentName(component),
				GetTokenName(), GetComponentName(component), armorClass, GetModeName(mode), GetHitclassName(hitclass));
			reference = graphicsManager->CreateReference(path, UsagePolicy_Permanent);

			if (reference == nullptr && hitclass != WC_HTH)
			{	// try using "HTH" as a fallback for the hitclass
				snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s%s%s%sHTH.dc6",
					GetTokenDataFolder(), GetTokenName(), GetComponentName(component),
					GetTokenName(), GetComponentName(component), armorClass, GetModeName(mode));
				reference = graphicsManager->CreateReference(path, UsagePolicy_Permanent);
			}
		}
	}

	// Revert to a fallback.
	if (reference == nullptr)
	{
		GetFallbackForComponentMode(hasFallback, mode);
		if (hasFallback)
		{
			return GetTokenGraphic(component, hitclass, mode, armorClass);
		}
	}

	Log_ErrorAssertReturn(reference != nullptr, reference);

	m_cachedGraphicsReferences[ref] = reference;
	return reference;
}

bool ITokenReference::HasComponentForMode(unsigned int component, unsigned int hitclass, unsigned int mode)
{
	// Don't render shield in certain modes
	if (component == COMP_SHIELD)
	{
		switch (hitclass)
		{
			case WC_1JS:
			case WC_1JT:
			case WC_1SS:
			case WC_1ST:
			case WC_BOW:
			case WC_2HS:
			case WC_XBW:
			case WC_HT2:
				return false;
		}
	}
	// Load COF if it isn't already loaded
	if (cofFiles[mode][hitclass] == INVALID_HANDLE)
	{
		LoadCOF(mode, hitclass);
	}

	return COF::LayerPresent(cofFiles[mode][hitclass], component);
}

const char* ITokenReference::GetHitclassName(unsigned int hitclass)
{
	static const char* HitclassNames[WC_MAX] = {
		"XXX", "HTH", "BOW", "1HS", "1HT", "STF", "2HS", "2HT", "XBW",
		"1JS", "1JT", "1SS", "1ST", "HT1", "HT2"
	};

	return HitclassNames[hitclass];
}

const char* ITokenReference::GetModeName(unsigned int mode)
{
	static const char* MonsterModes[MONMODE_MAX] = {
		"DT", "NU", "WL", "GH", "A1", "A2", "BL", "SC",
		"S1", "S2", "S3", "S4", "DD", "KB", "XX", "RN"
	};

	static const char* PlayerModes[PLRMODE_MAX] = {
		"DT", "NU", "WL", "RN", "GH", "TN", "TW", "A1",
		"A2", "BL", "SC", "TH", "KK", "S1", "S2", "S3",
		"S4", "DD", "SQ", "KB"
	};

	static const char* ObjectModes[OBJMODE_MAX] = {
		"NU", "OP", "ON", "S1", "S2", "S3", "S4", "S5"
	};

	switch (GetTokenType())
	{
		default:
		case TOKEN_MONSTER:
			return MonsterModes[mode];
		case TOKEN_CHAR:
			return PlayerModes[mode];
		case TOKEN_OBJECT:
			return ObjectModes[mode];
	}
}

const char* ITokenReference::GetComponentName(unsigned int component)
{
	static const char* ComponentNames[COMP_MAX] = {
		"HD", "TR", "LG", "RA", "LA", "RH", "LH", "SH",
		"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8"
	};

	return ComponentNames[component];
}

const char* ITokenReference::GetTokenDataFolder()
{
	switch (GetTokenType())
	{
		default:
		case TOKEN_MONSTER:
			return "monsters";
		case TOKEN_CHAR:
			return "chars";
		case TOKEN_OBJECT:
			return "objects";
	}
}

inline const BYTE ITokenReference::GetNumberOfFrames(int mode, int weaponClass)
{
	// Load COF if it isn't already loaded
	if (cofFiles[mode][weaponClass] == INVALID_HANDLE)
	{
		LoadCOF(mode, weaponClass);
	}

	return COF::GetFileData(cofFiles[mode][weaponClass])->header.nFrames;
}

inline const BYTE ITokenReference::GetNumberOfDirections(int mode, int weaponClass)
{
	// Load COF if it isn't already loaded
	if (cofFiles[mode][weaponClass] == INVALID_HANDLE)
	{
		LoadCOF(mode, weaponClass);
	}

	return COF::GetFileData(cofFiles[mode][weaponClass])->header.nDirs;
}

IGraphicsReference* GetTokenGraphic(unsigned int component, unsigned int hitclass,
	unsigned int mode, const char* armorClass)
{
	// FIXME
	return nullptr;
}

/**
 *	Monster token type
 */
MonsterTokenReference::MonsterTokenReference(const char* tokenName)
{
	SetTokenName(tokenName);
}

void MonsterTokenReference::GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode)
{
	switch (mode)
	{
		case MONMODE_KB:
		case MONMODE_BL:
			hasFallback = true;
			mode = MONMODE_GH;
			return;
		case MONMODE_A2:
			hasFallback = true;
			mode = MONMODE_A1;
			return;
		case MONMODE_RN:
			hasFallback = true;
			mode = MONMODE_WL;
			return;
	}
}

/**
 *	Object token type
 */
ObjectTokenReference::ObjectTokenReference(const char* tokenName)
{
	SetTokenName(tokenName);
}

void ObjectTokenReference::GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode)
{
	// object tokens don't have any component-mode fallbacks
}

/**
 *	Player token type
 */
PlayerTokenReference::PlayerTokenReference(const char* tokenName)
{
	SetTokenName(tokenName);
}

void PlayerTokenReference::GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode)
{
	switch (mode)
	{
		case PLRMODE_KB:
		case PLRMODE_BL:
			hasFallback = true;
			mode = PLRMODE_GH;
			return;
		case PLRMODE_A2:
			hasFallback = true;
			mode = PLRMODE_A1;
			return;
		case PLRMODE_TN:
			hasFallback = true;
			mode = PLRMODE_NU;
			return;
		case PLRMODE_TW:
		case PLRMODE_RN:
			hasFallback = true;
			mode = PLRMODE_WL;
			return;
	}
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

	// Try and load the file first
	fs_handle f;
	FS::Open(graphicsFile, &f, FS_READ);
	if (f == INVALID_HANDLE)
	{
		return nullptr;
	}
	FS::CloseFile(f);
	//

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

/**
 *	The vast, overwhelming majority of tokens are in DCC format.
 *	However, there are a few outliers that aren't. They are:
 *		- Mephisto
 *		- Tyrael1, Tyrael2, Tyrael3
 *		- Diablo (death animations only)
 *		- Maggot Queen (death animations only)
 *		- Baal (death animations only, all variations)
 *		- Hell Gate on Act 3
 *		- Worldstone Portal on Act 5
 */
ITokenReference* GraphicsManager::CreateReference(const D2TokenType& tokenType, 
	const char* tokenName)
{
	handle theHandle;
	bool bFull;

	switch (tokenType)
	{
		case TOKEN_MONSTER:
			if (!MonsterTokens.Contains(tokenName, &theHandle, &bFull))
			{
				if (bFull)
				{
					return nullptr; // no room available
				}

				MonsterTokens.Insert(theHandle, tokenName, new MonsterTokenReference(tokenName));
			}
			return MonsterTokens[theHandle];

		case TOKEN_OBJECT:
			if (!ObjectTokens.Contains(tokenName, &theHandle, &bFull))
			{
				if (bFull)
				{
					return nullptr; // no room available
				}

				ObjectTokens.Insert(theHandle, tokenName, new ObjectTokenReference(tokenName));
			}
			return ObjectTokens[theHandle];

		case TOKEN_CHAR:
			if (!PlayerTokens.Contains(tokenName, &theHandle, &bFull))
			{
				if (bFull)
				{
					return nullptr; // no room available
				}

				PlayerTokens.Insert(theHandle, tokenName, new PlayerTokenReference(tokenName));
			}
			return PlayerTokens[theHandle];
	}
	return nullptr; // invalid type ?
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

void GraphicsManager::DeleteReference(ITokenReference* token)
{
	if (!token)
	{
		return;
	}

	switch (token->GetTokenType())
	{
		case TOKEN_MONSTER:
			MonsterTokens.Erase(token->GetTokenName());
			break;
		case TOKEN_OBJECT:
			ObjectTokens.Erase(token->GetTokenName());
			break;
		case TOKEN_CHAR:
			PlayerTokens.Erase(token->GetTokenName());
			break;
	}

	delete token;
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