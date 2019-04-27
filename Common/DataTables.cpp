#include "D2Common.hpp"

D2COMMONAPI D2DataTablesStrc* sgptDataTables;

static D2DataTablesStrc gDataTables{ 0 };

////////////////////////////////////////
//
//	Functions


/*
*	Compiles a .txt into a .bin file
*	@author	eezstreet
*/
void BIN_Compile(const char* szTextName, D2TxtColumnStrc* pColumns, int* nNumberColumns)
{

}

/*
*	Reads a .bin file
*	@author	eezstreet
*/
bool BIN_Read(char* szBinName, void** pDestinationData, size_t* pFileSize)
{
	D2MPQArchive* pArchive = nullptr;
	fs_handle f = engine->MPQ_FindFile(szBinName, nullptr, &pArchive);
	DWORD dwNumRecords = 0;

	if (f == INVALID_HANDLE)
	{
		return false;	// couldn't find it...this is probably a bad thing
	}

	*pFileSize = engine->MPQ_FileSize(pArchive, f);
	Log_ErrorAssertReturn(*pFileSize != 0, false);

	*pDestinationData = malloc(*pFileSize);
	engine->MPQ_ReadFile(pArchive, f, (BYTE*)*pDestinationData, *pFileSize);

	//	Somewhat of a hack here, but the first field in the BIN actually seems to be a DWORD
	//	that specifies how many records in the file.
	//	Needless to say, this will screw everything up if we don't account for it correctly.
	void* offset = (void*)(((BYTE*)*pDestinationData) + 4);
	*pFileSize -= sizeof(DWORD);
	memmove(*pDestinationData, offset, *pFileSize);

	return true;
}

/*
 *	Loads a specific data table
 *	@author	eezstreet
 */
static int DataTables_Load(const char* szDataTableName, void** pDestinationData, 
	D2TxtLinkStrc** pDestinationLink, size_t dwRowSize)
{
	char szPath[MAX_D2PATH]{ 0 };
	size_t dwFileSize = 0;

	// Try and load the BIN file first
	if (!gpConfig->bTXT)
	{
		snprintf(szPath, MAX_D2PATH, "%s%s.bin", D2DATATABLES_DIR, szDataTableName);

		if (BIN_Read(szPath, pDestinationData, &dwFileSize))
		{	// found the BIN file
			// we don't need to fill out the rest of the linker, it's just for .txt files
			return dwFileSize / dwRowSize;
		}
	}
	

	// If that doesn't work (or we have -txt mode enabled) we need to compile the BIN itself from .txt
	snprintf(szPath, MAX_D2PATH, "%s%s.txt", D2DATATABLES_DIR, szDataTableName);
	return 0;
}

/*
 *	Load all of the data tables
 *	@author	eezstreet
 */
