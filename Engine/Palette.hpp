#pragma once
#include "../Shared/D2Shared.hpp"

/**
 *	@author Necrolis
 */
#pragma pack(push,enter_include)
#pragma pack(1)
struct PL2File
{
	uint32_t pRGBAPalette[256];            //+00000
	uint8_t pShadows[32][256];            //+00400
	uint8_t pLight[16][256];            //+02400
	uint8_t pGamma[256];                //+03400
	uint8_t pTrans25[256][256];            //+03500
	uint8_t pTrans50[256][256];            //+13500
	uint8_t pTrans75[256][256];            //+23500
	uint8_t pScreen[256][256];            //+33500
	uint8_t pLuminance[256][256];        //+43500
	uint8_t pStates[128][256];            //+53500
	uint8_t pDarkBlend[256][256];        //+5B500
	uint8_t pDarkenPalette[256];        //+6B500
	uint8_t pStandardColors[13][3];        //+6B600
	uint8_t pStandardShifts[13][256];    //+6B627    
};
#pragma pack(pop,enter_include)

enum PL2Entry
{
	PL2_RGBAPalette,
	PL2_Shadows = 1,
	PL2_Light = PL2_Shadows + 32,
	PL2_Gamma = PL2_Light + 16,
	PL2_Trans25,
	PL2_Trans50 = PL2_Trans25 + 256,
	PL2_Trans75 = PL2_Trans50 + 256,
	PL2_Screen = PL2_Trans75 + 256,
	PL2_Luminance = PL2_Screen + 256,
	PL2_States = PL2_Luminance + 256,
	PL2_DarkBlend = PL2_States + 128,
	PL2_DarkenPalette = PL2_DarkBlend + 256,
	PL2_StandardColors = PL2_DarkenPalette + 1,
	PL2_StandardShifts = PL2_StandardColors + 13,
	PL2_NumEntries = PL2_StandardShifts + 13,
};

// Palette.cpp
namespace Pal
{
	bool Init();
	D2Palette* GetPalette(int nIndex);
}