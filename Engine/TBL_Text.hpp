#pragma once
#include "../Shared/D2Shared.hpp"


/*
 *	String Archive TBLs
 *	Based on code from QTblEditor by kambala
 *	@author eezstreet
 */

#define MAX_TBL_FILE_HANDLE	16
#define MAX_TBL_KEY_SIZE	64

#pragma pack(push,enter_include)
#pragma pack(1)

struct TBLHeader // header of string *.tbl file
{
	WORD  CRC;             // +0x00 - CRC value for string table
	WORD  NodesNumber;     // +0x02 - size of Indices array
	DWORD HashTableSize;   // +0x04 - size of TblHashNode array
	BYTE  Version;         // +0x08 - file version, either 0 or 1, doesn't matter
	DWORD DataStartOffset; // +0x09 - string table start offset
	DWORD HashMaxTries;    // +0x0D - max number of collisions for string key search based on its hash value
	DWORD FileSize;        // +0x11 - size of the file
};

struct TBLHashNode // node of the hash table in string *.tbl file
{
	BYTE  Active;          // +0x00 - shows if the entry is used. if 0, then it has been "deleted" from the table
	WORD  Index;           // +0x01 - index in Indices array
	DWORD HashValue;       // +0x03 - hash value of the current string key
	DWORD StringKeyOffset; // +0x07 - offset of the current string key
	DWORD StringValOffset; // +0x0B - offset of the current string value
	WORD  StringValLength; // +0x0F - length of the current string value
};

struct TBLDataNode
{
	BYTE		bActive;
	char16_t	key[MAX_TBL_KEY_SIZE];
	char16_t*	str;
};

struct TBLFile
{
	TBLHeader header;
	TBLHashNode* pHashNodes;
	TBLDataNode* pDataNodes;
	WORD* pIndices;

	char szHandle[MAX_TBL_FILE_HANDLE];
	D2MPQArchive* pArchive;	// the archive that this file was found in
	fs_handle archiveHandle;	// the file inside the archive

	size_t dwFileSize; // the size of the whole TBL file
	char16_t* szStringTable; // a table that contains all of the string data
};

#pragma pack(pop, enter_include)

// TBL_Text.cpp
namespace TBL
{
	tbl_handle Register(const char *szTblFile);
	char16_t* FindStringFromIndex(DWORD dwIndex);
	tbl_handle FindStringIndexFromKey(tbl_handle tbl, char16_t* szReference);
	char16_t* FindStringText(char16_t* szReference);
	void Init();
	void Cleanup();
}