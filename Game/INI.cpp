#include "Diablo2.hpp"

#define INI_MAX_SECTION_NAMELEN		16
#define INI_MAX_KEY_NAMELEN			64
#define INI_MAX_SECTIONS			16
#define INI_MAX_FIELDS_PER_SECTION	64
#define INI_MAX_STRINGLEN			MAX_D2PATH_ABSOLUTE

extern D2CmdArgStrc CommandArguments[];
extern D2CmdArgStrc OpenD2CommandArguments[];

////////////////////////////////////////////////////
//
//	INI File Parsing
//	This is only ever used for the config files
namespace INI
{

	enum INIFieldType
	{
		INI_STRING,
		INI_BOOL,
		INI_INTEGER
	};

	struct INIField
	{
		char			szKeyName[INI_MAX_KEY_NAMELEN]{ 0 };
		INIFieldType	fieldType;
		union
		{
			bool		bValue;
			int			nValue;
			char		szValue[INI_MAX_STRINGLEN]{ 0 };
		} fieldValues;
	};

	struct INISection
	{
		char			szSectionName[INI_MAX_SECTION_NAMELEN];
		int				nNumberFields;
		INIField		fields[INI_MAX_FIELDS_PER_SECTION];
	};

	typedef INISection* INIFile;

	/*
	 *	Finds a free section of an INI
	 */
	static handle FindSection(INIFile* pFile, char* szSectionName)
	{
		INIFile file = *pFile;
		for (int i = 0; i < INI_MAX_SECTIONS; i++)
		{
			if (file[i].szSectionName[0] == '\0' || !D2Lib::stricmp(file[i].szSectionName, szSectionName))
			{
				return (handle)i;
			}
		}
		return INVALID_HANDLE;
	}

	/*
	 *	Writes an INI file
	 */
	static void WriteINIInternal(INIFile* pFile, fs_handle* f)
	{
		INIFile ini = *pFile;
		INISection* pSection;
		INIField* pField;
		char writeBuffer[2048]{ 0 };

		for (int i = 0; i < INI_MAX_SECTIONS; i++)
		{
			pSection = &ini[i];
			if (pSection->szSectionName[0] == '\0')
			{	// ran out of sections
				return;
			}

			if (pSection->nNumberFields == 0)
			{	// no fields?
				continue;
			}

			snprintf(writeBuffer, 2048, "[%s]" NL, pSection->szSectionName);
			FS::WritePlaintext(*f, writeBuffer);

			for (int j = 0; j < INI_MAX_FIELDS_PER_SECTION; j++)
			{
				pField = &pSection->fields[j];
				if (pField->szKeyName[0] == '\0')
				{	// ran out of fields for this section
					FS::WritePlaintext(*f, NL);
					break;
				}

				switch (pField->fieldType)
				{
				case INI_STRING:
					snprintf(writeBuffer, 2048, "%s=%s" NL, pField->szKeyName, pField->fieldValues.szValue);
					break;
				case INI_INTEGER:
					snprintf(writeBuffer, 2048, "%s=%i" NL, pField->szKeyName, pField->fieldValues.nValue);
					break;
				case INI_BOOL:
					if (pField->fieldValues.bValue)
					{
						snprintf(writeBuffer, 2048, "%s=true" NL, pField->szKeyName);
					}
					else
					{
						snprintf(writeBuffer, 2048, "%s=false" NL, pField->szKeyName);
					}
					break;
				}

				FS::WritePlaintext(*f, writeBuffer);
			}
		}
	}

	/*
	 *	Add to a file's sections based on data in a structure
	 */
	static void GenerateSectionStructureData(INIFile* pFile, D2CmdArgStrc* Commands, void* pData)
	{
		D2CmdArgStrc* pCurrent;
		INIFile file = *pFile;
		bool bWroteField;

		pCurrent = Commands;
		while (pCurrent->szKeyName[0])
		{
			handle section = FindSection(pFile, pCurrent->szSection);
			Log_ErrorAssert(section != INVALID_HANDLE);
			INIField* pField = &file[section].fields[file[section].nNumberFields];

			if (file[section].szSectionName[0] == '\0')
			{
				D2Lib::strncpyz(file[section].szSectionName, pCurrent->szSection, INI_MAX_SECTION_NAMELEN);
			}

			// DO NOT physically write this field if it varies from the default,
			// -OR- in the case of a string, it is blank!
			bWroteField = true;

			switch (pCurrent->dwType)
			{
			case CMD_BOOLEAN:
				pField->fieldValues.bValue = *(bool*)(pData + pCurrent->nOffset);
				pField->fieldType = INI_BOOL;
				if (pField->fieldValues.bValue == (bool)pCurrent->dwDefault)
				{
					bWroteField = false;
				}
				break;
			case CMD_BYTE:
			case CMD_WORD:
			case CMD_DWORD:
				pField->fieldValues.nValue = *(int*)(pData + pCurrent->nOffset);
				pField->fieldType = INI_INTEGER;
				if (pField->fieldValues.nValue == pCurrent->dwDefault)
				{
					bWroteField = false;
				}
				break;
			case CMD_STRING:
				D2Lib::strncpyz(pField->fieldValues.szValue, (char*)(pData + pCurrent->nOffset), INI_MAX_STRINGLEN);
				pField->fieldType = INI_STRING;
				if (pField->fieldValues.szValue[0] == '\0')
				{
					bWroteField = false;
				}
				break;
			}

			if (bWroteField)
			{
				D2Lib::strncpyz(pField->szKeyName, pCurrent->szKeyName, INI_MAX_KEY_NAMELEN);
				file[section].nNumberFields++;
			}

			pCurrent++;
		}
	}

