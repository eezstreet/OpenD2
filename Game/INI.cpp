#include "Diablo2.hpp"

#define INI_MAX_SECTION_NAMELEN		16
#define INI_MAX_KEY_NAMELEN			64
#define INI_MAX_SECTIONS			16
#define INI_MAX_FIELDS_PER_SECTION	64
#define INI_MAX_STRINGLEN			MAX_D2PATH_ABSOLUTE

////////////////////////////////////////////////////
//
//	INI File Parsing
//	This is only ever used for the config files

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
	char			szSectionName[INI_MAX_SECTION_NAMELEN]{ 0 };
	int				nNumberFields;
	INIField		fields[INI_MAX_FIELDS_PER_SECTION]{ 0 };
};

typedef INISection* INIFile;

/*
 *	Finds a free section of an INI
 */
static handle INI_FindSection(INIFile* pFile, char* szSectionName)
{
	INIFile file = *pFile;
	for (int i = 0; i < INI_MAX_SECTIONS; i++)
	{
		if (file[i].szSectionName[0] == '\0' || !D2_stricmp(file[i].szSectionName, szSectionName))
		{
			return (handle)i;
		}
	}
	return INVALID_HANDLE;
}

/*
 *	Writes an INI file
 */
static void INI_WriteINIInternal(INIFile* pFile, fs_handle* f)
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
		FS_WritePlaintext(*f, writeBuffer);

		for (int j = 0; j < INI_MAX_FIELDS_PER_SECTION; j++)
		{
			pField = &pSection->fields[j];
			if (pField->szKeyName[0] == '\0')
			{	// ran out of fields for this section
				FS_WritePlaintext(*f, NL);
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

			FS_WritePlaintext(*f, writeBuffer);
		}
	}
}

/*
 *	Add to a file's sections based on data in a structure
 */
static void INI_GenerateSectionStructureData(INIFile* pFile, D2CmdArgStrc* Commands, void* pData)
{
	D2CmdArgStrc* pCurrent;
	INIFile file = *pFile;
	bool bWroteField;

	pCurrent = Commands;
	while (pCurrent->szKeyName[0])
	{
		handle section = INI_FindSection(pFile, pCurrent->szSection);
		Log_ErrorAssert(section != INVALID_HANDLE);
		INIField* pField = &file[section].fields[file[section].nNumberFields];

		if (file[section].szSectionName[0] == '\0')
		{
			D2_strncpyz(file[section].szSectionName, pCurrent->szSection, INI_MAX_SECTION_NAMELEN);
		}

		// DO NOT physically write this field if it varies from the default,
		// -OR- in the case of a string, it is blank!
		bWroteField = true;

		switch (pCurrent->dwType)
		{
			case CMD_BOOLEAN:
				pField->fieldValues.bValue = *(bool*)((DWORD)pData + pCurrent->nOffset);
				pField->fieldType = INI_BOOL;
				if (pField->fieldValues.bValue == (bool)pCurrent->dwDefault)
				{
					bWroteField = false;
				}
				break;
			case CMD_BYTE:
			case CMD_WORD:
			case CMD_DWORD:
				pField->fieldValues.nValue = *(int*)((DWORD)pData + pCurrent->nOffset);
				pField->fieldType = INI_INTEGER;
				if (pField->fieldValues.nValue == pCurrent->dwDefault)
				{
					bWroteField = false;
				}
				break;
			case CMD_STRING:
				D2_strncpyz(pField->fieldValues.szValue, (char*)((DWORD)pData + pCurrent->nOffset), INI_MAX_STRINGLEN);
				pField->fieldType = INI_STRING;
				if (pField->fieldValues.szValue[0] == '\0')
				{
					bWroteField = false;
				}
				break;
		}

		if (bWroteField)
		{
			D2_strncpyz(pField->szKeyName, pCurrent->szKeyName, INI_MAX_KEY_NAMELEN);
			file[section].nNumberFields++;
		}

		pCurrent++;
	}
}

/*
 *	Generates sections based on data in a D2GameConfigStrc and an OpenD2ConfigStrc
 */
extern D2CmdArgStrc CommandArguments[];
extern D2CmdArgStrc OpenD2CommandArguments[];
static void INI_GenerateSections(INIFile* pFile, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
{
	INI_GenerateSectionStructureData(pFile, CommandArguments, pGameConfig);
	INI_GenerateSectionStructureData(pFile, OpenD2CommandArguments, pOpenConfig);
}

/*
 *	Outputs a config file
 */
void INI_WriteConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
{
	// allocate INI file
	INIFile ini = (INIFile)malloc(sizeof(INISection) * INI_MAX_SECTIONS);
	Log_ErrorAssert(ini != nullptr);
	memset(ini, 0, sizeof(INISection) * INI_MAX_SECTIONS);

	INI_GenerateSections(&ini, pGameConfig, pOpenConfig);
	INI_WriteINIInternal(&ini, f);

	free(ini);
}

/*
 *	Reads a config file
 */
void INI_ReadConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig)
{

}