void DataTables_Init()
{
	sgptDataTables = &gDataTables;

	//	Levels
	// levels.bin and leveldef.bin are special in that they are created from levels.txt
	// Therefore, they will share the same record count.
	// (and therefore, the count doesn't need to be stored in sgptDataTables
	sgptDataTables->nAutoMap = DataTables_Load("automap", (void**)&sgptDataTables->pAutoMap, nullptr, sizeof(D2AutomapTxt)); //Should be nAutomapTxt and pAutoMapTxt?
	sgptDataTables->nLevelsTxtRecordCount = DataTables_Load("levels", (void**)&sgptDataTables->pLevelsTxt, nullptr, sizeof(D2LevelsTxt));
	DataTables_Load("leveldefs", (void**)&sgptDataTables->pLevelDefBin, nullptr, sizeof(D2LevelDefBin));
	sgptDataTables->nLvlTypesTxtRecordCount = DataTables_Load("lvltypes", (void**)&sgptDataTables->pLvlTypesTxt, nullptr, sizeof(D2LvlTypesTxt));
	sgptDataTables->nLvlSubTxtRecordCount = DataTables_Load("lvlsub", (void**)&sgptDataTables->pLvlSubTxt, nullptr, sizeof(D2LvlSubTxt));
	sgptDataTables->nLvlWarpTxtRecordCount = DataTables_Load("lvlwarp", (void**)&sgptDataTables->pLvlWarpTxt, nullptr, sizeof(D2LvlWarpTxt));
	sgptDataTables->nLvlMazeTxtRecordCount = DataTables_Load("lvlmaze", (void**)&sgptDataTables->pLvlMazeTxt, nullptr, sizeof(D2LvlMazeTxt));
	sgptDataTables->nLvlPrestTxtRecordCount = DataTables_Load("lvlprest", (void**)&sgptDataTables->pLvlPrestTxt, nullptr, sizeof(D2LvlPrestTxt));
	sgptDataTables->nObjectsTxtRecordCount = DataTables_Load("objects", (void**)&sgptDataTables->pObjectsTxt, nullptr, sizeof(D2ObjectsTxt));
	sgptDataTables->nObjGroupTxtRecordCount = DataTables_Load("objgroup", (void**)&sgptDataTables->pObjGroupTxt, nullptr, sizeof(D2ObjGroupTxt));
	sgptDataTables->nObjModeTypeTxtRecordCount = DataTables_Load("objmode", (void**)&sgptDataTables->pObjModeTypeTxt, nullptr, sizeof(D2ObjModeTypeTxt)); //what is pObjMode ?
	//DataTables_Load("objtype", (void**)&sgptDataTables->pObjType, nullptr, sizeof(D2ObjModeTypeTxt));
	sgptDataTables->nShrinesTxtRecordCount = DataTables_Load("shrines", (void**)&sgptDataTables->pShrinesTxt, nullptr, sizeof(D2ShrinesTxt));

	//Character Data
	DataTables_Load("bodylocs", (void**)&sgptDataTables->pBodyLocsTxt, &sgptDataTables->pBodyLocsLinker, sizeof(D2BodyLocsTxt));
	DataTables_Load("experience", (void**)&sgptDataTables->pExperienceTxt, nullptr, sizeof(D2ExperienceTxt));
	sgptDataTables->nCharStatsTxtRecordCount = DataTables_Load("charstats", (void**)&sgptDataTables->pCharStatsTxt, nullptr, sizeof(D2CharStatsTxt));
	sgptDataTables->nInventoryTxtRecordCount = DataTables_Load("inventory", (void**)&sgptDataTables->pInventoryTxt, nullptr, sizeof(D2InventoryTxt));
	DataTables_Load("playerclass", (void**)&sgptDataTables->pPlayerClassTxt, &sgptDataTables->pPlayerClassLinker, sizeof(D2PlayerClassTxt));
	sgptDataTables->nPlrModeTypeTxtRecordCount = DataTables_Load("plrmode", (void**)&sgptDataTables->pPlayerMode, nullptr, sizeof(D2PlrModeTypeTxt));
	DataTables_Load("plrtype", (void**)&sgptDataTables->pPlayerType, nullptr, sizeof(D2PlrModeTypeTxt));

	//NPC
	sgptDataTables->nHirelingTxtRecordCount = DataTables_Load("hireling", (void**)&sgptDataTables->pHirelingTxt, nullptr, sizeof(D2HireDescTxt));
	sgptDataTables->nNpcTxtRecordCount = DataTables_Load("npc", (void**)&sgptDataTables->pNpcTxt, nullptr, sizeof(D2NpcTxt));
	DataTables_Load("storepage", (void**)&sgptDataTables->pStorePageTxt, &sgptDataTables->pStorePageLinker, sizeof(D2StorePageTxt));
	//sgptDataTables->nGambleTxtRecordCount = DataTables_Load("gamble", (void**)&sgptDataTables->? , nullptr, sizeof(D2GambleTxt));

	//Monser Data
	sgptDataTables->nMonAiTxtRecordCount = DataTables_Load("monai", (void**)&sgptDataTables->pMonAiTxt, &sgptDataTables->pMonAiLinker, sizeof(D2MonAiTxt));
	sgptDataTables->nMonEquipTxtRecordCount = DataTables_Load("monequip", (void**)&sgptDataTables->pMonEquipTxt, nullptr, sizeof(D2MonEquipTxt));
	sgptDataTables->nMonLvlTxtRecordCount = DataTables_Load("monlvl", (void**)&sgptDataTables->pMonLvlTxt, nullptr, sizeof(D2MonLvlTxt));
	sgptDataTables->nMonModeTxtRecordCount = DataTables_Load("monmode", (void**)&sgptDataTables->pMonMode, &sgptDataTables->pMonModeLinker, sizeof(D2MonModeTxt));
	sgptDataTables->nMonPlaceTxtRecordCount = DataTables_Load("monplace", (void**)&sgptDataTables->pMonPlaceTxt, &sgptDataTables->pMonPlaceLinker, sizeof(D2MonPlaceTxt));
	//sgptDataTables->nMonPresetTxtActRecordCounts = DataTables_Load("monpreset", (void**)&sgptDataTables->pMonPresetTxt, nullptr, sizeof(D2MonPresetTxt));
	sgptDataTables->nMonPropTxtRecordCount = DataTables_Load("monprop", (void**)&sgptDataTables->pMonPropTxt, &sgptDataTables->pMonPropLinker, sizeof(D2MonPropTxt));
	sgptDataTables->nMonSeqTxtRecordCount = DataTables_Load("monseq", (void**)&sgptDataTables->pMonSeqTxt, &sgptDataTables->pMonSeqLinker, sizeof(D2MonSeqTxt));
	sgptDataTables->nMonSoundsTxtRecordCount = DataTables_Load("monsounds", (void**)&sgptDataTables->pMonSoundsTxt, &sgptDataTables->pMonSoundsLinker, sizeof(D2MonSoundsTxt));
	sgptDataTables->nMonStatsTxtRecordCount = DataTables_Load("monstats", (void**)&sgptDataTables->pMonStatsTxt, &sgptDataTables->pMonStatsLinker, sizeof(D2MonStatsTxt));
	sgptDataTables->nMonStats2TxtRecordCount = DataTables_Load("monstats2", (void**)&sgptDataTables->pMonStats2Txt, &sgptDataTables->pMonStatsLinker, sizeof(D2MonStats2Txt));
	sgptDataTables->nMonUModTxtRecordCount = DataTables_Load("monumod", (void**)&sgptDataTables->pMonUModTxt, &sgptDataTables->pMonUModLinker, sizeof(D2MonUModTxt));
	sgptDataTables->nMonTypeTxtRecordCount = DataTables_Load("montype", (void**)&sgptDataTables->pMonTypeTxt, &sgptDataTables->pMonTypeLinker, sizeof(D2MonTypeTxt));
	sgptDataTables->nMissilesTxtRecordCount = DataTables_Load("missiles", (void**)&sgptDataTables->pMissilesTxt, &sgptDataTables->pMissilesLinker, sizeof(D2MissilesTxt));
	DataTables_Load("misscalc", (void**)&sgptDataTables->pMissileCalcTxt, &sgptDataTables->pMissileCalcLinker, sizeof(D2MissCalcTxt));
	sgptDataTables->nPetTypeTxtRecordCount = DataTables_Load("pettype", (void**)&sgptDataTables->pPetTypeTxt, &sgptDataTables->pPetTypeLinker, sizeof(D2PetTypeTxt));
	sgptDataTables->nUniqueAppellationTxtRecordCount = DataTables_Load("uniqueappellation", (void**)&sgptDataTables->pUniqueAppellationTxt, nullptr, sizeof(D2UniqueAppellationTxt));
	sgptDataTables->nUniquePrefixTxtRecordCount = DataTables_Load("uniqueprefix", (void**)&sgptDataTables->pUniquePrefixTxt, nullptr, sizeof(D2UniquePrefixTxt));
	sgptDataTables->nUniqueSuffixTxtRecordCount = DataTables_Load("uniquesuffix", (void**)&sgptDataTables->pUniqueSuffixTxt, nullptr, sizeof(D2UniqueSuffixTxt));
	sgptDataTables->nSuperUniquesTxtRecordCount = DataTables_Load("superuniques", (void**)&sgptDataTables->pSuperUniquesTxt, &sgptDataTables->pSuperUniquesLinker, sizeof(D2SuperUniquesTxt));
	sgptDataTables->nTreasureClassEx = DataTables_Load("treasureclassex", (void**)&sgptDataTables->pTreasureClassEx, &sgptDataTables->pTreasureClassExLinker, sizeof(D2TreasureClassExTxt));

	//Items
	DataTables_Load("automagic", (void**)&sgptDataTables->pAutoMagic, nullptr, sizeof(D2MagicAffixTxt));
	sgptDataTables->nBooksTxtRecordCount = DataTables_Load("books", (void**)&sgptDataTables->pBooksTxt, nullptr, sizeof(D2BooksTxt));
	sgptDataTables->nItemRatioTxtRecordCount = DataTables_Load("itemratio", (void**)&sgptDataTables->pItemRatioTxt, nullptr, sizeof(D2ItemRatioTxt));
	sgptDataTables->nItemStatCostTxtRecordCount = DataTables_Load("itemstatcost", (void**)&sgptDataTables->pItemStatCostTxt, &sgptDataTables->pItemStatCostLinker, sizeof(D2ItemStatCostTxt));
	sgptDataTables->nItemTypesTxtRecordCount = DataTables_Load("itemtypes", (void**)&sgptDataTables->pItemTypesTxt, &sgptDataTables->pItemTypesLinker, sizeof(D2ItemTypesTxt));
	sgptDataTables->nGemsTxtRecordCount = DataTables_Load("gems", (void**)&sgptDataTables->pGemsTxt, nullptr, sizeof(D2GemsTxt));
	sgptDataTables->nRunesTxtRecordCount = DataTables_Load("runes", (void**)&sgptDataTables->pRunesTxt, &sgptDataTables->pRunesLinker, sizeof(D2RunesTxt));
	sgptDataTables->nPropertiesTxtRecordCount = DataTables_Load("properties", (void**)&sgptDataTables->pPropertiesTxt, &sgptDataTables->pPropertiesLinker, sizeof(D2PropertiesTxt));
	sgptDataTables->nQualityItemsTxtRecordCount = DataTables_Load("qualityitems", (void**)&sgptDataTables->pQualityItemsTxt, nullptr, sizeof(D2QualityItemsTxt)); 
	DataTables_Load("magicprefix", (void**)&sgptDataTables->pMagicPrefix, nullptr, sizeof(D2MagicAffixTxt));
	DataTables_Load("magicsuffix", (void**)&sgptDataTables->pMagicSuffix, nullptr, sizeof(D2MagicAffixTxt));
	DataTables_Load("rareprefix", (void**)&sgptDataTables->pRarePrefix, nullptr, sizeof(D2RareAffixTxt));
	DataTables_Load("raresuffix", (void**)&sgptDataTables->pRareSuffix, nullptr, sizeof(D2RareAffixTxt));
	sgptDataTables->nUniqueItemsTxtRecordCount = DataTables_Load("uniqueitems", (void**)&sgptDataTables->pUniqueItemsTxt, &sgptDataTables->pUniqueItemsLinker, sizeof(D2UniqueItemsTxt));
	sgptDataTables->nSetItemsTxtRecordCount = DataTables_Load("setitems", (void**)&sgptDataTables->pSetItemsTxt, nullptr, sizeof(D2SetItemsTxt));
	sgptDataTables->nSetsTxtRecordCount = DataTables_Load("sets", (void**)&sgptDataTables->pSetsTxt, &sgptDataTables->pSetsLinker, sizeof(D2SetsTxt));
	DataTables_Load("armtype", (void**)&sgptDataTables->pArmTypeTxt, nullptr, sizeof(D2ArmTypeTxt));
	sgptDataTables->nArmorTxtRecordCount = DataTables_Load("armor", (void**)&sgptDataTables->pArmor, nullptr, sizeof(D2ItemsTxt));
	sgptDataTables->nWeaponsTxtRecordCount = DataTables_Load("weapons", (void**)&sgptDataTables->pWeapons, nullptr, sizeof(D2ItemsTxt));
	sgptDataTables->nMiscTxtRecordCount = DataTables_Load("misc", (void**)&sgptDataTables->pMisc, nullptr, sizeof(D2ItemsTxt));
	sgptDataTables->nLowQualityItemsTxtRecordCount = DataTables_Load("lowqualityitems", (void**)&sgptDataTables->pLowQualityItemsTxt, nullptr, sizeof(D2LowQualityItemsTxt));
	//DataTables_Load("weaponclass", (void**)&sgptDataTables->pWeaponsClassTxt, nullptr, sizeof(D2WeaponClassTxt)) //pWeaponClass and nWeaponClassRecordCount are missing? 
	sgptDataTables->nCubeMainTxtRecordCount = DataTables_Load("cubemain", (void**)&sgptDataTables->pCubeMainTxt, nullptr, sizeof(D2CubeMainTxt));
	

	//Common
	DataTables_Load("colors", (void**)&sgptDataTables->pColorsTxt, &sgptDataTables->pColorsLinker, sizeof(D2ColorsTxt));
	sgptDataTables->nCompCodeTxtRecordCount = DataTables_Load("compcode", (void**)&sgptDataTables->pCompCodeTxt, &sgptDataTables->pCompCodeLinker, sizeof(D2CompCodeTxt));
	DataTables_Load("composit", (void**)&sgptDataTables->pCompositTxt, nullptr, sizeof(D2CompositTxt));
	sgptDataTables->nDifficultyLevelsTxtRecordCount = DataTables_Load("difficultylevels", (void**)&sgptDataTables->pDifficultyLevelsTxt, nullptr, sizeof(D2DifficultyLevelsTxt));
	DataTables_Load("events", (void**)&sgptDataTables->pEventsTxt, &sgptDataTables->pEventsLinker, sizeof(D2EventsTxt));
	sgptDataTables->nOverlayTxtRecordCount = DataTables_Load("overlay", (void**)&sgptDataTables->pOverlayTxt, &sgptDataTables->pOverlayLinker, sizeof(D2OverlayTxt));
	//sgptDataTables->nSoundsTxtCodes = DataTables_Load("sounds", (void**)&sgptDataTables->pSoundsTxt, &sgptDataTables->pSoundsLinker, sizeof(D2SoundsTxtStub));
	//DataTables_Load("soundenviron", (void**)&sgptDataTables->pSoundEnvironTxt, null, sizeof(D2SoundEnviroTxt));
	sgptDataTables->nSkillsTxtRecordCount = DataTables_Load("skills", (void**)&sgptDataTables->pSkillsTxt, &sgptDataTables->pSkillsLinker, sizeof(D2SkillsTxt));
	sgptDataTables->nSkillDescTxtRecordCount = DataTables_Load("skilldesc", (void**)&sgptDataTables->pSkillDescTxt, &sgptDataTables->pSkillDescLinker, sizeof(D2SkillDescTxt));
	DataTables_Load("skillcalc", (void**)&sgptDataTables->pSkillCalcTxt, &sgptDataTables->pSkillCalcLinker, sizeof(D2SkillCalcTxt));
	sgptDataTables->nStatesTxtRecordCount = DataTables_Load("states", (void**)&sgptDataTables->pStatesTxt, &sgptDataTables->pStatesLinker, sizeof(D2StatesTxt));
	
}

