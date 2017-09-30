#pragma once
#include "D2Common.hpp"

/*
 *	Much of this is derived from Mentor's code involving DRLG.
 *	Necrolis provided me with a link to a lot of this information.
 */

/////////////////////////////////////////////////
//
//	Forward Declarations

struct DRLGAct;
struct DRLGLevel;
struct DRLGVer;
struct DRLGRoomEx;
struct DRLGRoom;
struct DRLGRoomCoordList;
struct DRLGLinkData;
struct DRLGJungleDefs;
struct DRLGMisc;
struct DRLGLink;
struct D2Unit;

struct MiscBuild;
typedef int(__fastcall* MISC_BUILDER)(MiscBuild*);

#define COLLISION_ITEMS				(COLLIDE_BLOCK_UNITS|COLLIDE_DOOR)// 0x801
#define COLLISION_LOS_WALL			(COLLIDE_WALL|COLLIDE_DOOR)// 0x804
#define COLLISION_LOS				(COLLIDE_BLOCK_UNITS|COLLIDE_WALL|COLLIDE_DOOR)// 0x805
#define PLAYER_COLLISION_WW			(COLLIDE_OBJECT|COLLIDE_BLOCK_UNITS|COLLIDE_DOOR)// 0xC01
#define PLAYER_COLLISION_DEFAULT	(COLLIDE_BLOCK_UNITS|COLLIDE_BLOCK_LEAP|COLLIDE_OBJECT|COLLIDE_DOOR|COLLIDE_UNIT_RELATED)// 0x1C09
#define MONSTER_COLLISION_FLY		(COLLIDE_WALL|COLLIDE_DOOR|COLLIDE_UNIT_RELATED)// 0x1804
#define MONSTER_COLLISION_DOORS		(COLLIDE_BLOCK_UNITS|COLLIDE_OBJECT|COLLIDE_UNIT_RELATED|COLLIDE_PET)// 0x3401
#define MONSTER_COLLISION_DEFAULT	(COLLIDE_BLOCK_UNITS|COLLIDE_OBJECT|COLLIDE_DOOR|COLLIDE_UNIT_RELATED|COLLIDE_PET)// 0x3C01
#define SPAWN_ITEM_COLLISION		(COLLIDE_BLOCK_UNITS|COLLIDE_ITEM|COLLIDE_OBJECT|COLLIDE_DOOR|COLLIDE_UNIT_RELATED|COLLIDE_PET)// 0x3E01
#define SPAWN_UNIT_COLLISION		(COLLIDE_BLOCK_UNITS|COLLIDE_ALTERNATE_FLOOR|COLLIDE_MONSTER|COLLIDE_ITEM|COLLIDE_OBJECT|COLLIDE_DOOR|COLLIDE_UNIT_RELATED|COLLIDE_PET)// 0x3F11

#define DRLG_GRID_SIZE				8
#define MAX_LEVELS					255

/////////////////////////////////////////////////
//
//	Coordinate and Grid

#pragma pack(1)
struct DRLGCoordPoint
{
	int nX;
	int nY;
};


struct DRLGRectSize
{
	int nW;
	int nH;
};


struct DRLGCoordBox			//sizeof 0x10
{
	int nX;
	int nY;
	int nW;
	int nH;
};


struct DRLGCoordsEx
{
	union {
		DRLGCoordBox SBox;
		DRLGCoordBox TBox;

		struct {
			int sX;
			int sY;
			int sW;
			int sH;
			int tX;
			int tY;
			int tW;
			int tH;
		};
	};
};


struct DRLGGrid				// sizeof 0x14
{
	DWORD*	pGridFlags;		// +0x00	contains flags / indexes
	DWORD*	pGridSectors;	// +0x04	steps of width * row#, for fast access of 2dim array
	int		nWidth;				// +0x08
	int		nHeight;			// +0x0C
	DWORD	bInited;			// +0x10	set on creation
};

/////////////////////////////////////////////////
//
//	Automap
//	(code credit: Nefarius)

#define AUTOMAP_CACHE_CELLS 512

struct AutoMapCell
{
	int layer;               // +00 - automap layer from levels.txt
	short idx;               // +04 - frame to draw from cell
	short x;               // +06 - x pos of cell
	short y;               // +08 - y pos of cell
	short flags;            // +0A
	AutoMapCell* pColNext;      // +0C - next col
	AutoMapCell* pRowNext;      // +10 - next row
	AutoMapCell* pNextCache;   // +14
	AutoMapCell* pNextList;      // +18 - previous one allocated
};

typedef AutoMapCell* (__stdcall* AUTOMAP_UPDATER)(DRLGRoom* pRoom);					//6FACD660 reveal room;
typedef void(__stdcall* AUTOMAP_CHECKER)(int nLvlId, int nFilePicked, int nX, int nY);	//6FACD840 reveal level

struct D2CellFrame
{
	DWORD dwUnknown1;
	int w;
	int h;
	int x;
	int y;
	DWORD dwUnknown2;
	BYTE* pEndOfBlock;
	DWORD wBlock;
	BYTE bFrameBytes[1];
};


struct D2CellFile
{
	DWORD dwVersion;
	DWORD dwUnknown1;
	DWORD dwUnknown2;
	DWORD dwPadding;
	DWORD dwNumDirs;
	DWORD dwNumFrames;
	D2CellFrame* hFrames[1];
};


struct AutoMapData
{
	D2CellFile* hCell;   // +00
	DRLGCoordBox rect;
	int cells_w;
	int cells_h;
};


struct AutoMapCacheCell            // sizeof = 0x14
{
	int layer;                  // +00
	short idx;                  // +04
	short x;                  // +06
	short y;                  // +08
	short flags;               // +0A
	AutoMapCacheCell* pColNext;      // +0C
	AutoMapCacheCell* pRowNext;      // +10
};


struct AutoMapInfo
{
	DWORD first;
	DWORD last;
};

struct AutoMapShort
{
	int lvltype;            // +00
	int tiletype;            // +04
	char bCheck;            // +08
	char bFilterMin;         // +09
	char bFilterMax;         // +0A
	BYTE _0B;               // +0B
	int nCellIdx[4];         // +0C
	int nRand;               // +1C
};

struct AutoMapCache                         // sizeof = 0x2804
{
	AutoMapCacheCell cells[AUTOMAP_CACHE_CELLS];   // +0000
	AutoMapCache* pNext;                     // +2800
};

/////////////////////////////////////////////////
//
//	Tile Structures
//	(code credit: Necrolis)

#define MAX_TILESETS         32
#define TILELIBARAY_VERSION      7
#define TILE_VERSION_RAW      1
#define TILELIBRARY_MAXHASH     128
#define DRLGLINK_INVALID      -1

struct D2TileLibraryBlock	//sizeof 0x14
{
	short nXpos;            //+00
	short nYpos;            //+02
	short nUnk;             //+04
	BYTE  nGridX;           //+06
	BYTE  nGridY;		   //+07
	WORD  wFormat;          //+08
	int   nLength;          //+0A
	short nUnk2;            //+0E
	DWORD dwOffset;         //+10
};


enum eD2TileLibraryEntryFlags
{
	TILE_NOWALK = 0x01,
	TILE_NOLOSLIGHT = 0x02,
	TILE_NOJUMPTELEPORT = 0x04,
	TILE_NOPLAYERWALK = 0x08,
	TILE_NOLIGHT = 0x20,
};