	/*
	 *	Generates sections based on data in a D2GameConfigStrc and an OpenD2ConfigStrc
	 */
	static void GenerateSections(INIFile* pFile, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
	{
		GenerateSectionStructureData(pFile, CommandArguments, pGameConfig);
		GenerateSectionStructureData(pFile, OpenD2CommandArguments, pOpenConfig);
	}

	/*
	 *	Outputs a config file
	 */
	void WriteConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
	{
		// allocate INI file
		INIFile ini = (INIFile)malloc(sizeof(INISection) * INI_MAX_SECTIONS);
		Log_ErrorAssert(ini != nullptr);
		memset(ini, 0, sizeof(INISection) * INI_MAX_SECTIONS);

		GenerateSections(&ini, pGameConfig, pOpenConfig);
		WriteINIInternal(&ini, f);

		free(ini);
	}

	/*
	 *	Parses a value from the config
	 */
	static void ParseConfigValue(char* szSectionName, char* szKeyName, char* szValueName,
		D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
	{
		D2CmdArgStrc* pCurrent;

		int nIntegerValue = atoi(szValueName);
		bool bBooleanValue;
		if (!D2Lib::stricmp(szValueName, "true"))
		{
			bBooleanValue = true;
		}
		else
		{
			bBooleanValue = false;
		}

		// read from vanilla commandline arguments
		pCurrent = CommandArguments;
		while (pCurrent->szKeyName[0])
		{
			if (!D2Lib::stricmp(pCurrent->szSection, szSectionName) && !D2Lib::stricmp(pCurrent->szKeyName, szKeyName))
			{
				// found the key, now write to it
				switch (pCurrent->dwType)
				{
				case CMD_BOOLEAN:
					*((BYTE*)pGameConfig + pCurrent->nOffset) = bBooleanValue;
					break;
				case CMD_DWORD:
					*(DWORD*)((BYTE*)pGameConfig + pCurrent->nOffset) = nIntegerValue;
					break;
				case CMD_WORD:
					*(WORD*)((BYTE*)pGameConfig + pCurrent->nOffset) = nIntegerValue;
					break;
				case CMD_BYTE:
					*((BYTE*)pGameConfig + pCurrent->nOffset) = nIntegerValue;
					break;
				case CMD_STRING:
					D2Lib::strncpyz((char*)pGameConfig + pCurrent->nOffset, szValueName, 32);
					break;
				}

				return;
			}
			pCurrent++;
		}

		// read from OpenD2 commandline arguments
		pCurrent = OpenD2CommandArguments;
		while (pCurrent->szKeyName[0])
		{
			if (!D2Lib::stricmp(pCurrent->szSection, szSectionName) && !D2Lib::stricmp(pCurrent->szKeyName, szKeyName))
			{
				// found the key, now write to it
				switch (pCurrent->dwType)
				{
				case CMD_BOOLEAN:
					*((BYTE*)pOpenConfig + pCurrent->nOffset) = bBooleanValue;
					break;
				case CMD_DWORD:
					*(DWORD*)((BYTE*)pOpenConfig + pCurrent->nOffset) = nIntegerValue;
					break;
				case CMD_WORD:
					*(WORD*)((BYTE*)pOpenConfig + pCurrent->nOffset) = nIntegerValue;
					break;
				case CMD_BYTE:
					*((BYTE*)pOpenConfig + pCurrent->nOffset) = nIntegerValue;
					break;
				case CMD_STRING:
					D2Lib::strncpyz((char*)pOpenConfig + pCurrent->nOffset, szValueName, 32);
					break;
				}

				return;
			}
			pCurrent++;
		}
	}

	/*
	 *	Reads a config file
	 */
	void ReadConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
	{
		size_t fileSize;
		char* fileBuffer;
		char* readHead;
		char* sectionName;
		char* keyName;
		char* valueName;
		bool bReadingValue = false;

		// get file size
		FS::Seek(*f, 0, FS_SEEK_END);
		fileSize = FS::Tell(*f);
		FS::Seek(*f, 0, FS_SEEK_SET);

		// read the whole file in
		fileBuffer = (char*)malloc(fileSize);
		Log_ErrorAssert(fileBuffer != nullptr);
		FS::Read(*f, fileBuffer, fileSize);

		// parse each file, symbol by symbol
		readHead = fileBuffer;
		keyName = readHead;
		while (*readHead)
		{
			if (*readHead == '[')
			{
				// section start - read until we find a newline or another ] character
				sectionName = readHead + 1;

				while (*readHead && *readHead != ']' && *readHead != '\r' && *readHead != '\n')
				{
					readHead++;
				}
				*readHead = '\0';	// cap off the section name
				readHead++;
				bReadingValue = false;
				keyName = readHead;
			}
			else if (*readHead == ';')
			{
				// read until end of file or newline, whichever is first
				if (bReadingValue)
				{	// parse the value if we found it
					*readHead = '\0';
					ParseConfigValue(sectionName, keyName, valueName, pGameConfig, pOpenConfig);
				}
				bReadingValue = false;

				while (*readHead && *readHead != '\r' && *readHead != '\n')
				{
					readHead++;
				}
				keyName = readHead;
			}
			else if (*readHead == '\r' || *readHead == '\n')
			{
				if (bReadingValue)
				{	// parse the value if we found it
					*readHead = '\0';
					bReadingValue = false;
					ParseConfigValue(sectionName, keyName, valueName, pGameConfig, pOpenConfig);
				}
				readHead++;
				keyName = readHead;
			}
			else if (*readHead == '=')
			{
				*readHead = '\0';
				bReadingValue = true;
				readHead++;
				valueName = readHead;
			}
			else
			{
				readHead++;
			}
		}

		// free out any excess memory that we allocated
		free(fileBuffer);
	}
}