/*
 *	Delete all of the data tables
 *	@author	eezstreet
 */
void DataTables_Free()
{
	//Levels
	free(sgptDataTables->pAutoMap);
	free(sgptDataTables->pLevelsTxt);
	free(sgptDataTables->pLevelDefBin); 
	free(sgptDataTables->pLvlTypesTxt);
	free(sgptDataTables->pLvlSubTxt);
	free(sgptDataTables->pLvlWarpTxt);
	free(sgptDataTables->pLvlMazeTxt);
	free(sgptDataTables->pLvlPrestTxt);
	free(sgptDataTables->pObjectsTxt);
	free(sgptDataTables->pObjGroupTxt);
	free(sgptDataTables->pObjModeTypeTxt);
	//free(sgptDataTables->pObjType);
	free(sgptDataTables->pShrinesTxt);
	//Character
	free(sgptDataTables->pBodyLocsTxt);
	free(sgptDataTables->pExperienceTxt);
	free(sgptDataTables->pCharStatsTxt);
	free(sgptDataTables->pInventoryTxt);
	free(sgptDataTables->pPlayerClassTxt);
	free(sgptDataTables->pPlayerMode);
	free(sgptDataTables->pPlayerType);
	//NPC
	free(sgptDataTables->pHirelingTxt);
	free(sgptDataTables->pNpcTxt);
	free(sgptDataTables->pStorePageTxt);
	//Monsters
	free(sgptDataTables->pMonAiTxt);
	free(sgptDataTables->pMonEquipTxt);
	free(sgptDataTables->pMonLvlTxt);
	free(sgptDataTables->pMonModeTxt);
	free(sgptDataTables->pMonPlaceTxt);
	free(sgptDataTables->pMonPresetTxt);
	free(sgptDataTables->pMonPropTxt);
	free(sgptDataTables->pMonSeqTxt);
	free(sgptDataTables->pMonSoundsTxt);
	free(sgptDataTables->pMonStatsTxt);
	free(sgptDataTables->pMonStats2Txt);
	free(sgptDataTables->pMonUModTxt);
	free(sgptDataTables->pMonTypeTxt);
	free(sgptDataTables->pMissilesTxt);
	free(sgptDataTables->pMissileCalcTxt);
	free(sgptDataTables->pPetTypeTxt);
	free(sgptDataTables->pUniqueAppellationTxt);
	free(sgptDataTables->pUniquePrefixTxt);
	free(sgptDataTables->pUniqueSuffixTxt);
	free(sgptDataTables->pSuperUniquesTxt);
	free(sgptDataTables->pTreasureClassEx);
	//Items
	free(sgptDataTables->pAutoMagic);
	free(sgptDataTables->pBooksTxt);
	free(sgptDataTables->pItemRatioTxt);
	free(sgptDataTables->pItemStatCostTxt);
	free(sgptDataTables->pItemTypesTxt);
	free(sgptDataTables->pGemsTxt);
	free(sgptDataTables->pRunesTxt);
	free(sgptDataTables->pPropertiesTxt);
	free(sgptDataTables->pQualityItemsTxt);
	free(sgptDataTables->pMagicPrefix);
	free(sgptDataTables->pMagicSuffix);
	free(sgptDataTables->pRarePrefix);
	free(sgptDataTables->pRareSuffix);
	free(sgptDataTables->pUniqueItemsTxt);
	free(sgptDataTables->pSetItemsTxt);
	free(sgptDataTables->pSetsTxt);
	free(sgptDataTables->pArmTypeTxt);
	free(sgptDataTables->pArmor);
	free(sgptDataTables->pWeapons);
	free(sgptDataTables->pMisc);
	free(sgptDataTables->pLowQualityItemsTxt);
	//free(sgptDataTables->pWeaponsClassTxt);
	free(sgptDataTables->pCubeMainTxt);
	//Common
	free(sgptDataTables->pColorsTxt);
	free(sgptDataTables->pCompCodeTxt);
	free(sgptDataTables->pCompositTxt);
	free(sgptDataTables->pDifficultyLevelsTxt);
	free(sgptDataTables->pEventsTxt);
	free(sgptDataTables->pOverlayTxt);
	//free(sgptDataTables->pSoundsTxt);
	//free(sgptDataTables->pSoundEnvironTxt);
	free(sgptDataTables->pSkillsTxt);
	free(sgptDataTables->pSkillDescTxt);
	free(sgptDataTables->pSkillCalcTxt);
	free(sgptDataTables->pStatesTxt);
}