enum GFXTILE_MATERIAL								//mostly found by Nefarius
{
	GFXTILE_MATERIAL_DEFAULTWALK = 0x0001,		// seems to be footstep_medium
	GFXTILE_MATERIAL_WATER = 0x0002,
	GFXTILE_MATERIAL_NOREVERB = 0x0004,		// near barrels / crates // also cathedral // seems to block reverb
	GFXTILE_MATERIAL_ISTONE = 0x0008,		// 2 indoor stone
	GFXTILE_MATERIAL_OSTONE = 0x0010,		// 3 outoor stone
	GFXTILE_MATERIAL_DIRT = 0x0020,		// 1
	GFXTILE_MATERIAL_SAND = 0x0040,		// 4
	GFXTILE_MATERIAL_WOOD = 0x0080,		// 6
	GFXTILE_MATERIAL_LAVA = 0x0100,
	GFXTILE_MATERIAL_SNOW = 0x0400,		// 5
};

struct D2TileLibraryEntry
{
	int nDirection;				// +00	'General' orientation 
	short nRoofHeight;			// +04	In pixels
	union {
		struct {
			BYTE nSound;	// +06	
			BYTE bAnimated;	// +07	
		};
		WORD fMaterialFlags;	// +06	
	};
	int nHeight;				// +08	in pixels, always power of 32, always a negative number 
	int nWidth;					// +0C	in pixels, always power of 32 
	DWORD dwUnk;				// +10	
	int nOrientation;			// +14	The 3 indexes that identify a Tile 
	int nIndex;					// +18	
	int nSubIndex;				// +1C	
	union {						// +20	frame index only for animated floor tiles
		int nRarity;				// 	
		int nFrame;					// 	
	};
	DWORD dwUnk2;				// +24	
	BYTE fSubTileFlags[25];	// +28	
	BYTE Unk[7];				// +41	
	union {						// +48	
		DWORD dwBlockOffset;		// 	
		D2TileLibraryBlock* pBlock;	// 	Pointer in file to Block Headers for this Tile 
	};
	int nBlockSize;				// +4C	Block Headers + Block Datas of this Tile 
	int nBlocks;				// +50	
	BYTE* pCompArray;			// +54	
	char* pLibraryName;			// +58	
	void* pLRUCache;			// +5C	
};

enum eDataEnvFlags
{
	ENV_USEOTHERTABLE = 1,
	ENV_CHANGELIGHT = 2,
	ENV_TILEHIDE = 4,
};

enum eMapTileFlags								// mostly done by Mentor
{
	MAPTILE_LAYER_ABOVE = 0x00000001,//	
	MAPTILE_EXIT = 0x00000002,//	warps, door exit, arches, gateways, near wp or on paths within level or to another level
	MAPTILE_ENCLOSED = 0x00000004,//	used by trees and some walls  //  seems to delimit an enclosure inside another area;
	MAPTILE_HIDDEN = 0x00000008,//	no gfx
	MAPTILE_10 = 0x00000010,//	
	MAPTILE_HASPRESETUNITS = 0x00000020,//	set for orientations 8-9; after spawning doors & towers etc.
	MAPTILE_UNWALKABLE = 0x00000040,//	
	MAPTILE_FILL_LOS = 0x00000080,//	all subtiles will get wall collision //  idle monsters, interaction issues
	MAPTILE_NO_HIDE = 0x00000100,//	ignores pops; doesn't hide roofs
	MAPTILE_REVEAL_HIDDEN = 0x00000200,//	used by A2 palace inner wall //  hidden wall displayed when near
	MAPTILE_PITCH_BLACK = 0x00000400,//	the lighting changed, RGB 0; tested for walls  //  no gfx
	MAPTILE_OBJECT_WALL = 0x00000800,//	wall tile made of props: barrels / crates / benches / tables (material flag 0x04)
	MAPTILE_1000 = 0x00001000,//	
	MAPTILE_FLOOR = 0x00002000,//	
	MAPTILE_WALL1 = 0x00004000,//	except for shadow orientation (13)
	MAPTILE_WALL2 = 0x00008000,//	except for shadow orientation (13) // also FLOOR2
	MAPTILE_WALL3 = 0x0000C000,//	except for shadow orientation (13)
	MAPTILE_WALL4 = 0x00010000,//	except for shadow orientation (13)
	MAPTILE_AUTOMAP_VALID = 0x00020000,//	it can be displayed //  show on automap
	MAPTILE_AUTOMAP_REVEALED = 0x00040000,//	already displayed //  skip automap
};

struct DRLGMapTileData					// sizeof 0x30
{
	int nWidth;							// +00	
	int nHeight;						// +04	
	int nXpos;							// +08	
	int nYpos;							// +0C	
	DRLGRoomCoordList* pCoordList;		// +10	
	DWORD fTileDataFlags;				// +14	
	D2TileLibraryEntry* pTile;			// +18	
	int eOrientation;					// +1C	
	DRLGMapTileData* pNext;				// +20	pNext // used by warps or tiles that spread across rooms
	DWORD dwEnvFlags;					// +24	1=use_other_table; 2=change_light , 4=maptile_hide // 6FAA63E0
	DWORD dwRGBA;						// +28	6FAA63E0
	DWORD dwTicks;						// +2C	6FAA63E0
};

struct D2TileLibraryHashRefStrc      //sizeof 0x8
{
	D2TileLibraryEntry* pTile;   //+00
	D2TileLibraryHashRefStrc* pPrev;//+04
};

struct D2TileLibraryHashNodeStrc      //sizeof 0x14
{
	int nIndex;                     //+00
	int nSubIndex;                  //+04
	int nOrientation;               //+08
	D2TileLibraryHashRefStrc* pRef;      //+0C
	D2TileLibraryHashNodeStrc* pPrev;   //+10
};

struct D2TileLibraryHashStrc   //sizeof 0x200
{
	D2TileLibraryHashNodeStrc* pNodes[TILELIBRARY_MAXHASH];   //+00
};

struct D2TileLibraryHeaderStrc      //sizeof 0x114
{
	DWORD dwVersion;               //+00
	DWORD fFlags;                  //+04
	char szLibraryName[260];         //+08
	int nTiles;                     //+10C
	union
	{
		DWORD dwTileStart;            //+110
		D2TileLibraryEntry* pFirst; //+110 - runtime only
	};
};

struct D2TileLibraryStrc         //sizeof 0x114 + {variable}
{
	D2TileLibraryHeaderStrc pHeader;      //+00
	D2TileLibraryEntry pTiles[1];      //+114
	D2TileLibraryBlock pTileBlocks[1];   //+114 + sizeof(pTiles[])
										 //BYTE pData[];                     //+114 + sizeof(pTiles[]) + sizeof(pTileBlocks[])
};


struct D2TileRecordStrc                     //sizeof 0x110
{
	char               szLibraryName[176];   // +00
	D2TileLibraryStrc*      pLibrary;         // +104
	D2TileLibraryHashStrc*   pHashBlock;         // +108
	D2TileRecordStrc*      pPrev;            // +10C
};

struct D2TileStrc         // = D2TileLibraryEntry
{
	D2TileStrc*            pNext;          // +00
	WORD               w1;            // +04
	WORD               fFlags;         // +06
	DWORD               dw2;           // +08
	DWORD               nTile;           // +0C
	DWORD               dw1;         // +10
	DWORD               nAct;           // +14
	DWORD               nLevel;         // +18
	DWORD               nTileType;      // +1C - 0xB = BASE_ROOF
	DWORD               dw3[7];           // +20
	D2TileRecordStrc*      pPerant;      // +3C
	DWORD               dw4[33];      // +40
};


