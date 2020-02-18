#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	DS1 files.
 *	They are an arranged collection of tiles that have been created either by Blizzard or through Paul Siramy's win_ds1edit tool.
 */
#define MAX_DS1_OBJECTS			300
#define MAX_DS1_WALL_LAYERS		4
#define MAX_DS1_FLOOR_LAYERS	4
#define MAX_DS1_SHADOW_LAYERS	1
#define MAX_DS1_PATHS			100
typedef char DS1Path[MAX_D2PATH];
#pragma pack(push,enter_include)
#pragma pack(1)
struct DS1File
{
	struct DS1FileHeader {
		DWORD dwX1;					// Not known.
		DWORD dwWidth;				// Number of tiles wide that this DS1 is.
		DWORD dwHeight;				// Number of tiles tall that this DS1 is.
		DWORD dwAct;				// Which palette this DS1 uses.
		DWORD bUnknownLayer;		// If true, there exists an unknown layer in the file (after wall, floor and shadow layers)
		DWORD dwExtraFilesCount;	// Vestigial.
	};

	struct DS1LayerHeader {
		struct DS1LoadedLayerHeader {
			DWORD dwWallLayers;			// Number of wall layers to use. Usually 4.
			DWORD dwFloorLayers;		// Number of floor layers to use. Usually 2.
		} loaded;
		DWORD dwShadowLayers;		// Number of shadow layers to use. Always 1.
	};

	struct DS1ObjectHeader {
		DWORD dwNumObjects;			// Number of objects to have in this DS1.
	};

	struct DS1Object {
		struct DS1ObjectInfo {
			DWORD dwType;
			DWORD dwId;
			DWORD dwX;
			DWORD dwY;
			DWORD dwUnused;
		} info;
	};

	struct DS1Path {
		DWORD dwPath;
	};

	struct DS1Cell {
		BYTE prop1;
		BYTE prop2;
		BYTE prop3;
		BYTE prop4;
		BYTE orientation; // only present on wall tiles
		DWORD tileIndex;
		BYTE flags;
	};

	DS1FileHeader fileHeader;
	DS1Path* optionalFileList;		// Vestigial. This is most likely used by an internal tool to "know" which tiles to load, because it isn't used in-game.
									// It's pretty obvious that this is the case when everything up to Act4 tiles uses .tgl files, but Act5 uses .dt1 references.
	DS1LayerHeader layerHeader;
	BYTE* pUnknownLayer;
	DS1ObjectHeader objectHeader;
	DS1Object objects[MAX_DS1_OBJECTS];

	// TODO: add NPC pathing

	DS1Cell* pFloorCells;
	DS1Cell* pWallCells;
	DS1Cell* pShadowCells;
};
#pragma pack(pop, enter_include)