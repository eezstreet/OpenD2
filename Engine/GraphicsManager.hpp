#pragma once
#include "../Shared/D2Shared.hpp"
#include "../Shared/D2HashMap.hpp"
#include "DC6.hpp"
#include "DCC.hpp"
#include "Palette.hpp"
#include "Renderer.hpp"

typedef void (*AtlassingCallback)(void* pixels, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH);
typedef void* (*AnimTextureAllocCallback)(unsigned int directionWidth, unsigned int directionHeight);
typedef void (*AnimTextureDecodeCallback)(void* pixels, void* extraData, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH);

/**
 *	IGraphicsReference is just a generic interface for handling various graphic formats in D2.
 *	FIXME: There's a lot of methods here that are only used for specific cases.
 *	This leads to a lot of empty stubs, but the advantage is that we aren't using potentially expensive RTTI.
 *	Is there maybe a better way that we can organize it?
 */
class IGraphicsReference
{
protected:
	union
	{
		void* loadedGraphicsData;
		void* loadedGraphicsForDirection[MAX_DIRECTIONS * 2]; // FIXME
	};
	union
	{
		bool bAreGraphicsLoaded;
		bool bAreGraphicsLoadedForDirection[MAX_DIRECTIONS * 2];
	};

	int directionCount;
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
		directionCount = -1;
		memset(bAreGraphicsLoadedForDirection, 0, sizeof(bool) * MAX_DIRECTIONS * 2);
		memset(loadedGraphicsForDirection, 0, sizeof(void*) * MAX_DIRECTIONS * 2);
	}

	/**
	 *	Default virtual destructor for virtual methods, takes a usage policy.
	 */
	~IGraphicsReference()
	{
		//delete this;
	}

	/**
	 *	Returns true if the graphics have been fully loaded.
	 */
	virtual bool AreGraphicsLoaded(int direction = -1)
	{
		if (direction == -1)
		{
			return bAreGraphicsLoaded;
		}
		return bAreGraphicsLoadedForDirection[direction];
	}

	/**
	 *	Returns the loaded graphics data.
	 */
	virtual void* GetLoadedGraphicsData(int direction = -1)
	{
		if (direction == -1)
		{
			if (bAreGraphicsLoaded)
			{
				return loadedGraphicsData;
			}
		}
		else
		{
			return loadedGraphicsForDirection[direction];
		}
		return nullptr;
	}

	/**
	 *	Removes the unloaded graphics data
	 */
	virtual void UnloadGraphicsData(int direction = ALL_DIRECTIONS)
	{
		if (direction == -1 || directionCount == -1)
		{
			if (bAreGraphicsLoaded)
			{
				RenderTarget->DeleteLoadedGraphicsData(loadedGraphicsData, this);
			}
		}
		else if (direction == ALL_DIRECTIONS)
		{
			for (int i = 0; i < directionCount; i++)
			{
				if (bAreGraphicsLoadedForDirection[i])
				{
					RenderTarget->DeleteLoadedGraphicsData(loadedGraphicsForDirection[i], this);
				}
			}
		}
		else if(bAreGraphicsLoadedForDirection[direction])
		{
			RenderTarget->DeleteLoadedGraphicsData(loadedGraphicsForDirection[direction], this);
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
	virtual void SetLoadedGraphicsData(void* data, int _loadedGraphicsFrame = -1, int direction = -1)
	{
		loadedGraphicsFrame = _loadedGraphicsFrame;

		if (direction != -1 && directionCount != -1 && direction < directionCount)
		{
			loadedGraphicsForDirection[direction] = data;
			bAreGraphicsLoadedForDirection[direction] = true;
		}
		else
		{
			loadedGraphicsData = data;
			bAreGraphicsLoaded = true;
		}
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
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY,
		int directionNumber = -1) = 0;

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
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int direction = -1) = 0;

	/**
	 *	Get cap height (for font resource only)
	 */
	virtual float GetCapHeight() { return 0.0f; }

	/**
	 *	Specifies what to do when we have been deallocated.
	 */
	virtual void Deallocate() = 0;

	/**
	 *	Set direction count.
	 */
	virtual void SetDirectionCount(int _directionCount) { directionCount = _directionCount; };

	/**
	 *	Load data for a single direction.
	 *	NOTE: This does not need to be defined for all subclasses.
	 */
	virtual void* LoadSingleDirection(unsigned int direction,
		AnimTextureAllocCallback allocCallback,
		AnimTextureDecodeCallback decodeCallback) { return nullptr; };
};

/**
 *	DCCReference is the IGraphicsReference implementation for DCC files.
 */
class DCCReference : public IGraphicsReference
{
private:
	union
	{
		bool bLoadedAnimation;
		bool bLoadedDirection[MAX_DIRECTIONS * 2]; // FIXME
	};