/////////////////////////////////////////////////
//
//	Preset Data

enum Orientation
{
	ORNT_FLOOR,					// 0  FL 
	ORNT_WALL_LEFT,				// 1  WL 
	ORNT_WALL_RIGHT,			// 2  WR 
	ORNT_WALL_TOP_LOWER_RIGHT,	// 3  WTL 
	ORNT_WALL_TOP_LOWER_LEFT,	// 4  WTL 
	ORNT_WALL_TOP_RIGHT,		// 5  WTR 
	ORNT_WALL_BOTTOM_LEFT,		// 6  WBL 
	ORNT_WALL_BOTTOM_RIGHT,		// 7  WBR 
	ORNT_WALL_LEFT_DOOR,		// 8  WLD gate / arch / doorway
	ORNT_WALL_RIGHT_DOOR,		// 9  WRD gate / arch / doorway
	ORNT_WALL_LEFT_EXIT,		// 10 WLE warps / pops
	ORNT_WALL_RIGHT_EXIT,		// 11 WRE warps
	ORNT_CENTER_OBJECT,			// 12 CO object tiles (crates, barrels, tables etc.)
	ORNT_SHADOWS,				// 13 SH 
	ORNT_TREES,					// 14 TR 
	ORNT_ROOFS,					// 15 RF 
	ORNT_LEFT_DOWN,				// 16 LD lower wall
	ORNT_RIGHT_DOWN,			// 17 RD lower wall
	ORNT_FULL_DOWN,				// 18 FD lower wall center 
	ORNT_FULL_INVISIBLE,		// 19 FI (paired with 18)
};

enum DS1TileFlags										// researched by Mentor
{
	TILEFLAG_WALL = 0x00000001,		//  default valid wall
	TILEFLAG_FLOOR = 0x00000002,		// (0x2000: MAPTILE_FLOOR) default valid floor
	TILEFLAG_LOS = 0x00000004,		//  code-generated; added on edges
	TILEFLAG_ENCLOSE = 0x00000008,		// (0x04: MAPTILE_ENCLOSED) seems to delimit an enclosure inside another area;
	TILEFLAG_EXIT = 0x00000010,		//  arch or doorway or gateway in a wall
	TILEFLAG_20 = 0x00000020,		//  
	TILEFLAG_LAYER_BELOW = 0x00000040,		//  only floor
	TILEFLAG_LAYER_ABOVE = 0x00000080,		// (0x01: MAPTILE_LAYER_ABOVE) wall & floor
	TILEFLAG_TILESUBINDEX = 0x00000100,		//  SHR R32, 0x8 // AND R32, 0xFF
	//TILEFLAG_TILESUBINDEX			=	0x00000200,		//  
	//TILEFLAG_TILESUBINDEX			=	0x00000400,		//  
	//TILEFLAG_TILESUBINDEX			=	0x00000800,		//  
	//TILEFLAG_TILESUBINDEX			=	0x00001000,		//  
	//TILEFLAG_TILESUBINDEX			=	0x00002000,		//  
	//TILEFLAG_TILESUBINDEX			=	0x00004000,		//  
	//TILEFLAG_TILESUBINDEX			=	0x00008000,		//  AND 0x3F0FF00 is used to check if index 0 subindex 0
	TILEFLAG_FILL_LOS = 0x00010000,		// (0x80: MAPTILE_FILL_LOS) all subtiles will get wall collision;
	TILEFLAG_UNWALKABLE = 0x00020000,		// (0x40: MAPTILE_UNWALKABLE)
	TILEFLAG_OVERLAPPED_LAYER2 = 0x00040000,		//  code-generated; set when at least Floor2 or Wall2  // (nLayer - 1) << 0x12
	TILEFLAG_OVERLAPPED_LAYER3 = 0x00080000,		//  code-generated; set when has more walls, incl' of other orientations than usual; obj, shd, tree, roof, lower
	TILEFLAG_OVERLAPPED_LAYER4 = 0x000C0000,		//  code-generated; set when has more walls, incl' of other orientations than usual; obj, shd, tree, roof, lower
	TILEFLAG_TILEINDEX = 0x00100000,		//  SHR R32, 0x14  //  AND R32, 0x3F
	//TILEFLAG_TILEINDEX				=	0x00200000,		//  
	//TILEFLAG_TILEINDEX				=	0x00400000,		//  
	//TILEFLAG_TILEINDEX				=	0x00800000,		//  
	//TILEFLAG_TILEINDEX				=	0x01000000,		//  
	//TILEFLAG_TILEINDEX				=	0x02000000,		//  
	TILEFLAG_REVEAL_HIDDEN = 0x04000000,		// (0x20C: MAPTILE_ENCLOSED | MAPTILE_HIDDEN | MAPTILE_REVEAL_HIDDEN) looks like an upper wall brought to a layer in front
	TILEFLAG_SHADOW = 0x08000000,		//  this layer is a shadow layer
	TILEFLAG_LINKAGE = 0x10000000,		// (0x102: MAPTILE_EXIT | MAPTILE_NO_HIDE) near wp, lvl links, paths // will never get hidden
	TILEFLAG_OBJECT_WALL = 0x20000000,		// (0x800: MAPTILE_OBJECT_WALL) wall tiles with props; may be block reverb / other sounds (crates, barrels, tables etc.)
	TILEFLAG_40000000 = 0x40000000,		//  
	TILEFLAG_HIDDEN = 0x80000000,		// (0x08: MAPTILE_HIDDEN)
};

struct DRLGRoomGridArgs        			// sizeof 0x3C used for lvlsub
{
	DRLGRoomEx* pRoomEx;				// +0x00
	DRLGGrid* pOutOrientationGrid[4];	// +0x04
	DRLGGrid* pOutWallGrid[4];			// +0x14
	DRLGGrid* pOutFloorGrid;			// +0x24
	DRLGGrid* pOutPathGrid;				// +0x28
	int nWallLayers;					// +0x2C
	union {								//+ 0x30
		int nSubWaypoint;
		int nSubshrineNo;
		int nSubtypeNo;
	};
	union {								// +0x34
		int nNULL;
		int nNULL2;
		int nSubthemeNo;
	};
	union {								// +0x38
		DWORD WPFlags;					// fRoomExFlag >> 16 & 3 = WPflags
		DWORD ShrineFlags;				// fRoomExFlag >> 12 & 15   = ShrineFlags
		DWORD fSubSubPicked;			// mask; 2^row of the subtype where prob for subtheme > rand 0-99
	};
};


enum eMapAIAction
{
	AIACTION_MODE_NU = 1,
	AIACTION_MODE_S1 = 4,
	AIACTION_MODE_S2 = 5,
};


struct DRLGFilePathPosition	// sizeof 0x0C
{
	int eMapAIAction;		// +00
	int nXpos;				// +04
	int nYpos;				// +08
};


struct DRLGMapAi						// sizeof 0x08
{
	int nPathNodes;						// +00
	DRLGFilePathPosition* pPosition;	// +04
};


enum PUFlags
{
	PUFLAG_NOSPAWN = 0x01,
};


struct DRLGPresetUnit		// sizeof 0x20
{
	int eType;				// +00
	int eClass;				// +04
	int eMode;				// +08
	int nXpos;				// +0C
	int nYpos;				// +10
	DWORD fPUFlags;		// +14
	DRLGMapAi* pMapAi;		// +18
	DRLGPresetUnit* pNext;	// +1C
};


