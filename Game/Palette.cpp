#include "Diablo2.hpp"

/*
 *
 *	ALL ABOUT PALETTES
 *	Diablo II has 16 palettes + 1 general purpose "units" palette for all things GUI related.
 *	When rendering any image or animation, we must also specify which palette we want to use.
 *	The reason is simple: the image and animation files are actually encoded as 8-bit, to save on memory.
 *	The following is a brief explanation of each palette and what it does:
 *
 *	ACT1 - Act 1 world palette
 *	ACT2 - Act 2 world palette
 *	ACT3 - Act 3 world palette
 *	ACT4 - Act 4 world palette
 *	ACT5 - Act 5 world palette
 *	ENDGAME - Used for the endgame screen only
 *	FECHAR - Used for the character creation screen only
 *	LOADING - Used for the loading screen only
 *	MENU0 - Not used
 *	MENU1 - Not used
 *	MENU2 - Not used
 *	MENU3 - Not used
 *	MENU4 - Not used
 *	SKY - Unknown; most likely not used
 *	STATIC - Unknown
 *	TRADEMARK - Used for the trademark screen only
 *	UNITS - Used for 99% of things, including most UI
 *
 *	TODO: handle .pl2 files for colormapping
 */

static bool gbPalettesInitialized = false;

struct D2PaletteEntry {
	char szPath[MAX_D2PATH];
	D2Palette pal;
};

static D2PaletteEntry D2Palettes[PAL_MAX_PALETTES] = {
	{ "data\\global\\palette\\ACT1\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\ACT2\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\ACT3\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\ACT4\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\ACT5\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\ENDGAME\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\FECHAR\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\LOADING\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\MENU0\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\MENU1\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\MENU2\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\MENU3\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\MENU4\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\SKY\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\STATIC\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\TRADEMARK\\pal.dat",{ 0 } },
	{ "data\\global\\palette\\UNITS\\pal.dat",{ 0 } },
};

/*
 *	Registers a palette and stores it in pPalette argument
 */
static bool Pal_RegisterPalette(char* szPalettePath, D2Palette* pPalette)
{
	D2MPQArchive* pArchive = nullptr;

	if (pPalette == nullptr)
	{	// bad palette pointer
		return false;
	}

	fs_handle f = FSMPQ_FindFile(szPalettePath, nullptr, &pArchive);
	if (f == (fs_handle)-1 || pArchive == nullptr)
	{	// Couldn't register palette
		return false;
	}

	size_t dwPaletteSize = MPQ_FileSize(pArchive, f);
	if (dwPaletteSize != sizeof(D2Palette))
	{	// Not correct size = don't load
		return false;
	}

	// read the palette
	MPQ_ReadFile(pArchive, f, **pPalette, dwPaletteSize);
	return true;
}

/*
 *	Initializes the palette subsystem
 */
bool Pal_Init()
{
	for (int i = 0; i < PAL_MAX_PALETTES; i++)
	{
		D2PaletteEntry* pEntry = &D2Palettes[i];
		if (!Pal_RegisterPalette(pEntry->szPath, &pEntry->pal))
		{
			return false;
		}
	}
	gbPalettesInitialized = true;
	return true;
}

/*
 *	Retrieves a palette with the specified index
 */
D2Palette* Pal_GetPalette(int nIndex)
{
	if (!gbPalettesInitialized || nIndex < 0 || nIndex >= PAL_MAX_PALETTES)
	{
		return nullptr;
	}
	return &D2Palettes[nIndex].pal;
}