	unsigned int xOffsetForFrame[MAX_DIRECTIONS * 2][MAX_DCC_FRAMES];
	unsigned int yOffsetForFrame[MAX_DIRECTIONS * 2][MAX_DCC_FRAMES];
	unsigned int directionWidth[MAX_DIRECTIONS * 2];
	unsigned int directionHeight[MAX_DIRECTIONS * 2];
	char dccHandleName[MAX_D2PATH];
	DCCFile dccFile;
	bool bLoaded;

	void LoadDCCFile();
public:
	DCCReference(const char* fileName, GraphicsUsagePolicy usagePolicy);
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	virtual size_t GetNumberOfFrames();
	virtual void GetGraphicsData(void** pixels, int32_t frame,
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY,
		int directionNumber = -1);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int directionNumber = -1);
	virtual void Deallocate();
	virtual void* LoadSingleDirection(unsigned int direction,
		AnimTextureAllocCallback allocCallback,
		AnimTextureDecodeCallback decodeCallback) override;

};

/**
 *	DC6Reference is the IGraphicsReference implementation for DC6 files.
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
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY,
		int directionNumber = -1);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int directionNumber = -1);
	virtual void Deallocate();
};

/**
 *	DT1Reference is the IGraphicsReference implementation for DT1 files.
 */
class DT1Reference : public IGraphicsReference
{
private:
	handle dt1Handle;
	char filePath[MAX_D2PATH];
	bool bLoaded = false;
public:
	DT1Reference(const char* fileName, GraphicsUsagePolicy usagePolicy);
	// not needed?
	virtual size_t GetTotalSizeInBytes(int32_t frame);
	// not needed?
	virtual size_t GetNumberOfFrames();
	// not needed?
	virtual void GetGraphicsData(void** pixels, int32_t frame, 
		 uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY,
		int directionNumber = -1);
	// this one is needed
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	// this one is needed
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	// not needed?
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int directionNumber = -1);

	virtual void Deallocate();
	virtual bool AreGraphicsLoaded(int direction = -1) override { return bLoaded; }
};

/**
 *	FontReference is the IGraphicsReference implementation of fonts.
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
		uint32_t* width, uint32_t* height, int32_t* offsetX, int32_t* offsetY,
		int directionNumber = -1);
	virtual void GetGraphicsInfo(bool bAtlassing, int32_t start, int32_t end, uint32_t* width, uint32_t* height);
	virtual void IterateFrames(bool bAtlassing, int32_t start, int32_t end, AtlassingCallback callback);
	virtual void GetAtlasInfo(int32_t frame, uint32_t* x, uint32_t* y, uint32_t* totalWidth, uint32_t* totalHeight, int directionNumber = -1);
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
	cof_handle cofFiles[XXXMODE_MAX][WC_MAX];
	void LoadCOF(unsigned int mode, unsigned int hitclass);

	// The key is an encoded string, containing the following:
	// First 5 bits: the mode
	// 5 bits: weapon class
	// 5 bits: component
	// If you've been counting, this is 15 bits, or one bit short of a byte.
	// The first bit is therefore used to mark an "invalid" key.
	HashMap<char, IGraphicsReference*> m_cachedGraphicsReferences;

	void SetTokenName(const char* newTokenName);

public:
	/**
	 *	Default virtual destructor for virtual methods.
	 */
	~ITokenReference()
	{
		delete this;
	}
	virtual D2TokenType GetTokenType() = 0;
	virtual const char* GetTokenFolder() = 0;
	virtual void GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode) = 0;
	const char* GetTokenName() { return tokenName; }

	virtual bool HasComponentForMode(unsigned int component, unsigned int hitclass, unsigned int mode);
	inline const char* GetHitclassName(unsigned int hitclass);
	inline const char* GetModeName(unsigned int mode);
	inline const char* GetComponentName(unsigned int component);
	inline const char* GetTokenDataFolder();

	// Gotten from COF data..
	virtual inline const BYTE GetNumberOfFrames(int mode, int weaponClass);
	virtual inline const BYTE GetNumberOfDirections(int mode, int weaponClass);
	// End COF info

	virtual IGraphicsReference* GetTokenGraphic(unsigned int component, unsigned int hitclass,
		unsigned int mode, const char* armorClass);
};

/**
 *	Monster token reference
 */
class MonsterTokenReference : public ITokenReference
{
public:
	virtual D2TokenType GetTokenType() { return TOKEN_MONSTER; }
	virtual const char* GetTokenFolder() { return "MONSTERS"; }
	virtual void GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode);

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
	virtual void GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode);

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
	virtual void GetFallbackForComponentMode(bool& hasFallback, unsigned int& mode);

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
