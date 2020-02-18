#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	Font TBLs
 *	Based on structures provided by Necrolis
 *	@author	eezstreet
 */

#pragma pack(push,enter_include)
#pragma pack(1)

 // FIXME: there's a lot of unknowns here, they need to be properly investigated with a debugger
struct TBLFontGlyph
{
	WORD			wChar;		// Unicode character code
	BYTE			nUnknown1;
	BYTE			nWidth;
	BYTE			nHeight;
	BYTE			nUnknown2;
	WORD			wUnknown3;
	BYTE			nImageIndex;
	BYTE			nChar;		// ANSI character code
	DWORD			dwUnknown4;	// NOTE: on SDL renderer this gets set to the X offset of the glyph on atlas
};

struct TBLFontFile
{
	BYTE			magic[4];	// always "Woo!", this exclamation appears in the save file format too
	WORD			wVersion;
	DWORD			dwLocale;	// 1 = english, 5 = russian, ...?
	BYTE			nHeight;	// interline spacing
	BYTE			nWidth;		// cap height (?)
	TBLFontGlyph	glyphs[256];
};

#pragma pack(pop, enter_include)

// TBL_Font.cpp
namespace TBLFont
{
	tbl_handle RegisterFont(const char* szFontName);
	TBLFontFile* GetPointerFromHandle(tbl_handle handle);
}