struct DRLGSubGroup			// sizeof 0x18
{
	int nOffsetX;			// +00			start x cell in file
	int nOffsetY;			// +04			start y cell in file
	int nSubRectWidth;		// +08			size x of SubRect's in this group
	int nSubRectHeight;		// +0C			size y of SubRect's in this group
	DWORD dwUnk1;			// +10			looks like a seed number
	int nSubRects;			// +14			count of SubRects - 1; for type #1, this is always 0
};


enum SubType
{
	SUB_NONE = 0,
	SUB_SINGLE = 1,	// tags are set individually for substitution groups and the count includes all of them
	SUB_MUTLI = 2,	// tags are set for a group of rects and the count includes the groups only
};


struct DRLGFile				// sizeof 0x5C
{
	int eSubType;			// +00	substitution type
	void* pDS1;				// +04
	DWORD dw08;				// +08
	int w;					// +0C
	int h;					// +10
	int nWalls;				// +14
	int nFloors;			// +18
	void* pOrntLayer[4];	// +1C
	void* pWallLayer[4];	// +2C
	void* pFloorLayer[2];	// +3C
	void* pShadowLayer;		// +44
	void* pSubGroupTags;	// +48
	int nSubstGroups;		// +4C
	DRLGSubGroup* pSubGroup;// +50
	DRLGPresetUnit* pFirst;	// +54
	DWORD dw58;				// +58
};


struct DRLGMap
{
	int nLevelPrestNo;				// +00
	int nFilePicked;				// +04
	D2LvlPrestTxt* pLvlPrest;		// +08
	DRLGFile* pFile;				// +0C
	DRLGCoordBox MapBox;			// +10
	DWORD bHasInfo;					// +20
	DRLGGrid MapMazeGrid;			// +24	not really sure what this one does
	DRLGPresetUnit* pFirst;			// +38
	DWORD bInit;					// +3C		
	int nPops;						// +40	6F647760
	void *pPopsIndex;				// +44	array, sizeof nPops*4   ||   initially holds the index of the pop tiles, then index / 4 -1	6F647760
	void *pPopsSubIndex;			// +48	array, sizeof nPops*4   ||   holds the subindex of the pop tiles	6F647760
	void *pPopsTicks;				// +4C	array, sizeof nPops*4   ||   ticks for each pop	6F647760
	void *pPopsCoords;				// +50	array, sizeof nPops*16  ||   each 'line': 00=StartX 04=StartY 08=Width 0C=Height	6F647760
	DRLGMap* pNext;					// +54		
};


/////////////////////////////////////////////////
//
//	Room Data

struct DRLGDelete
{
	int eType;
	DWORD dwGUID;
	DRLGDelete* pNext;
};

// credits to Nefarius for the collision info
enum CollisionMask
{
	COLLIDE_BLOCK_UNITS = 0x0001,// black space' in arcane sanctuary, cliff walls etc
	COLLIDE_BLOCK_MISSILE = 0x0002,// tile based obstacles you can't shoot over
	COLLIDE_WALL = 0x0004,// assumed to be walls, most things check this
	COLLIDE_BLOCK_LEAP = 0x0008,// 
	COLLIDE_ALTERNATE_FLOOR = 0x0010,// some floors have this set, others don't
	COLLIDE_BLANK = 0x0020,// returned if the subtile is invalid
	COLLIDE_MISSILE = 0x0040,// 
	COLLIDE_PLAYER = 0x0080,// 
	COLLIDE_MONSTER = 0x0100,// 
	COLLIDE_ITEM = 0x0200,// 
	COLLIDE_OBJECT = 0x0400,// 
	COLLIDE_DOOR = 0x0800,// 
	COLLIDE_UNIT_RELATED = 0x1000,// set for units sometimes, but not always
	COLLIDE_PET = 0x2000,// 
	COLLIDE_4000 = 0x4000,// 
	COLLIDE_CORPSE = 0x8000,// also used by portals, but dead monsters are mask 0x8000
};


struct RoomCollisionGrid	// sizeof 0x24
{
	DRLGCoordsEx CollBox;	// +0x00
	WORD** ppCollisionMask;	// +0x20
};

struct DRLGTileGrid				// sizeof 0x18
{
	DRLGMapTileData* pWalls;	// +00
	int nWalls;					// +04
	DRLGMapTileData* pFloors;	// +08
	int nFloors;				// +0C
	DRLGMapTileData* pShadows;	// +10
	int nShadows;				// +14
};

struct DRLGRoom						// sizeof 0x80
{
	DRLGCoordsEx RoomCoords;		// +00
	DRLGTileGrid* pTileGrid;		// +20	
	DRLGRoom** ppRoomsNear;			// +24	pointer to list of adjacent rooms
	int nRoomsNear;					// +28	max 9, includes itself
	D2Unit* pUnitFirst;				// +2C	first unit in room
	D2Unit* pUnitUpdate;			// +30	pUnit to be updated
	RoomCollisionGrid* pCollGrid;	// +34	collision map for the room's subtiles
	DRLGRoomEx* pRoomEx;			// +38	
	D2Seed RoomSeed;				// +3C
	DRLGDelete* pDelete;			// +44	
	DWORD fRoomFlags;				// +48	1=init, 2=active, 4=update units (also automap_revealed); flag0x1 taken from pRoomEx+0x18.
	void** ppClients;				// +4C	list of clients in the current room
	int nClients;					// +50	count of clients in the current room
	int nMaxClients;				// +54	
	int nInactiveFrames;			// +58	inactive period; max 0x0B
	int nPlayers;					// +5C	includes summons / hirelings
	DWORD _60;						// +60	
	BYTE nKilledMonCount;			// +64	max 4
	BYTE _65[3];					// +65	pad, most probably
	DWORD dwMonGUID[4];				// +68	killed mon GUID
	DRLGAct* pAct;					// +78	
	DRLGRoom* pNext;				// +7C
};


/////////////////////////////////////////////////
//
//	RoomEx Data

struct DRLGRoomCoordList		// sizeof 0x30
{
	DRLGCoordsEx RoomCoords;	// +0x00
	DWORD bNode;				// +0x20 BOOL bFloorHiddenUnwalkable 	hidden flag or blank.dt1, index 30, subindex 0 (coll 5)
	DWORD bRoomActive;			// +0x24
	int nIdx;					// +0x28 fCoordFlag &0xFFFFFFF	
	DRLGRoomCoordList* pNext;	// +0x2C
};


struct DRLGRoomCoords	// sizeof 0x34	
{
	DWORD Flags;						// +00	1=non-logicals; 2=logicals
	int nLists;							// +04	
	DRLGGrid pOtherCoordsGrid;			// +08	list to pointers to other coords lists
	DRLGGrid pIndicesGrid;				// +1C	this is forms a 1 array pIndexList[pIndices[nYpos] + nXpos];
	DRLGRoomCoordList* pCoordList;		// +30	this is an array
};


struct DRLGRoomWarpTiles				// sizeof 0x18
{
	DRLGRoomEx* pDestination;			// +0x00
	D2LvlWarpTxt* pWarpRecord;			// +0x04
	DWORD bEnabled;						// +0x08
	DRLGMapTileData* pHidden;			// +0x0C wall tiles   non-hidden / floors 4-7 hidden
	DRLGMapTileData* pRevealed;			// +0x10 floors 0-7  non hidden / floors 0-3 hidden
	DRLGRoomWarpTiles* pNext;			// +0x14
};


