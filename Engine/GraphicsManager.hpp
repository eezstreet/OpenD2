#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Shared/D2HashMap.hpp"
#include "DC6.hpp"
#include "Palette.hpp"
#include "Renderer.hpp"

/**
 *	GraphicsLRU is responsible for keeping track of temporary graphics.
 */
class GraphicsLRU
{
private:
	
public:
};

/**
 *	IGraphicsHandle is just a generic interface for handling various graphic formats in D2.
 */
class IGraphicsReference
{
protected:
	void* loadedGraphicsData;
	bool bAreGraphicsLoaded;
	GraphicsUsagePolicy usagePolicy;
	int loadedGraphicsFrame;

public:
	/**
	 *	Default constructor, takes a usage policy.
	 */
	IGraphicsReference(GraphicsUsagePolicy policy)
	{
		usagePolicy = policy;
		bAreGraphicsLoaded = false;
		loadedGraphicsData = nullptr;
	}


	/**
	 *	Returns true if the graphics have been fully loaded.
	 */
	virtual bool AreGraphicsLoaded()
	{
		return bAreGraphicsLoaded;
	}

	/**
	 *	Returns the loaded graphics data.
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
	 *	Removes the unloaded graphics data
	 */
	virtual void UnloadGraphicsData()
	{
		if (bAreGraphicsLoaded)
		{
			RenderTarget->DeleteLoadedGraphicsData(loadedGraphicsData, this);
		}
	}

	/**
	 *	Returns the loaded graphics frame.
	 */
	virtual int GetLoadedGraphicsFrame()
	{
		return loadedGraphicsFrame;
	}

	/**
	 *	Sets the loaded graphics handle.
	 */
	virtual void SetLoadedGraphicsData(void* data, int _loadedGraphicsFrame)
	{
		loadedGraphicsData = data;
		loadedGraphicsFrame = _loadedGraphicsFrame;
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

	/**
	 *	Specifies what to do when we have been deallocated.
	 */
	virtual void Deallocate() = 0;
};

/**
 *	DCCGraphicsHandle is the IGraphicsHandle implementation for DCC files.
 */
class DCCReference : public IGraphicsReference
{
private:
	char dccHandleName[MAX_D2PATH];

public:
	DCCReference(const char* fileName, GraphicsUsagePolicy usagePolicy);
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
	virtual void Deallocate();
};

/**
 *	DC6GraphicsHandle is the IGraphicsHandle implementation for DC6 files.
 */
class DC6Reference : public IGraphicsReference
{
private:
	char filePath[MAX_D2PATH];
	DC6Image image;
	bool bLoaded = false;
public:
	DC6Reference(const char* fileName, GraphicsUsagePolicy usagePolicy);
	~DC6Reference();
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
	virtual void Deallocate();
};

/**
 *	DT1GraphicsHandle is the IGraphicsHandle implementation for DT1 files.
 */
class DT1Reference : public IGraphicsReference
{
private:
	char filePath[MAX_D2PATH];

public:
	DT1Reference(const char* fileName, GraphicsUsagePolicy usagePolicy);
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame, 
		 uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
	virtual void Deallocate();
};

/**
 *	FontGraphicsHandle is the IGraphicsHandle implementation of fonts.
 */
class FontReference : public IGraphicsReference
{
private:
	DC6Image image;
	tbl_handle tblHandle;
	char handleName[MAX_D2PATH];

public:
	FontReference(const char* graphicsFile, const char* tbl, GraphicsUsagePolicy usagePolicy);
	~FontReference();

	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight);
	virtual float GetCapHeight() override;
	virtual void Deallocate();
};

/**
 *	ITokenReference is the base class of all token references.
 */
class ITokenReference
{
protected:
	char tokenName[4];
public:
	virtual D2TokenType GetTokenType() = 0;
	virtual const char* GetTokenFolder() = 0;
	const char* GetTokenName() { return tokenName; }

	ITokenReference(const char* tokenName);
};

/**
 *	Monster token reference
 */
class MonsterTokenReference : public ITokenReference
{
public:
	virtual D2TokenType GetTokenType() { return TOKEN_MONSTER; }
	virtual const char* GetTokenFolder() { return "MONSTERS"; }

	MonsterTokenReference(const char* tokenName);
};

/**
 *	Object token reference
 */
class ObjectTokenReference : public ITokenReference
{
public:
	virtual D2TokenType GetTokenType() { return TOKEN_OBJECT; }
	virtual const char* GetTokenFolder() { return "OBJECTS"; }

	ObjectTokenReference(const char* tokenName);
};

/**
 *	Player token reference
 */
class PlayerTokenReference : public ITokenReference
{
public:
	virtual D2TokenType GetTokenType() { return TOKEN_CHAR; }
	virtual const char* GetTokenFolder() { return "CHARS"; }

	PlayerTokenReference(const char* tokenName);
};

/**
 *	GraphicsManager provides a format-agnostic interface for handling graphics files.
 *	DCC, DC6, DT1, PL2 ---> IGraphicsHandle, which is queried for stats and pixels.
 */
class GraphicsManager : public IGraphicsManager
{
private:
	HashMap<char, DCCReference*> DCCGraphics;
	HashMap<char, DC6Reference*> DC6Graphics;
	HashMap<char, DT1Reference*> DT1Graphics;
	HashMap<char, FontReference*> Fonts;
	HashMap<char, MonsterTokenReference*> MonsterTokens;
	HashMap<char, ObjectTokenReference*> ObjectTokens;
	HashMap<char, PlayerTokenReference*> PlayerTokens;

public:
	GraphicsManager();
	~GraphicsManager();

	virtual IGraphicsReference* CreateReference(const char* graphicsFile,
			GraphicsUsagePolicy policy);

	virtual ITokenReference* CreateReference(const D2TokenType& tokenType,
		const char* tokenName);

	virtual void DeleteReference(IGraphicsReference* handle);
	virtual void DeleteReference(ITokenReference* handle);

	virtual IGraphicsReference* LoadFont(const char* fontGraphic,
		const char* fontTBL);

	// Individual graphics handle functions
	inline void RemovePermanentDCCGraphic(const char* key);
	inline void RemovePermanentDC6Graphic(const char* key);
	inline void RemovePermanentDT1Graphic(const char* key);
	inline void RemoveFont(const char* key);
};

extern GraphicsManager* graphicsManager;
