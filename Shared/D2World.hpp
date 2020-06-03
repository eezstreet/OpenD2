#pragma once
#include "D2Shared.hpp"

// The Diablo 2 world as expressed in OpenD2 consists of Quad-Trees.
// Each Quad-Tree breaks down to a 8x8 subunit known as a Room.
// Each tile has 16 subtiles which are used for collision, light propagation, LOS, etc.

#define MAX_ROOM_SIZE	8

struct WorldTree 
{
	bool bIsRoom;

	union
	{
		struct
		{
			WorldTree* pNorthWest;
			WorldTree* pNorthEast;
			WorldTree* pSouthWest;
			WorldTree* pSouthEast;

			DWORD dwTreeWidth;	// extent from west to east, in tiles
			DWORD dwTreeHeight; // extent from north to south, in tiles
		} tree;

		struct
		{
			tile_handle tiles[MAX_ROOM_SIZE][MAX_ROOM_SIZE];
		} room;
	} treeData;
};