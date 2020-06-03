#include "TBL_Font.hpp"
#include "FileSystem.hpp"

////////////////////////////////////////////////////////
//
//	Font-Based .TBL files store information about game fonts.
//	These should not be confused with Text-Based TBLs.

#define MAX_TBL_FONTS		32		// I counted 16, there might be more?
#define FONT_TBL_VERSION	1

namespace TBLFont
{

	static TBLFontFile FontTable[MAX_TBL_FONTS]{ 0 };
	static tbl_handle gLastUsedIndex = 0;

	/*
	 *	Registers a font TBL file.
	 */
	tbl_handle RegisterFont(const char* szFontName)
	{
		char szFilePath[MAX_D2PATH]{ 0 };
		fs_handle file;
		tbl_handle current = gLastUsedIndex;

		snprintf(szFilePath, MAX_D2PATH, "data/local/FONT/%s/%s.tbl", GAME_CHARSET, szFontName);

		FS::Open(szFilePath, &file, FS_READ, true);
		FS::Read(file, (BYTE*)&FontTable[gLastUsedIndex], sizeof(TBLFontFile));
		FS::CloseFile(file);

		gLastUsedIndex++;

		return current;
	}

	TBLFontFile* GetPointerFromHandle(tbl_handle handle)
	{
		if (handle == INVALID_HANDLE)
		{	// literally impossible but whatever
			return nullptr;
		}

		if (handle >= MAX_TBL_FONTS)
		{	// also literally impossible
			return nullptr;
		}

		return &FontTable[handle];
	}
}
