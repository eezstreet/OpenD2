#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	COF (component object files)
 *	COF consists of three key elements: a header, a series of layers, and a series of keyframes.
 *	The layer count and keyframe count are included as part of the header.
 *	Each COF has exactly seven characters in its name (not including the extension), and is formatted thusly:
 *	<token name> <animation> <weapon set>
 *	So for example, the Amazon's first attack animation, using a staff, would be:
 *	AMA1STF.cof (AM for the AMazon token, A1 for the first attack animation, STF for the STaFf.
 *	For optimization (?), some COF metadata is stored in animdata.d2, however we don't need this for our purposes.
 *	These structures are provided by Paul Siramy.
 */

#pragma pack(push,enter_include)
#pragma pack(1)
enum COFKeyframe
{
	COFKEY_NONE,
	COFKEY_ATTACK,
	COFKEY_MISSILE,
	COFKEY_SOUND,
	COFKEY_SKILL,
	COFKEY_MAX,
};

struct COFHeader
{
	BYTE	nLayers;
	BYTE	nFrames;
	BYTE	nDirs;
	BYTE	nVersion;
	DWORD	dwUnk1;
	int		nXMin;
	int		nXMax;
	int		nYMin;
	int		nYMax;
	WORD	nFPS;
	/*BYTE	nFPS;
	BYTE	nArmType;*/
	WORD	wUnk2;
};

struct COFLayer
{
	BYTE	nComponent;
	BYTE	bShadow;
	BYTE	bSelectable;
	BYTE	nOverrideTranslvl;
	BYTE	nNewTranslvl;
	BYTE	szWeaponClass[4];
};

struct COFFile
{
	COFHeader	header;
	COFLayer*	layers;
	BYTE*		keyframes;

	// The following aren't in the file. They are just loaded for my purposes.
	DWORD		dwLayersPresent;
};
#pragma pack(pop, enter_include)

// COF.cpp
namespace COF
{
	cof_handle Register(const char *type, const char *token, const char *animation, const char *hitclass);
	void Deregister(cof_handle cof);
	void DeregisterType(char* type);
	void DeregisterAll();
	bool LayerPresent(cof_handle cof, int layer);
	COFFile* GetFileData(cof_handle handle);
};