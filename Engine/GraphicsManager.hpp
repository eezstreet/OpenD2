#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Shared/D2HashMap.hpp"

/**
 *	IGraphicsHandle is just a generic interface for handling various graphic formats in D2.
 */
class IGraphicsHandle
{
public:
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
	virtual void GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height) = 0;

	typedef void(*AtlassingCallback)(void* pixels, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH);
	/**
	 *	Iterates over all frames in the graphic, running an AtlassingCallback for every frame.
	 *	@param callback   The callback for the atlassing function.
	 *	@param start      The first frame to iterate over
	 *	@param end        The last frame to iterate over (negative to iterate over all frames)
	 */
	virtual void IterateFrames(AtlassingCallback callback, int32_t start, int32_t end) = 0;

	/**
	 *	Gets the graphics data for a subset of frames.
	 *	The outputted pixels and where they are placed is implementation-specific.
	 *	@param start      The first frame to query
	 *	@param end        The last frame to query (negative to query until the end)
	 *	@param width      Optional output: width of pixels outputted
	 *	@param height     Optional output: height of pixels outputted
	 */
	virtual void GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height) = 0;
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
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height);
	virtual void GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(AtlassingCallback callback, int32_t start, int32_t end);
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
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height);
	virtual void GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(AtlassingCallback callback, int32_t start, int32_t end);
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
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height);
	virtual void GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(AtlassingCallback callback, int32_t start, int32_t end);
};

/**
 *	PL2GraphicsHandle is the IGraphicsHandle implementation for PL2 files.
 */
class PL2GraphicsHandle : public IGraphicsHandle
{
private:
	fs_handle fileHandle;
	PL2File file;

public:
	PL2GraphicsHandle(const char* fileName);
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame, uint32_t* width, uint32_t* height);
	virtual void GetGraphicsInfo(int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(AtlassingCallback callback, int32_t start, int32_t end);
};


/**
 *	GraphicsManager provides a format-agnostic interface for handling graphics files.
 *	DCC, DC6, DT1, PL2 ---> IGraphicsHandle, which is queried for stats and pixels.
 */
class GraphicsManager : public IGraphicsManager
{
private:
	HashMap<char, DCCGraphicsHandle> DCCGraphics;
	HashMap<char, DC6GraphicsHandle> DC6Graphics;
	HashMap<char, DT1GraphicsHandle> DT1Graphics;
	HashMap<char, PL2GraphicsHandle> PL2Graphics;

public:
	GraphicsManager();
	~GraphicsManager();

	virtual IGraphicsHandle* LoadGraphic(const char* graphicsFile, 
			GraphicsUsagePolicy policy);
};

extern GraphicsManager* graphicsManager;
