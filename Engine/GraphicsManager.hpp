#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Shared/D2HashMap.hpp"
#include "DC6.hpp"
#include "Palette.hpp"

/**
 *	IGraphicsHandle is just a generic interface for handling various graphic formats in D2.
 */
class IGraphicsHandle
{
private:
	void* loadedGraphicsData;
	bool bAreGraphicsLoaded;

public:
	/**
	 *	Returns true if the graphics have been fully loaded.
	 */
	virtual bool AreGraphicsLoaded()
	{
		return bAreGraphicsLoaded;
	}

	/**
	 *	Returns the loaded graphics handle.
	 */
	virtual void* GetLoadedGraphicsData()
	{
		if(bAreGraphicsLoaded)
		{
			return loadedGraphicsData;
		}
		return nullptr;
	}

	/**
	 *	Sets the loaded graphics handle.
	 */
	virtual void SetLoadedGraphicsData(void* data)
	{
		loadedGraphicsData = data;
		bAreGraphicsLoaded = true;
	}

	/**
	 *	Gets the total size in bytes of this graphics handle.
	 *	@param frame     The frame to query, or -1 for all frames.
	 */
	virtual size_t GetTotalSizeInBytes(int32_t frame) = 0;

	/**
	 *	Gets the total number of frames.
	 */
	virtual size_t GetNumberOfFrames() = 0;

	/**
	 *	Gets the graphics data for a particular frame.
	 *	@param pixels     Output: pixels to write to
	 *	@param frame      The frame to query
	 *	@param width      Optional output: width of the frame.
	 *	@param height     Optional output: height of the frame. 
	 */
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY) = 0;

	typedef void(*AtlassingCallback)(void* pixels, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH);
	/**
	 *	Iterates over all frames in the graphic, running an AtlassingCallback for every frame.
	 *	@param callback   The callback for the atlassing function.
	 *	@param start      The first frame to iterate over
	 *	@param end        The last frame to iterate over (negative to iterate over all frames)
	 */
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback) = 0;

	/**
	 *	Gets the graphics data for a subset of frames.
	 *	The outputted pixels and where they are placed is implementation-specific.
	 *	@param start      The first frame to query
	 *	@param end        The last frame to query (negative to query until the end)
	 *	@param width      Optional output: width of pixels outputted
	 *	@param height     Optional output: height of pixels outputted
	 */
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height) = 0;

	/**
	 *	Gets the atlassed information for a resource and a frame.
	 */
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight) = 0;

	/**
	 *	Get cap height (for font resource only)
	 */
	virtual float GetCapHeight() { return 0.0f; }
};

/**
 *	DCCGraphicsHandle is the IGraphicsHandle implementation for DCC files.
 */
class DCCGraphicsHandle : public IGraphicsHandle
{
private:
	fs_handle fileHandle;

public:
	DCCGraphicsHandle(const char* fileName);
	DCCGraphicsHandle() {}
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
};

/**
 *	DC6GraphicsHandle is the IGraphicsHandle implementation for DC6 files.
 */
class DC6GraphicsHandle : public IGraphicsHandle
{
private:
	char filePath[MAX_D2PATH];
	DC6Image image;
	bool bLoaded = false;
public:
	DC6GraphicsHandle(const char* fileName);
	~DC6GraphicsHandle();
	DC6GraphicsHandle() {}
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
};

/**
 *	DT1GraphicsHandle is the IGraphicsHandle implementation for DT1 files.
 */
class DT1GraphicsHandle : public IGraphicsHandle
{
private:
	fs_handle fileHandle;

public:
	DT1GraphicsHandle(const char* fileName);
	DT1GraphicsHandle() {}
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame, 
		 uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
};

/**
 *	FontGraphicsHandle is the IGraphicsHandle implementation of fonts.
 */
class FontGraphicsHandle : public IGraphicsHandle
{
private:
	DC6Image image;
	tbl_handle tblHandle;

public:
	FontGraphicsHandle(const char* graphicsFile, const char* tbl);
	~FontGraphicsHandle();

	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
	virtual float GetCapHeight() override;
};


/**
 *	GraphicsManager provides a format-agnostic interface for handling graphics files.
 *	DCC, DC6, DT1, PL2 ---> IGraphicsHandle, which is queried for stats and pixels.
 */
class GraphicsManager : public IGraphicsManager
{
private:
	HashMap<char, DCCGraphicsHandle*> DCCGraphics;
	HashMap<char, DC6GraphicsHandle*> DC6Graphics;
	HashMap<char, DT1GraphicsHandle*> DT1Graphics;
	HashMap<char, PL2GraphicsHandle*> PL2Graphics;
	HashMap<char, FontGraphicsHandle*> Fonts;

public:
	GraphicsManager();
	~GraphicsManager();

	virtual IGraphicsHandle* LoadGraphic(const char* graphicsFile, 
			GraphicsUsagePolicy policy);

	virtual void UnloadGraphic(IGraphicsHandle* handle);

	virtual IGraphicsHandle* LoadFont(const char* fontGraphic,
		const char* fontTBL);
};

extern GraphicsManager* graphicsManager;
