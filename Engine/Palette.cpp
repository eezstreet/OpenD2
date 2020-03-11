#include "Palette.hpp"
#include "FileSystem.hpp"

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

namespace Pal
{
	static bool gbPalettesInitialized = false;

	struct D2PaletteEntry {
		char szPath[MAX_D2PATH];
		char szPL2Path[MAX_D2PATH];
		D2Palette pal;
		PL2File pl2;
	};

	static D2PaletteEntry D2Palettes[PAL_MAX_PALETTES] = {
		{ "data\\global\\palette\\ACT1\\pal.dat",     "data\\global\\palette\\ACT1\\pal.pl2",     { 0 }, { 0 } },
		{ "data\\global\\palette\\ACT2\\pal.dat",     "data\\global\\palette\\ACT2\\pal.pl2",     { 0 }, { 0 } },
		{ "data\\global\\palette\\ACT3\\pal.dat",     "data\\global\\palette\\ACT3\\pal.pl2",     { 0 }, { 0 } },
		{ "data\\global\\palette\\ACT4\\pal.dat",     "data\\global\\palette\\ACT4\\pal.pl2",     { 0 }, { 0 } },
		{ "data\\global\\palette\\ACT5\\pal.dat",     "data\\global\\palette\\ACT5\\pal.pl2",     { 0 }, { 0 } },
		{ "data\\global\\palette\\ENDGAME\\pal.dat",  "data\\global\\palette\\ENDGAME\\pal.pl2",  { 0 }, { 0 } },
		{ "data\\global\\palette\\ENDGAME2\\pal.dat", "data\\global\\palette\\ENDGAME2\\pal.pl2", { 0 }, { 0 } },
		{ "data\\global\\palette\\FECHAR\\pal.dat",   "data\\global\\palette\\FECHAR\\pal.pl2",   { 0 }, { 0 } },
		{ "data\\global\\palette\\LOADING\\pal.dat",  "data\\global\\palette\\LOADING\\pal.pl2",  { 0 }, { 0 } },
		{ "data\\global\\palette\\MENU0\\pal.dat",    "data\\global\\palette\\MENU0\\pal.pl2",    { 0 }, { 0 } },
		{ "data\\global\\palette\\MENU1\\pal.dat",    "data\\global\\palette\\MENU1\\pal.pl2",    { 0 }, { 0 } },
		{ "data\\global\\palette\\MENU2\\pal.dat",    "data\\global\\palette\\MENU2\\pal.pl2",    { 0 }, { 0 } },
		{ "data\\global\\palette\\MENU3\\pal.dat",    "data\\global\\palette\\MENU3\\pal.pl2",    { 0 }, { 0 } },
		{ "data\\global\\palette\\MENU4\\pal.dat",    "data\\global\\palette\\MENU4\\pal.pl2",    { 0 }, { 0 } },
		{ "data\\global\\palette\\SKY\\pal.dat",      "data\\global\\palette\\SKY\\pal.pl2",      { 0 }, { 0 } },
		{ "data\\global\\palette\\STATIC\\pal.dat",   "data\\global\\palette\\STATIC\\pal.pl2",   { 0 }, { 0 } },
		{ "data\\global\\palette\\TRADEMARK\\pal.dat","data\\global\\palette\\TRADEMARK\\pal.pl2",{ 0 }, { 0 } },
		{ "data\\global\\palette\\UNITS\\pal.dat",    "data\\global\\palette\\UNITS\\pal.pl2",    { 0 }, { 0 } },
	};

	/*
	 *	Registers a palette and stores it in pPalette argument
	 */
	static bool RegisterPalette(const char* szPalettePath, const char* szPL2Path, D2Palette* pPalette, PL2File* pl2)
	{
		if (pPalette == nullptr || pl2 == nullptr)
		{	// bad palette pointer
			return false;
		}

		fs_handle f;
		DWORD dwPaletteSize = FS::Open(szPalettePath, &f, FS_READ, true);
		if (f == INVALID_HANDLE)
		{	// Couldn't register palette
			return false;
		}
		if (dwPaletteSize != sizeof(D2Palette))
		{
			// bad file
			FS::CloseFile(f);
			return false;
		}

		// read the palette
		FS::Read(f, **pPalette, dwPaletteSize);
		FS::CloseFile(f);

		// open the PL2 now
		dwPaletteSize = FS::Open(szPL2Path, &f, FS_READ, true);
		if (f == INVALID_HANDLE)
		{	// bad PL2 file
			return false;
		}
		if(dwPaletteSize != sizeof(PL2File))
		{
			// bad file
			FS::CloseFile(f);
			return false;
		}

		// read the PL2 file
		FS::Read(f, pl2, dwPaletteSize);
		FS::CloseFile(f);

		return true;
	}

	/*
	 *	Initializes the palette subsystem
	 */
	bool Init()
	{
		for (int i = 0; i < PAL_MAX_PALETTES; i++)
		{
			D2PaletteEntry* pEntry = &D2Palettes[i];
			if (!RegisterPalette(pEntry->szPath, pEntry->szPL2Path, &pEntry->pal, &pEntry->pl2))
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
	D2Palette* GetPalette(int nIndex)
	{
		if (!gbPalettesInitialized || nIndex < 0 || nIndex >= PAL_MAX_PALETTES)
		{
			return nullptr;
		}
		return &D2Palettes[nIndex].pal;
	}

	/**
	 *  Gets the color modulation for a particular color.
	 */
	bool GetPL2ColorModulation(int palette, int color, float& R, float& G, float& B)
	{
		if(!gbPalettesInitialized || nIndex < 0 || nIndex >= PAL_MAX_PALETTES)
		{
			return false;
		}
		if(color < 0 || color >= 13)
		{
			return false;
		}
		
		R = D2Palettes[nIndex].pl2.pStandardColors[color][0] / 255.0f;
		G = D2Palettes[nIndex].pl2.pStandardColors[color][1] / 255.0f;
		B = D2Palettes[nIndex].pl2.pStandardColors[color][2] / 255.0f;
		return true;
	}
}
