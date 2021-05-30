#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	DS1 files.
 *	They are an arranged collection of tiles that have been created either by Blizzard or through Paul Siramy's win_ds1edit tool.
 */
#define MAX_DS1_OBJECTS			300
#define MAX_DS1_WALL_LAYERS		4
#define MAX_DS1_FLOOR_LAYERS	2
#define MAX_DS1_SHADOW_LAYERS	1
#define MAX_DS1_TAG_LAYERS		1
#define MAX_DS1_PATHS			100
#define MAX_DS1_LAYERS			14

enum DS1LayerTypes {
	Bad,
	Wall1,
	Wall2,
	Wall3,
	Wall4,
	Orientation1,
	Orientation2,
	Orientation3,
	Orientation4,
	Floor1,
	Floor2,
	Shadow,
	Tag,
};

typedef char DS1Path[MAX_D2PATH];
#pragma pack(push,enter_include)
#pragma pack(1)
struct DS1File
{
	struct DS1FileHeader {
		DWORD version;					// Not known.
		DWORD dwWidth;				// Number of tiles wide that this DS1 is.
		DWORD dwHeight;				// Number of tiles tall that this DS1 is.
		DWORD dwAct;				// Which palette this DS1 uses.
		DWORD dwExtraFilesCount;	// Vestigial.
	};

	struct DS1LayerHeader {
		DWORD dwWallLayers;			// Number of wall layers to use. Usually 4.
		DWORD dwFloorLayers;		// Number of floor layers to use. Usually 2.
		DWORD dwShadowLayers;		// Number of shadow layers to use. Always 1.
		DWORD dwTagLayersPresent;   //
		DWORD dwTagLayers;          //
		DWORD dwTotalLayersPresent;
		BYTE nLayerLayout[MAX_DS1_LAYERS];
	};

	struct DS1ObjectHeader {
		DWORD dwNumObjects;			// Number of objects to have in this DS1.
	};

	struct DS1PathElement {
		DWORD dwPath;
	};

	struct DS1Tag {
		DWORD num;
		BYTE flags;
	};

	struct DS1TagGroup {
		DWORD tileX;
		DWORD tileY;
		DWORD width;
		DWORD height;
		DWORD unknown;
	};

	DS1FileHeader fileHeader;
	DS1Path optionalFileList[8];		// Vestigial. This is most likely used by an internal tool to "know" which tiles to load, because it isn't used in-game.
									// It's pretty obvious that this is the case when everything up to Act4 tiles uses .tgl files, but Act5 uses .dt1 references.
	DS1LayerHeader layerHeader;
	BYTE* pUnknownLayer;
	DS1ObjectHeader objectHeader;
	DS1Object objects[MAX_DS1_OBJECTS];

	DS1Cell* pFloorCells;
	DS1Cell* pWallCells;
	DS1Cell* pShadowCells;
	DWORD dwNumTagGroups;
	DS1TagGroup* pTagGroups;
	DS1Tag* pTags;

	DS1File(const char* path);
	~DS1File();
};
#pragma pack(pop, enter_include)

namespace DS1
{
	handle LoadDS1(const char* path);
	void GetSize(handle ds1, int32_t& width, int32_t& height);
	DWORD GetObjectCount(handle ds1);
	DS1Cell* GetCellAt(handle ds1, uint32_t x, uint32_t y, const DS1CellType& type);
	const DS1Object& GetObject(handle ds1, int32_t which);
}