/*
warptype		LIT0										LIT1
hidden		C	no floor, no wall							floor index(warpsubindex), subindex 4-7					warp with no gfx
10	no floor, no wall							floor index(warpsubindex), subindex 0-3					warp with no gfx
revealed	C	no wall										wall index(warpindex), subindex(warpsubindex+nFloors)	warp with a wall gfx
10	wall index(warpindex), subindex 0-3 / 4-7	wall index(warpindex), subindex(warpsubindex+nFloors)	warp with a wall gfx
*/
enum eRoomExStatus
{
	STATUS_CLIENT_IN_ROOM = 0x00,		//client is in this very room
	STATUS_CLIENT_IN_ADJ = 0x01,		//client is in one of the adjacent rooms
	STATUS_OUT_OF_SIGHT = 0x02,		//outside client's range of adjacent (visible) rooms
	STATUS_UNTILE = 0x03,		//blizz term :| pRoom will be freed
	STATUS_INITED_UNTILED = 0x04,		//pRoom has been freed or pRoomEx just created
};


struct DRLGAnimTileGrid					// sizeof 0x14
{
	DRLGMapTileData** ppMapTileData;	// sizeof dword * nAdded; countof nAdded // flag hidden added to old frame and removed for new frame
	int nFrames;						// how many hashes added to pRoomEx; in fact, a frame is another tile of a different sequence
	int nCurrentFrame;					// nCurrentFrame = (nAnimSpeed + nCurrentFrame) % (nFrames >> 8) (D2Common.#10105 & #10104)
	int nAnimationSpeed;				// taken from LvlPrestTxt (yeah, that's right! ;) ), otherwise 80
	DRLGAnimTileGrid* pNext;
};


struct DRLGMapTileLink				// sizeof 0x0C
{
	DWORD bFloor;					// +00
	DRLGMapTileData *pFirst;		// +04
	DRLGMapTileLink* pNext;			// +08
};


struct DRLGMapTileGridEx				//	sizeof 0x2C
{
	DRLGMapTileLink* pMapTileLink;		//	+00		a list of tile data for linked tiles; warps use this among others
	DRLGAnimTileGrid* pAnimTileGrid;	//	+04		structure holding info about the animated tiles; lava floors
	int nWalls;							//	+08	increased at tile load	copied to 0x18  after load
	int nFloors;						//	+0C	increased at tile load	copied to 0x20  after load
	int nShadows;						//	+10	increased at tile load	copied to 0x28  after load
	DRLGTileGrid pRoomTileGrid;			//	+14	this gets copied to pRoom
};


struct DRLGOutdoorRoom			// sizeof 0x70
{
	DRLGGrid pOrientationGrid;	// +00	OrientationGrid
	DRLGGrid pWallGrid;			// +14	WallGrid
	DRLGGrid pFloorGrid;		// +28	FloorGrid
	DRLGGrid pPathGrid;			// +3C	DirtPath Grid
	DRLGVer* pVer;				// +50	
	DWORD fOutMapFlags;			// +54	passed from pLevel+38
	DWORD fOutUnusedFlags;		// +58	passed from pLevel+38
	DWORD _5C;					// +5C	
	DWORD _60;					// +60	
	int nSubtypeNo;				// +64	acc to lvldefs record // this is the lvlsub index
	int nSubthemeNo;			// +68	acc to lvldefs record // this is the column 0-4 in lvlsub
	DWORD dwSubRowsPicked;		// +6C	mask; 2^row of the subtype where prob for subtheme > rand 0-99
};


struct TombStonesCoords				//sizeof 0x30
{
	DRLGCoordPoint StonePoint[6];
};


struct DRLGPresetRoom				// sizeof  0xF8
{
	int nLevelPrestId;				// +00	
	int nFilePicked;				// +04	
	DRLGMap* pMap;					// +08	
	DWORD fFlags;					// +0C	1=single_room, 2=hasmap(ds1) *0x1 = bOutdoors
	DRLGGrid pWallGrid[4];			// +10	+24 +38 +4C; walls 2-4 get OR'ed with 0x40000/0x80000/0xC0000
	DRLGGrid pOrientationGrid[4];	// +60	+74 +88 +9C
	DRLGGrid pFloorGrid[2];			// +B0	+C4
	DRLGGrid pShadowGrid;			// +D8	
	DRLGGrid* pMapMazeGrid;			// +EC	pointer to DRLGMap +0x24 // not 100% sure, though
	TombStonesCoords* pTombCoords;	// +F0	sizeof 0x30 // 6 groups x 2 DWORDS // X and Y of wall tiles of index 10, subindex 23-27
	int nTombStones;				// +F4	count of said tiles // used only by graveyard
};


enum ePresetType
{
	ROOMTYPE_OUT = 0x1,
	ROOMTYPE_MAZE_PRESET = 0x2,
};

enum eRoomAltFlags
{
	ROOMALTFLAG_HADROOM = 0x1,	//used for pLevel->pRoomBools and for pRoom+0x48 (inited)
};


enum eDRLGRoomExFlags
{
	ROOMEXFLAG_INACTIVE = 0x00000002,
	ROOMEXFLAG_HAS_WARP_0 = 0x00000010,
	ROOMEXFLAG_HAS_WARP_1 = 0x00000020,
	ROOMEXFLAG_HAS_WARP_2 = 0x00000040,
	ROOMEXFLAG_HAS_WARP_3 = 0x00000080,
	ROOMEXFLAG_HAS_WARP_4 = 0x00000100,
	ROOMEXFLAG_HAS_WARP_5 = 0x00000200,
	ROOMEXFLAG_HAS_WARP_6 = 0x00000400,
	ROOMEXFLAG_HAS_WARP_7 = 0x00000800,
	ROOMEXFLAG_SUBSHRINE_ROW1 = 0x00001000,
	ROOMEXFLAG_SUBSHRINE_ROW2 = 0x00002000,
	ROOMEXFLAG_SUBSHRINE_ROW3 = 0x00004000,
	ROOMEXFLAG_SUBSHRINE_ROW4 = 0x00008000,
	ROOMEXFLAG_HAS_WAYPOINT = 0x00010000,	//outdoors with subtheme and subwaypoint
	ROOMEXFLAG_HAS_WAYPOINT_SMALL = 0x00020000,	//waypoint small
	ROOMEXFLAG_AUTOMAP_REVEAL = 0x00040000,
	ROOMEXFLAG_NO_LOS_DRAW = 0x00080000,
	ROOMEXFLAG_HAS_ROOM = 0x00100000,	//an active pRoom structure is attached to this RoomEx
	ROOMEXFLAG_ROOM_FREED_SRV = 0x00200000,	//set after freeing the pRoom
	ROOMEXFLAG_HASPORTAL = 0x00400000,	//prevents room deletion
	ROOMEXFLAG_POPULATION_ZERO = 0x00800000,	//set for towns, also set if a ds1 has populate=0 // nospawn region
	ROOMEXFLAG_TILELIB_LOADED = 0x01000000,
	ROOMEXFLAG_PRESET_UNITS_ADDED = 0x02000000,	//refers to DRLGMap or DRLGFile; adds the hardcoded preset units
	ROOMEXFLAG_PRESET_UNITS_SPAWNED = 0x04000000,	//set after RoomEx preset units have been spawned / prevents respawning them on room reactivation
	ROOMEXFLAG_ANIMATED_FLOOR = 0x08000000,	//animated floors (river of flame, hell act5)
};


