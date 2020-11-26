#include "DS1.hpp"
#include "../Shared/D2HashMap.hpp"
#include "FileSystem.hpp"
#include "Logging.hpp"

namespace DS1
{
	HashMap<char, DS1File*> loadedDS1Files;

	handle LoadDS1(const char* path)
	{
		handle returnValue;
		bool isFull = false;
		if (loadedDS1Files.Contains(path, &returnValue, &isFull))
		{
			return returnValue;
		} 
		else if (isFull)
		{
			return INVALID_HANDLE;
		}

		returnValue = loadedDS1Files.NextFree(path);
		loadedDS1Files.Insert(returnValue, path, new DS1File(path));
		return returnValue;
	}
}

BYTE directionLookup[] = {
		0x00, 0x01, 0x02, 0x01, 0x02, 0x03, 0x03, 0x05, 0x05, 0x06,
		0x06, 0x07, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
		0x0F, 0x10, 0x11, 0x12, 0x14
};

DS1File::DS1File(const char* path) :
	layerHeader({ 0 }),
	fileHeader({ 0 }),
	pUnknownLayer(nullptr),
	pFloorCells(nullptr),
	pWallCells(nullptr),
	pShadowCells(nullptr),
	pTagGroups(nullptr),
	pTags(nullptr),
	objectHeader({ 0 }),
	dwNumTagGroups(0)
{
	fs_handle f;
	size_t fileSize = FS::Open(path, &f, FS_READ, true);
	BYTE* buffer = (BYTE*)malloc(fileSize);
	Log_ErrorAssertReturn(buffer);
	FS::Read(f, buffer, 1, fileSize);
	FS::CloseFile(f);

	memset(optionalFileList, 0, sizeof(DS1Path) * 8);
	memset(objects, 0, sizeof(DS1Object) * MAX_DS1_OBJECTS);

	// start reading stuff into the header
	DWORD* readHead = (DWORD*)buffer;
	fileHeader.version = *readHead;
	readHead++;
	fileHeader.dwWidth = (*readHead) + 1;
	readHead++;
	fileHeader.dwHeight = (*readHead) + 1;
	readHead++;

	fileHeader.dwAct = 1;
	if (fileHeader.version >= 8)
	{
		// this version of DS1 has an "act" DWORD inserted here.
		// Not all DS1s have this.
		fileHeader.dwAct = *readHead;
		readHead++;
		if (fileHeader.dwAct > 5)
		{
			fileHeader.dwAct = 5;
		}
	}

	layerHeader.dwShadowLayers = 1;
	layerHeader.dwTagLayersPresent = 0;
	if (fileHeader.version >= 10)
	{
		layerHeader.dwTagLayersPresent = *readHead;
		readHead++;

		if (layerHeader.dwTagLayersPresent == 1 || layerHeader.dwTagLayersPresent == 2)
		{
			layerHeader.dwTagLayers = 1;
		}
	}

	// read filenames (these don't appear to be used and are vestigial)
	if (fileHeader.version >= 3)
	{
		fileHeader.dwExtraFilesCount = *readHead;
		readHead++;

		for (int x = 0; x < fileHeader.dwExtraFilesCount; x++)
		{
			size_t n = strlen((char*)readHead) + 1;
			D2Lib::strncpyz(optionalFileList[x], (const char*)readHead, MAX_D2PATH);
			readHead = (DWORD*)(((char*)readHead) + n);
		}
	}

	// skip two DWORDs in some cases
	if (fileHeader.version >= 9 && fileHeader.version <= 13)
	{
		readHead += 2;
	}

	// load number of wall, floor, etc layers
	if (fileHeader.version >= 4)
	{
		layerHeader.dwWallLayers = *readHead;
		readHead++;

		if (fileHeader.version >= 16)
		{
			layerHeader.dwFloorLayers = *readHead;
			readHead++;
		}
		else
		{
			layerHeader.dwFloorLayers = 1;
		}
	}
	else
	{
		layerHeader.dwFloorLayers = 1;
		layerHeader.dwWallLayers = 1;
		layerHeader.dwTagLayers = 1;
	}

	// fix the layer ordering
	if (fileHeader.version < 4)
	{
		layerHeader.nLayerLayout[0] = DS1LayerTypes::Wall1;
		layerHeader.nLayerLayout[1] = DS1LayerTypes::Floor1;
		layerHeader.nLayerLayout[2] = DS1LayerTypes::Orientation1;
		layerHeader.nLayerLayout[3] = DS1LayerTypes::Tag;
		layerHeader.nLayerLayout[4] = DS1LayerTypes::Shadow;
		layerHeader.dwTotalLayersPresent = 5;
	}
	else
	{
		DWORD layersPresent = 0;
		for (int x = 0; x < layerHeader.dwWallLayers; x++)
		{
			layerHeader.nLayerLayout[layersPresent++] = DS1LayerTypes::Wall1 + x;
			layerHeader.nLayerLayout[layersPresent++] = DS1LayerTypes::Orientation1 + x;
		}

		for (int x = 0; x < layerHeader.dwFloorLayers; x++)
		{
			layerHeader.nLayerLayout[layersPresent++] = DS1LayerTypes::Floor1 + x;
		}

		if (layerHeader.dwShadowLayers)
		{
			layerHeader.nLayerLayout[layersPresent++] = DS1LayerTypes::Shadow;
		}

		if (layerHeader.dwTagLayers)
		{
			layerHeader.nLayerLayout[layersPresent++] = DS1LayerTypes::Tag;
		}

		layerHeader.dwTotalLayersPresent = layersPresent;
	}

	// walls
	DWORD wallLine = fileHeader.dwWidth * layerHeader.dwWallLayers;
	DWORD wallLen = wallLine * fileHeader.dwHeight;
	size_t wallBufferLen = wallLen * sizeof(DS1File::DS1Cell);
	pWallCells = (DS1File::DS1Cell*)malloc(wallBufferLen);
	memset(pWallCells, 0, wallBufferLen);

	// floors
	DWORD floorLine = fileHeader.dwWidth * layerHeader.dwFloorLayers;
	DWORD floorLen = floorLine * fileHeader.dwHeight;
	size_t floorBufferLen = floorLen * sizeof(DS1File::DS1Cell);
	pFloorCells = (DS1File::DS1Cell*)malloc(floorBufferLen);
	memset(pFloorCells, 0, floorBufferLen);

	// shadow
	DWORD shadowLine = fileHeader.dwWidth * layerHeader.dwShadowLayers;
	DWORD shadowLen = shadowLine * fileHeader.dwHeight;
	size_t shadowBufferLen = shadowLen * sizeof(DS1File::DS1Cell);
	pShadowCells = (DS1File::DS1Cell*)malloc(shadowBufferLen);
	memset(pShadowCells, 0, shadowBufferLen);

	// tags
	DWORD tagLine = fileHeader.dwWidth * layerHeader.dwTagLayers;
	DWORD tagLen = tagLine * fileHeader.dwHeight;
	size_t tagBufferLen = tagLen * sizeof(DS1File::DS1Tag);
	pTags = (DS1File::DS1Tag*)malloc(tagBufferLen);
	memset(pTags, 0, tagBufferLen);

	DS1File::DS1Cell* pFloor[MAX_DS1_FLOOR_LAYERS];
	DS1File::DS1Cell* pShadow[MAX_DS1_SHADOW_LAYERS];
	DS1File::DS1Tag* pTag[MAX_DS1_TAG_LAYERS];
	DS1File::DS1Cell* pOrientation[MAX_DS1_WALL_LAYERS];
	DS1File::DS1Cell* pWall[MAX_DS1_WALL_LAYERS];

	for (int x = 0; x < MAX_DS1_FLOOR_LAYERS; x++)
	{
		pFloor[x] = pFloorCells + x;
	}

	for (int x = 0; x < MAX_DS1_SHADOW_LAYERS; x++)
	{
		pShadow[x] = pShadowCells + x;
	}

	for (int x = 0; x < MAX_DS1_TAG_LAYERS; x++)
	{
		pTag[x] = pTags + x;
	}

	for (int x = 0; x < MAX_DS1_WALL_LAYERS; x++)
	{
		pOrientation[x] = pWallCells + x;
		pWall[x] = pWallCells + x;
	}

	DWORD w = fileHeader.dwWidth;
	DWORD h = fileHeader.dwHeight;
	BYTE* bptr = (BYTE*)readHead;

	for (int n = 0; n < layerHeader.dwTotalLayersPresent; n++)
	{
		for (int y = 0; y < fileHeader.dwHeight; y++)
		{
			for (int x = 0; x < fileHeader.dwWidth; x++)
			{
				switch (layerHeader.nLayerLayout[n])
				{
				case DS1LayerTypes::Wall1:
				case DS1LayerTypes::Wall2:
				case DS1LayerTypes::Wall3:
				case DS1LayerTypes::Wall4:
					if (x < w && y < h)
					{
						int p = layerHeader.nLayerLayout[n] - DS1LayerTypes::Wall1;
						pWall[p]->prop1 = *bptr;
						bptr++;
						pWall[p]->prop2 = *bptr;
						bptr++;
						pWall[p]->prop3 = *bptr;
						bptr++;
						pWall[p]->prop4 = *bptr;
						bptr++;
						pWall[p] += layerHeader.dwWallLayers;
					}
					else
					{
						bptr += 4;
					}
					break;

				case DS1LayerTypes::Orientation1:
				case DS1LayerTypes::Orientation2:
				case DS1LayerTypes::Orientation3:
				case DS1LayerTypes::Orientation4:
					if (x < w && y < h)
					{
						int p = layerHeader.nLayerLayout[n] - DS1LayerTypes::Orientation1;
						if (fileHeader.version < 7)
						{
							pOrientation[p]->orientation = directionLookup[*bptr];
						}
						else
						{
							pOrientation[p]->orientation = *bptr;
						}
						pOrientation[p] += layerHeader.dwWallLayers;
					}
					bptr += 4;
					break;

				case DS1LayerTypes::Floor1:
				case DS1LayerTypes::Floor2:
					if (x < w && y < h)
					{
						int p = layerHeader.nLayerLayout[n] - DS1LayerTypes::Floor1;
						pFloor[p]->prop1 = *bptr;
						bptr++;
						pFloor[p]->prop2 = *bptr;
						bptr++;
						pFloor[p]->prop3 = *bptr;
						bptr++;
						pFloor[p]->prop4 = *bptr;
						bptr++;
						pFloor[p] += layerHeader.dwFloorLayers;
					}
					else
					{
						bptr += 4;
					}
					break;

				case DS1LayerTypes::Shadow:
					if (x < w && y < h)
					{
						int p = layerHeader.nLayerLayout[n] - DS1LayerTypes::Shadow;
						pShadow[p]->prop1 = *bptr;
						bptr++;
						pShadow[p]->prop2 = *bptr;
						bptr++;
						pShadow[p]->prop3 = *bptr;
						bptr++;
						pShadow[p]->prop4 = *bptr;
						pShadow[p] += layerHeader.dwShadowLayers;
					}
					else
					{
						bptr += 4;
					}
					break;

				case DS1LayerTypes::Tag:
					if (x < w && y < h)
					{
						int p = layerHeader.nLayerLayout[n] - DS1LayerTypes::Tag;
						pTag[p]->num = *((DWORD*)bptr);
						pTag[p] += layerHeader.dwTagLayers;
					}
					bptr += 4;
					break;
				}
			}
		}
	}

	readHead = (DWORD*)bptr;

	// load objects
	objectHeader.dwNumObjects = 0;
	if (fileHeader.version >= 2)
	{
		objectHeader.dwNumObjects = *readHead;
		readHead++;

		const int maxSubtileWidth = fileHeader.dwWidth * 5;
		const int maxSubtileHeight = fileHeader.dwHeight * 5;
		for (DWORD n = 0; n < objectHeader.dwNumObjects; n++)
		{
			if (n > MAX_DS1_OBJECTS)
			{
				Log::Warning("DS1 %s has more objects (%d) than max (%d), loaded objects will be truncated.\n",
					path, objectHeader.dwNumObjects, MAX_DS1_OBJECTS);
				objectHeader.dwNumObjects = MAX_DS1_OBJECTS;
				break;
			}
			objects[n].dwType = *readHead;
			readHead++;
			objects[n].dwId = *readHead;
			readHead++;
			objects[n].dwX = *readHead;
			readHead++;
			objects[n].dwY = *readHead;
			readHead++;

			if (fileHeader.version > 5)
			{
				objects[n].dwFlags = *readHead;
				readHead++;
			}

			// TODO: integrity checking.
		}
	}

	// load groups for tags
	// NOTENOTE: There can be less groups than expected, such as in data\global\tiles\act1\outdoors\trees.ds1,
	// where the file stops right after the last tileX group data, leaving the other data unknown.
	// This is why we explicitly check if the read head is going out of bounds past this point.
	if (fileHeader.version >= 12 &&
		(layerHeader.dwTagLayersPresent == 1 || layerHeader.dwTagLayersPresent == 2))
	{
		if (fileHeader.version >= 18)
		{
			// skip a DWORD here
			readHead++;
		}

		dwNumTagGroups = *readHead;
		readHead++;

		size_t groupSize = dwNumTagGroups * sizeof(DS1File::DS1TagGroup);
		pTagGroups = (DS1File::DS1TagGroup*)malloc(groupSize);
		if (pTagGroups == nullptr)
		{
			return;
		}
		memset(pTagGroups, 0, groupSize);

		for (DWORD x = 0; x < dwNumTagGroups; x++)
		{
			if ((BYTE*)readHead < buffer + fileSize)
			{
				pTagGroups[x].tileX = *readHead;
			}
			readHead++;
			if ((BYTE*)readHead < buffer + fileSize)
			{
				pTagGroups[x].tileY = *readHead;
			}
			readHead++;
			if ((BYTE*)readHead < buffer + fileSize)
			{
				pTagGroups[x].width = *readHead;
			}
			readHead++;
			if ((BYTE*)readHead < buffer + fileSize)
			{
				pTagGroups[x].height = *readHead;
			}
			readHead++;
			if (fileHeader.version >= 13)
			{
				if ((BYTE*)readHead < buffer + fileSize)
				{
					pTagGroups[x].unknown = *readHead;
				}
				readHead++;
			}
		}
	}

	// load NPC paths
	if (fileHeader.version >= 14)
	{
		DWORD npc = 0;
		if ((BYTE*)readHead < buffer + fileSize)
		{
			npc = *readHead;
		}
		readHead++;

		for (DWORD n = 0; n < npc; n++)
		{	// note, we don't check for out of bound access here because there aren't any incomplete DS1s like this
			DWORD path = *readHead;
			readHead++;
			DWORD x = *readHead;
			readHead++;
			DWORD y = *readHead;
			readHead++;

			// find the object associated with the path data
			DWORD o = 0, lastO = 0, nb = 0;
			do
			{
				if (o < objectHeader.dwNumObjects)
				{
					if (objects[o].dwX == x && objects[o].dwY == y)
					{
						lastO = o;
						nb++;
						if (nb >= 2)
						{
							break;
						}
					}
					o++;
				}
				else
				{
					break;
				}
			} while (true);

			if (nb >= 2)
			{
				// There are at least 2 objects at the same coordinates.
				Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Found two objects at the same coordinates in %s.\n",
					path);
				Log::Print(OpenD2LogFlags::PRIORITY_MESSAGE, "Removing %d path points at coordinates (%d,%d)\n",
					path, x, y);

				// first, delete the already assigned paths
				for (o = 0; o < objectHeader.dwNumObjects; o++)
				{
					if (objects[o].dwX == x && objects[o].dwY == y && objects[o].dwPathNumber != 0)
					{
						memset(objects[o].paths, 0, sizeof(objects[o].paths[0]) * 100);
						objects[o].dwPathNumber = 0;
					}
				}

				// skip these paths
				if (fileHeader.version >= 15)
				{	// 3 dwords
					readHead += (3 * path);
				}
				else
				{	// 2 dwords
					readHead += (2 * path);
				}
			}
			else
			{
				// only one object at these coordinates.
				objects[o].dwPathNumber = path;
				for (DWORD p = 0; p < path; p++)
				{
					objects[o].paths[p].x = *readHead;
					readHead++;
					objects[o].paths[p].y = *readHead;
					readHead++;
					if (fileHeader.version >= 15)
					{	// version 15 adds more than 1 action type per path
						objects[o].paths[p].action = *readHead;
						readHead++;
					}
					else
					{
						objects[o].paths[p].action = 1;
					}
				}
			}
		}
	}

	free(buffer);
}

DS1File::~DS1File()
{
#define FreeIfExisting(x) if(x) free(x);
	FreeIfExisting(pFloorCells);
	FreeIfExisting(pShadowCells);
	FreeIfExisting(pTagGroups);
	FreeIfExisting(pTags);
#undef	FreeIfExisting
}