#define ROOMEXMASK_CHECK_WARP		(ROOMEXFLAG_HAS_WARP_0|ROOMEXFLAG_HAS_WARP_1|ROOMEXFLAG_HAS_WARP_2|ROOMEXFLAG_HAS_WARP_3|ROOMEXFLAG_HAS_WARP_4|ROOMEXFLAG_HAS_WARP_5|ROOMEXFLAG_HAS_WARP_6|ROOMEXFLAG_HAS_WARP_7)
#define ROOMEXMASK_CHECK_WP			(ROOMEXFLAG_HAS_WAYPOINT|ROOMEXFLAG_HAS_WAYPOINT_SMALL)//combined for presets;
#define ROOMEXMASK_CHECK_SHRINE		(ROOMEXFLAG_SUBSHRINE_ROW1|ROOMEXFLAG_SUBSHRINE_ROW2|ROOMEXFLAG_SUBSHRINE_ROW3|ROOMEXFLAG_SUBSHRINE_ROW4)


struct DRLGRoomEx						//sizeof 0xEC
{
	DRLGLevel* pLevel;					//+0x00	
	DRLGCoordBox RoomExBox;				//+0x04
	DWORD fRoomExFlags;					//+0x14	
	DWORD fRoomAltFlags;				//+0x18	1=had_room; used for pLevel->pRoomBools and for pRoom+0x48 (inited)
	int ePresetType;					//+0x1C	1=out_room, 2=maze_preset
	union {
		DRLGPresetRoom* pPreset;		//+0x20	pointer to info from a ds1 in this roomex
		DRLGOutdoorRoom* pOutdoor;
	};
	DWORD dwDT1Mask;					//+0x24	DT1Mask of all subtype ds1's for out_room
	D2TileLibraryHashStrc* pTiles[32];	//+0x28	pointers to tile libraries loaded acc. to the DT1Mask
	DRLGMapTileGridEx* pTileGridEx;		//+0xA8	similar to pRoom+0x20 // sizeof 0x2c
	BYTE eRoomStatus;					//+0xAC	
	BYTE _AD;							//+0xAD	pad
	short nAdj0;						//+0xAE	related to status of adj rooms
	short nAdj1;						//+0xB0	related to status of adj rooms
	short nAdj2;						//+0xB2	related to status of adj rooms
	short nAdj3;						//+0xB4	related to status of adj rooms
	short nAdj4;						//+0xB6	related to status of adj rooms
	DRLGRoomEx* pStatusNext;			//+0xB8	next room of same status or pointer to pMiscdata+unk
	DRLGRoomEx* pStatusPrev;			//+0xBC	next room of same status or pointer to pMiscdata+unk
	DRLGRoomEx** ppRoomsNear;			//+0xC0	Blizz dubbed them "pptVisibleRooms"
	int nRoomsNear;						//+0xC4	number of adjacent roomex's | eg: 4=corner, 6=edge, 9=inside
	DRLGRoomWarpTiles* pRoomWarpTiles;	//+0xC8	level linking tiles (ie warps)
	DRLGPresetUnit* pPresetUnits;		//+0xCC	units placed on a ds1 in this roomex; warp units and waypoints are always here
	DRLGLinkData* pLinkData;			//+0xD0	info on how it's connected to adjacent rooms
	D2Seed RoomExSeed;					//+0xD4
	DWORD dwInitSeed;					//+0xDC	
	DRLGRoomCoords* pRoomCoords;		//+0xE0	
	DRLGRoom* pRoom;					//+0xE4	
	DRLGRoomEx* pNext;					//+0xE8	

};


// credits to Nefarius

struct LvlLinkTable					// sizeof 0x10
{
	MISC_BUILDER pfnMiscBuilder;	// +00
	int nLvlId;						// +04
	int nIndex;						// +08
	int nUnknown;					// +0C
};
/* nIndex corresponds with the index in MiscBuild box and LinkByDir arrays*/

struct OutLvlPlaceTable		// sizeof 0x18
{
	int nLvlNo;				// +00
	int nLinkLvlNo[2];		// +04
	int nLinkLvlDir[2];		// +0C
	DWORD dwMask;			// +14
};
/*
nLinkLvlDir[0] corresponds with the nLvlDir[i] entry
inside the array and nLinkLvlDir[1] corresponds with nLvlDir[i+1]
inside pBuild->nLvlDir*/



struct MiscBuild			// sizeof 0x1F4
{
	D2Seed BuildSeed;			// +000
	DRLGCoordBox BuildBox[15];	// +008
	LvlLinkTable* pLinks;		// +0F8
	int nLvlDir[15];			// +0FC 0-3
	int nLvlBaseDir[15];		// +138 0-3
	int nLvlInc[15];			// +174 0-1
	int nLvlBaseInc[15];		// +1B0 0-1
	int nIndex;					// +1EC
	int nLvlId;					// +1F0

								// nIndexTable holds which levels are linked in what direction
								// the boxes are for 15 connected outdoor levels
								// they recieve their offsets and size ( size is from leveldefs )
								// current index is the current index in pLinks
								// current lvlid is the id of the level currently being worked on
};

/////////////////////////////////////////////////////
//
//	Level Info

enum DRLGDIRS
{
	DIRECTION_SOUTHWEST = 0,
	DIRECTION_NORTHWEST = 1,
	DIRECTION_SOUTHEAST = 2,
	DIRECTION_NORTHEAST = 3,
	DIRECTION_COUNT = 4,
	DIRECTION_INVALID = -1,
};


enum DRLGDIRSALT
{
	ALTDIR_WEST = 0,
	ALTDIR_NORTH = 1,
	ALTDIR_EAST = 2,
	ALTDIR_SOUTH = 3,
	ALTDIR_NORTHWEST = 4,
	ALTDIR_NORTHEAST = 5,
	ALTDIR_SOUTHEAST = 6,
	ALTDIR_SOUTHWEST = 7,
	ALTDIR_COUNT = 8,
	ALTDIR_INVALID = -1,
};


enum DRLGMAZEFLAGS2
{
	DRLGMAZE_LINKED = 0x1,
	DRLGMAZE_COMPLETE = 0x2,
};


enum DRLGDIRFLAGS
{
	DRLGDIR_SOUTHWEST = 0x1,
	DRLGDIR_SOUTHEAST = 0x2,
	DRLGDIR_NORTHEAST = 0x4,
	DRLGDIR_NORTHWEST = 0x8,
};


struct DRLGLinkData				// sizeof 0x18
{
	union {						// +00
		DRLGLevel* pLevel;
		DRLGRoomEx* pRoomEx;
	};
	char eDir;					// +04
	BYTE bPad[3];				// +05
	DWORD bVerFlag;				// +08 BOOL bPresetLinker
	DWORD bWarp;				// +0C
	DRLGCoordBox* pCoordBox;	// +10 taken from pLevel+0x20 or pRoomEx+0x04
	DRLGLinkData* pPrev;		// +14
};


struct LvlPathData				// sizeof 0x14
{
	int nX;						// +00
	int nY;						// +04
	BYTE eDir;					// +08
	BYTE _09;					// +09
	BYTE _0A;					// +0A
	BYTE _0B;					// +0B
	DWORD _0C;					// +0C
	DWORD _10;					// +10
};


enum eDRLGVerFlag
{
	VERFLAG_EDGE = 1,	//in corner
	VERFLAG_LINK = 2,
};


struct DRLGVer				// sizeof = 0x14
{
	int nX;					// +00
	int nY;					// +04
	char eDir;				// +08
	DWORD dwVerFlag;		// +0C
	DRLGVer* pPrev;			// +10
};


enum DRLGGRID_INDEX
{
	DRLGGRID_LVLPREST = 0,
	DRLGGRID_ROOMFLAG = 1,
	DRLGGRID_OUTLVLFLAG = 2,
	DRLGGRID_UNUSED = 3,
	DRLGGRID_COUNT = 4,
};


enum OUTDOORFLAGS
{
	OUTDOOR_FLAG1 = 0x00000001,
	OUTDOOR_FLAG2 = 0x00000002,
	OUTDOOR_BRIDGE = 0x00000004,
	OUTDOOR_RIVER_OTHER = 0x00000008,
	OUTDOOR_RIVER = 0x00000010,
	OUTDOOR_CLIFFS = 0x00000020,
	OUTDOOR_OUT_CAVES = 0x00000040,
	OUTDOOR_SOUTHWEST = 0x00000080,
	OUTDOOR_NORTHWEST = 0x00000100,
	OUTDOOR_SOUTHEAST = 0x00000200,
	OUTDOOR_NORTHEAST = 0x00000400,
};


enum OUTLVLFLAGS
{
	OUTLVL_BORDER = 0x00000001,
	OUTLVL_CLIFF_BORDER = 0x00000002,
	OUTLVL_PATH_LINK = 0x00000080,	// dirt path cell
	OUTLVL_BLANK_CELL = 0x00000100,	// no ds1, no pRoomEx
	OUTLVL_HAS_PRESET = 0x00000200,	// lvlsub or any other theme ds1
	OUTLVL_LVL_LINK = 0x00000400,	// cell on level transition
	OUTLVL_WAYPOINT_SET = 0x00000800,
	OUTLVL_SHRINE_SET = 0x00001000,
	OUTLVL_PREST_FILE2 = 0x00010000,	// SHL 0x10
	OUTLVL_PREST_FILE3 = 0x00020000,
	OUTLVL_PREST_FILE4 = 0x00030000,	// for A1 borders: other; dirt path continuous
	OUTLVL_PREST_FILE5 = 0x00040000,	// for A1 borders: other_end; dirt path end
};


#define OUTLVL_SKIP_NODE OUTLVL_BORDER|OUTLVL_PATH_LINK|OUTLVL_BLANK_CELL|OUTLVL_HAS_PRESET|OUTLVL_WAYPOINT_SET|OUTLVL_SHRINE_SET


#define MAX_PATHS 6


struct DRLGOutdoorInfo				// sizeof 0x268
{
	DWORD fOUTDOORFLAGS;			//+000
	DRLGGrid pGrid[4];				//+004
	int nW;							//+054
	int nH;							//+058
	int wGrid;						//+05C
	int hGrid;						//+060
	DRLGVer* pVer;					//+064
	DWORD DW1[MAX_PATHS];			//+068
	LvlPathData start[MAX_PATHS];	//+080
	LvlPathData end[MAX_PATHS];		//+0F8
	DWORD DW3[60];					//+170
	int nPathCount;					//+260
	DRLGLinkData* pData;			//+264
};


struct DRLGPresetInfo	//sizeof 0x08
{
	DRLGMap* pMap;		// +00
	int nFile;			// +04
};


struct DRLGMazeInfo// sizeof 0x1C
{
	int nLvlNo;			// +00
	int nRooms[3];		// +04
	int nWidth;			// +10
	int nHeight;		// +14
	int nMerge;			// +18
};

//////////////////////////////////////////////////////////////////////////
//
//	DRLGLevel

enum eLevelFlags
{
	DRLGLVL_AUTOMAP_REVEAL = 0x10,
};

enum eSpTiles				//index	subindex   orientation
{
	SP_TOWNENTRY1 = 0,		//30		0          10
	SP_TOWNENTRY2 = 5,		//31		0          10
	SP_CORPSETILE = 10,		//32		0          10
	SP_TP_MAPENTRY = 11,	//33, 34	0, 11      10, 10
	SP_TILE34 = 12,			//34		0          10
	SP_TILE_WP = 13,		//only when searching for special rooms
};

struct DRLGLevelInfo	// sizeof 0x0C
{
	int nSpecialTilePosX;	// +00
	int nSpecialTilePosY;	// +04
	int eSpecialTileIndex;	// +08
};

struct DRLGBuild			// sizeof 0x10
{
	int nLvlPresetId;		// +00
	int nFiles;				// +04
	int nFilePicked;		// +08
	DRLGBuild* pNext;		// +0C
};

enum eDRLGType
{
	DRLGTYPE_MAZE = 1,
	DRLGTYPE_PRESET = 2,
	DRLGTYPE_OUTDOORS = 3,
	DRLGTYPE_COUNT = 4,
};

struct DRLGLevel				// sizeof 0x230
{
	DRLGMisc* pMisc;			//+000	
	int nLevel;					//+004	
	int nLevelType;				//+008	
	int eDRLGType;				//+00C	
	DWORD fLevelFlags;			//+010	10=automap_reveal
	D2Seed LevelSeed;			//+014	D2SeedStrc* pSeed;
	DWORD dwStartSeed;			//+01C	
	DRLGCoordBox LvlBox;		//+020	level offset in tiles
	DRLGRoomEx* pFirstRoomEx;	//+030	
	int nRooms;					//+034	count of RoomEx's on this level; also used by MonRegion
	union {						//+038	 
		DRLGPresetInfo* pPreset;	//	
		DRLGOutdoorInfo* pOutdoors;	//	
		D2LvlMazeTxt* pMazeInfo;
	};		//	
	DRLGMap* pCurrent;			//+03C	
	int nLogicalCounter;		//+040	counter used by logical room info
	DRLGLevelInfo Info[32];		//+044	
	int nSpecialTiles;			//+1C4	count of all special tiles on this level
	DWORD nWarpRoomCenterX[9];	//+1C8	coord x of warp / waypoint room center
	DWORD nWarpRoomYCenter[9];	//+1EC	coord y of warp / waypoint room center
	int nWarpRooms;				//+210	count of all warp / waypoint rooms (max=9; 1 for WP, 8 for warps)
	int* pJunglePresets;		//+214	researched by Nefarius (RoomsW * RoomsH)
	int nClearingCount;			//+218	researched by Nefarius
	DRLGBuild* pBuild;			//+21C	list of ds1's used to build borders
	DWORD bActive;				//+220	set when entering / leaving the level
	int nInactiveFrames; 		//+224	counting backwards towards making the level inactive, free rooms etc.
	int* pRoomBools;			//+228	pointer to a list of bools for all DRLGRoomEx' that a had DRLGRoom visited by a client
	DRLGLevel* pNext;			//+22C
};


//////////////////////////////////////////////////////////////////////////
//
//	DRLGMisc

#define MAX_WARPS	8
struct DRLGWarp		// sizeof 0x48
{
	int nLevel;		// +00
	int nVis[MAX_WARPS];	// +04
	int nWarp[MAX_WARPS];	// +24
	DRLGWarp* pNext;// +44
};


enum eMiscFlags
{
	DRLGMISC_REFRESHFLAG = 0x01,
};


struct DRLGMisc							// sizeof() = 0x498
{
	DRLGLevel* pFirst;					//+000	latest added level
	void* pMemPool;						//+004	pGame+1C
	DWORD dwZero;						//+008	used when loading .ds1
	DRLGAct* pAct;						//+00C
	int nAct;							//+010
	D2Seed MiscSeed;					//+014
	DWORD dwStartSeed;					//+01C
	DWORD dwGameLoSeed;					//+020	pGame+7C?
	DWORD fMiscFlags;					//+024	0x1 is fClient; DRLGMISC_REFRESHFLAG 0x01
	DRLGRoomEx pRoomEx[4];				//+028	DRLGMiscData* pMiscData[4], +0D4 = 0 = clientinroom, +1C0 = 1 = clientinsight, +2AC= 2 = outofsight, +398 = 3 = untile room
	DWORD _3D8;							//+3D8	//used
	BYTE nRooms;						//+3DC	all pRooms - only 255 wtf?
	BYTE nUnk;							//+3DD	bServer*2+5; counts backwards
	BYTE _3DE;							//+3DE
	BYTE _3DF;							//+3DF
	int nRoomsVisited;					//+3E0	all pRooms
	int nRoomsInactive;					//+3E4	not 100% sure: inactive / deleted pRooms
	void* pGame;						//+3E8	NULL on Client
	int nDifficulty;					//+3EC	taken from pGame
	AUTOMAP_UPDATER pfRoomAutomap;		//+3F0	6FACD660	void __stdcall pfRoomAutomap(DRLGRoom* pRoom) = reveal room
	AUTOMAP_CHECKER pfLevelAutomap;		//+3F4	6FACD840	void __stdcall pfLevelAutomap(int nLvlId, int nFilePicked, int nX, int nY) reveal level
	DWORD nStaffTombLvl;				//+3F8
	DWORD nBossTombLvl;					//+3FC
	D2TileLibraryHashStrc* pTiles[32];	//+400	return from d2cmp.#10087 for town floor
	DWORD bJungleInterlink;				//+480	act 3 only
	DRLGWarp* pWarp;					//+484
};

//////////////////////////////////////////////////////////////////////////////
//
//	DRLGEnv

struct TimeEnvData
{
	int ticks;
	int daytime;
	BYTE red;
	BYTE green;
	BYTE blue;
	BYTE intensity;
};

struct DRLGEnv		// sizeof 0x38
{
	int idx;		// index in time of day table
	int dayTime;	// current time of day
	int ticks;		// ticks used for time counter
	int intensity;	// light intensity
	DWORD cputicks;	// GetTickCount()
	DWORD _14;		// not used
	BYTE red;
	BYTE green;
	BYTE blue;
	BYTE _1B;		// not used
	float f_cos;	// set, but never read
	float f_last;	// set, but never read
	float f_sin;	// set, but never read
	int rate;		// rate used to advance time
	DWORD init;		// controls if it uses 180ths or 4ths as the rate
	DWORD eclipse;	// controls how time is calculated
	int prev;		// previous ticks
};

//////////////////////////////////////////////////////////////////////////////
//
//	DRLGAct

struct DRLGAct				//sizeof 0x60
{
	int nAct;
	DRLGRoom* pRoom;		// most recent pRoom loaded
	DRLGMisc* pMisc;
	DWORD dwInitSeed;
	int nTown;
	DRLGEnv* pEnv;
	void* pCallback;		// 6FAA5E10 void __fastcall(pRoom); D2Client.6FAB8290 void __stdcall WallsUpdate(DRLGRoom* pRoom, void* pProllyARect)
	DWORD bClient;
	DWORD bUpdateRooms;		// set on room creation; controls funcs such as inactive store / restore  // removed after all rooms have been updated
	DWORD bDeleteUnits;		// set when a unit is added to the deletion list                          // removed after freeing the delete_units strc
	DWORD bUpdateUnits;		// set when a unit is added to a room, for updating a chain of units      // removed after releasing the units
	DRLGMapTileData ActData[1];
	void* pMemPool;
};

////////////////////////////////////////////////////////////////////
//
//	Outdoor Level Linker
//	@author	Necrolis

struct DRLGLevelLinkData        // sizeof 0x1F4
{
	D2Seed pSeed;            //+00
	DRLGCoordBox pLevelCoord[15];    //+08
	DRLGLink* pLink;            //+F8
	int nRand[4][15];            //+FC/138/174/1B0
	int nIteration;                //+1EC
	int nCurrentLevel;            //+1F0
};

typedef bool(*DRLGLINKER)	(DRLGLevelLinkData* pLinkData);
typedef void(*DRLGLINKEREX)	(DRLGLevel* pLevel, int nIteration, int* pRand);
typedef DWORD(*DRLGSPACE)	(DRLGLevelLinkData* pLinkData, int nIteration);

struct DRLGLink                    //sizeof 0x10
{
	DRLGLINKER pfLinker;        //+00
	int nLevel;                    //+04
	int nLevelLink;                //+08
	int nLevelLinkEx;            //+0C
};

struct DRLGLinkerParams            //sizeof 0x18
{
	int nLinkerLevels[3];
	int nChance[2];
	DWORD fFlags;
};
#pragma pack()

//////////////////////////////////////////////////////////////
//
//	Functions

// DRLG.cpp
DRLGLevel* DRLG_GetLevelFromID(DRLGMisc* pMisc, int nLevelID);
int* DRLG_GetWarpLevels(DRLGMisc* pMisc, int nLevel);
int* DRLG_GetVisLevels(DRLGMisc* pMisc, int nLevel);
bool DRLG_NotOverlapping(DRLGCoordBox* pA, DRLGCoordBox* pB, int nThreshold);
D2LvlMazeTxt* DRLG_GetMazeRecord(int nLevelID);
D2LvlPrestTxt* DRLG_GetPrestRecord(int nLevelID);

// DRLG_Generation.cpp
DRLGRoomEx* DRLG_AllocateRoomEx(DRLGLevel* pLevel, int nPresetType);
void DRLG_BindRoomEx(DRLGLevel* pLevel, DRLGRoomEx* pRoomEx);
DRLGLevel* DRLG_GenerateLevel(DRLGMisc* pMisc, int nLevel);
void DRLG_FreeLevels();

// DRLG_Link.cpp
void DRLG_MiscGenerateOverworld(DRLGMisc* pMisc);

// DRLG_Outdoors.cpp
void DRLG_InsertOutdoorGridRoom(DRLGLevel* pLevel, int nX, int nY, int nW, int nH, int nRoomFlags,
	int nOutdoorMapFlags, int nUnused, int dt1Mask);
void DRLG_CreateOutdoorVerData(DRLGVer** ppVer, DRLGCoordBox* pCoords, char nDirection, DRLGLinkData* pLink);
void DRLG_CreateAct1OutdoorsLevel(DRLGLevel* pLevel);
void DRLG_CreateAct2OutdoorsLevel(DRLGLevel* pLevel);
void DRLG_CreateAct3OutdoorsLevel(DRLGLevel* pLevel);
void DRLG_CreateAct4OutdoorsLevel(DRLGLevel* pLevel);
void DRLG_CreateAct5OutdoorsLevel(DRLGLevel* pLevel);

// DRLG_Preset.cpp
void DRLG_InsertPresetRoom(DRLGLevel* pLevel, DRLGMap* pMap, DRLGCoordBox* pCoords,
	int dt1Mask, int nRoomFlags, int nPresetFlags, DRLGGrid* pGrid);
DRLGMap* DRLG_CreateDRLGMap(DRLGLevel* pLevel, DRLGCoordBox* pCoords);