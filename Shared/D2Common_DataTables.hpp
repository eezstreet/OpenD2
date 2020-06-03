#pragma once

#include "D2Common_Shared.hpp"

#pragma pack(1)

/////////////////////////////////////////////////
//
//	Forward Declarations

struct D2MonSeqTxt;

/////////////////////////////////////////////////
//
//	Linker Structures

struct D2TxtLinkNodeStrc
{
	char szText[32];				//0x00
	int nLinkIndex;					//0x20
	D2TxtLinkNodeStrc* pPrevious;	//0x24
	D2TxtLinkNodeStrc* pNext;		//0x28
};

struct D2TxtLinkStrc
{
	int nRecords;					//0x00
	DWORD dw1;						//0x04
	DWORD dw2;						//0x08
	D2TxtLinkNodeStrc* pFirstNode;	//0x0C
};


//////////////////////////////////////////////////
//
//	BIN Structures

struct D2AutomapRandStrc
{
	int nStart;							//0x00
	int nEnd;							//0x04
};

struct D2AutomapShortStrc
{
	DWORD dwAutomapLevelType;			//0x00
	DWORD dwAutomapTileType;			//0x04
	BYTE nStyle;						//0x08
	BYTE nStartSequence;				//0x09
	BYTE nEndSequence;					//0x0A
	BYTE pad0x0B;						//0x0B
	DWORD dwCel[4];						//0x0C
	int nCels;							//0x1C
};

struct D2CharItemStrc
{
	DWORD dwItemCode;					//0x00
	BYTE nBodyLoc;						//0x04
	char nItemCount;					//0x05
	WORD pad0x06;						//0x06
};

struct D2CubeInputItem
{
	WORD wInputFlags;					//0x00
	WORD wItem;							//0x02
	WORD wItemID;						//0x04
	BYTE nQuality;						//0x06
	BYTE nQuantity;						//0x07
};

struct D2CubeOutputParamStrc
{
	int nMod;							//0x00
	WORD nModParam;						//0x04
	short nModMin;						//0x06
	short nModMax;						//0x08
	WORD nModChance;					//0x0A
};

struct D2CubeOutputItem
{
	WORD wItemFlags;					//0x00
	WORD wBaseItemId;					//0x02
	WORD wItemID;						//0x04
	BYTE nQuality;						//0x06
	BYTE nQuantity;						//0x07
	BYTE nType;							//0x08
	BYTE nLvl;							//0x09
	BYTE nPLvl;							//0x0A
	BYTE nILvl;							//0x0B
	WORD wPrefixId[3];					//0x0C
	WORD wSuffixId[3];					//0x12
	D2CubeOutputParamStrc pParam[5];	//0x18
};

struct D2InvCompGridStrc
{
	int nLeft;							//0x00
	int nRight;							//0x04
	int nTop;							//0x08
	int nBottom;						//0x0C
	BYTE nWidth;						//0x10
	BYTE nHeight;						//0x11
	WORD pad0x12;						//0x12
};

struct D2InventoryGridInfoStrc
{
	BYTE nGridX;							//0x00
	BYTE nGridY;							//0x01
	WORD pad0x02;							//0x02
	int nGridLeft;							//0x04
	int nGridRight;							//0x08
	int nGridTop;							//0x0C
	int nGridBottom;						//0x10
	BYTE nGridBoxWidth;						//0x14
	BYTE nGridBoxHeight;					//0x15
	WORD pad0x16;							//0x16
};

struct D2InvRectStrc
{
	int nLeft;							//0x00
	int nRight;							//0x04
	int nTop;							//0x08
	int nBottom;						//0x0C
};

struct D2ItemStatCostDescStrc
{
	WORD nRecordId;						//0x00
	short nDescPriority;				//0x02
};

struct D2OpStatDataStrc
{
	WORD nOpBase;						//0x00
	WORD nStat;						//0x02
	BYTE nOp;							//0x04
	BYTE nOpParam;						//0x05
};

//TODO: Find a better name - Could be DRLGFile posted by Necrolis
struct D2UnkLvlSubTypeStrc
{
	int unk0x00;						//0x00
	void* unk0x04;						//0x04
	BYTE unk0x08[4];					//0x08
	int unk0x0C;						//0x0C
	int unk0x10;						//0x10
	int unk0x14;						//0x14
	int unk0x18;						//0x18
	BYTE unk0x1C[16];					//0x1C
	int unk0x2C;						//0x2C
	BYTE unk0x30[12];					//0x30
	int unk0x3C;						//0x3C
	BYTE unk0x40[4];					//0x40
	int unk0x44;						//0x44
	int unk0x48;						//0x48
	int nClusters;						//0x4C
	void* unk0x50;						//0x50
	void* unk0x54;						//0x54
	int unk0x58;						//0x58
};

//TODO: Find a better name - Called ptTileLibrary by D2CMP.#10087
struct D2UnkLvlDataTableStrc
{
	BYTE unk0x00[128][32];
};

struct D2PropertyStrc
{
	int nProperty;						//0x00
	int nLayer;							//0x04
	int nMin;							//0x08
	int nMax;							//0x0C
};

struct D2SeqRecordStrc
{
	D2MonSeqTxt* pMonSeqTxtRecord;		//0x00
	int unk0x04;						//0x04
	int unk0x08;						//0x08
};

//TODO: Find a better name
struct D2UnkSequenceStrc
{
	D2SeqRecordStrc unk0x00[14];		//0x00
};

struct D2TCExInfoStrc
{
	int nClassic;
	int nProb;
	short nItemId;
	short nTxtRow;
	BYTE nFlags;
	char unk0x0D;
	short unk0x0E;
	short unk0x10;
	short unk0x12;
	short unk0x14;
	short unk0x16;
	short unk0x18;
	short unk0x1A;
};

struct D2TCExShortStrc
{
	short nGroup;							//0x00
	short nLevel;							//0x02
	int nTypes;								//0x04
	int nClassic;							//0x08
	int nProb;								//0x0C
	int nPicks;								//0x10
	int nNoDrop;							//0x14
	short unk0x18;							//0x18
	short nMagic;							//0x1A
	short nRare;							//0x1C
	short nSet;								//0x1E
	short nUnique;							//0x20
	short nSuperior;						//0x22
	short nNormal;							//0x24
	short unk0x26;							//0x26
	D2TCExInfoStrc* pInfo;					//0x28
};


//DATATABLES

struct D2ArenaTxt
{
	DWORD dwSuicide;					//0x00
	DWORD dwPlayerKill;					//0x04
	DWORD dwPlayerKillPercent;			//0x08
	DWORD dwMonsterKill;				//0x0C
	DWORD dwPlayerDeath;				//0x10
	DWORD dwPlayerDeathPercent;			//0x14
	DWORD dwMonsterDeath;				//0x18
};

struct D2ArmTypeTxt
{
	char szName[32];					//0x00
	char szToken[20];					//0x20
};

struct D2AutomapTxt
{
	char szLevelName[16];				//0x00
	char szTileName[8];					//0x10
	BYTE nStyle;						//0x18
	BYTE nStartSequence;				//0x19
	BYTE nEndSequence;					//0x1A
	BYTE pad0x1B;						//0x1B
	DWORD dwCel[4];						//0x1C
};

struct D2BeltsTxt
{
	int nIndex;							//0x00
	int nBoxes;							//0x04
	D2InvRectStrc beltBoxes[16];		//0x08
};

struct D2BodyLocsTxt
{
	DWORD dwCode;						//0x00
};

struct D2BooksTxt
{
	WORD wStr;							//0x00
	BYTE nSpellIcon;					//0x02
	BYTE pad0x03;						//0x03
	DWORD dwPSpell;						//0x04
	DWORD dwScrollSkillId;				//0x08
	DWORD dwBookSkillId;				//0x0C
	DWORD dwBaseCost;					//0x10
	DWORD dwCostPerCharge;				//0x14
	DWORD dwScrollSpellCode;			//0x18
	DWORD dwBookSpellCode;				//0x1C
};

struct D2CharStatsTxt
{
	wchar_t wszClassName[16];			//0x00
	char szClassName[16];				//0x20
	BYTE nStr;							//0x30
	BYTE nDex;							//0x31
	BYTE nInt;							//0x32
	BYTE nVit;							//0x33
	BYTE nStamina;						//0x34
	BYTE nLifeAdd;						//0x35
	BYTE nPercentStr;					//0x36
	BYTE nPercentInt;					//0x37
	BYTE nPercentDex;					//0x38
	BYTE nPercentVit;					//0x39
	BYTE nManaRegen;					//0x3A
	BYTE pad0x3B;						//0x3B
	DWORD dwToHitFactor;				//0x3C
	BYTE nWalkSpeed;					//0x40
	BYTE nRunSpeed;						//0x41
	BYTE nRunDrain;						//0x42
	BYTE nLifePerLevel;					//0x43
	BYTE nStaminaPerLevel;				//0x44
	BYTE nManaPerLevel;					//0x45
	BYTE nLifePerVitality;				//0x46
	BYTE nStaminaPerVitality;			//0x47
	BYTE nManaPerMagic;					//0x48
	BYTE nBlockFactor;					//0x49
	WORD pad0x4A;						//0x4A
	DWORD dwBaseWClass;					//0x4C
	BYTE nStatsPerLevel;				//0x50
	BYTE pad0x51;						//0x51
	WORD wStrAllSkills;					//0x52
	WORD wStrSkillTab[3];				//0x54
	WORD wStrClassOnly;					//0x5A
	D2CharItemStrc pItems[10];			//0x5C
	WORD wStartSkill;					//0xAC
	WORD wBaseSkill[10];				//0xAE
	WORD pad0xC2;						//0xC2
};

struct D2CharTemplateTxt
{
	char szName[30];					//0x00
	BYTE nClass;						//0x1E
	BYTE nLevel;						//0x1F
	BYTE nAct;							//0x20
	BYTE nStr;							//0x21
	BYTE nDex;							//0x22
	BYTE nInt;							//0x23
	BYTE nVit;							//0x24
	BYTE nMana;							//0x25
	BYTE nHitpoints;					//0x26
	BYTE nManaRegenBonus;				//0x27
	BYTE nVelocity;						//0x28
	BYTE nAttackRate;					//0x29
	BYTE nOtherRate;					//0x2A
	BYTE pad0x2B;						//0x2B
	DWORD dwRightSkill;					//0x2C
	DWORD dwSkill[9];					//0x30
	DWORD dwSkillLevel[9];				//0x54
	D2CharItemStrc pItems[15];			//0x78
};

struct D2ColorsTxt
{
	DWORD dwCode;						//0x00
};

struct D2CompCodeTxt
{
	DWORD dwCode;						//0x00
};

struct D2CompositTxt
{
	char szName[32];					//0x00
	char szToken[20];					//0x20
};

struct D2CubeMainTxt
{
	BYTE nEnabled;						//0x00
	BYTE nLadder;						//0x01
	BYTE nMinDiff;						//0x02
	BYTE nClass;						//0x03
	BYTE nOp;							//0x04
	BYTE pad0x05[3];					//0x05
	int nParam;							//0x08
	int nValue;							//0x0C
	char nNumInputs;					//0x10
	BYTE pad0x11;						//0x11
	WORD wVersion;						//0x12
	D2CubeInputItem pInputItem[7];		//0x14
	D2CubeOutputItem pOutputItem[3];	//0x4C
};

struct D2DifficultyLevelsTxt
{
	DWORD dwResistPenalty;				//0x00
	DWORD dwDeathExpPenalty;			//0x04
	DWORD dwUberCodeOddsNorm;			//0x08
	DWORD dwUberCodeOddsGood;			//0x0C
	DWORD dwMonsterSkillBonus;			//0x10
	DWORD dwMonsterFreezeDiv;			//0x14
	DWORD dwMonsterColdDiv;				//0x18
	DWORD dwAiCurseDiv;					//0x1C
	DWORD dwUltraCodeOddsNorm;			//0x20
	DWORD dwUltraCodeOddsGood;			//0x24
	DWORD dwLifeStealDiv;				//0x28
	DWORD dwManaStealDiv;				//0x2C
	DWORD dwUniqueDmgBonus;				//0x30
	DWORD dwChampionDmgBonus;			//0x34
	DWORD dwHireableBossDmgPercent;		//0x38
	DWORD dwMonsterCEDmgPercent;		//0x3C
	DWORD dwStaticFieldMin;				//0x40
	DWORD dwGambleRare;					//0x44
	DWORD dwGambleSet;					//0x48
	DWORD dwGambleUniq;					//0x4C
	DWORD dwGambleUber;					//0x50
	DWORD dwGambleUltra;				//0x54
};

struct D2ElemTypesTxt
{
	DWORD dwCode;						//0x00
};

struct D2EventsTxt
{
	WORD wEvent;						//0x00
};

struct D2ExperienceTxt
{
	DWORD dwClass[7];					//0x00
	DWORD dwExpRatio;					//0x1C
};

struct D2GambleTxt
{
	DWORD dwItemCode;					//0x00
	int nLevel;							//0x04
	int nItemId;						//0x08
										//char szItemCode[12];				//0x00
};

struct D2GemsTxt
{
	char szName[32];					//0x00
	char szLetter[8];					//0x20
	union
	{
		DWORD dwItemCode;					//0x28
		char szItemCode[4];					//0x28
	};
	WORD wStringId;						//0x2C
	BYTE nNumMods;						//0x2E
	BYTE nTransForm;					//0x2F
	D2PropertyStrc pProperties[3][3];	//0x30
};

struct D2HireDescTxt
{
	DWORD dwCode;						//0x00
};

struct D2HirelingTxt
{
	WORD wVersion;						//0x00
	WORD pad0x02;						//0x02
	int nId;							//0x04
	DWORD dwClass;						//0x08
	DWORD dwAct;						//0x0C
	DWORD dwDifficulty;					//0x10
	DWORD dwSeller;						//0x14
	DWORD dwGold;						//0x18
	DWORD dwLevel;						//0x1C
	DWORD dwExpPerLvl;					//0x20
	DWORD dwHitpoints;					//0x24
	DWORD dwHitpointsPerLvl;			//0x28
	DWORD dwDefense;					//0x2C
	DWORD dwDefensePerLvl;				//0x30
	DWORD dwStr;						//0x34
	DWORD dwStrPerLvl;					//0x38
	DWORD dwDex;						//0x3C
	DWORD dwDexPerLvl;					//0x40
	DWORD dwAttackRate;					//0x44
	DWORD dwAttackRatePerLvl;			//0x48
	DWORD dwShare;						//0x4C
	DWORD dwDmgMin;						//0x50
	DWORD dwDmgMax;						//0x54
	DWORD dwDmgPerLvl;					//0x58
	DWORD dwResist;						//0x5C
	DWORD dwResistPerLvl;				//0x60
	DWORD dwDefaultChance;				//0x64
	DWORD dwHead;						//0x68
	DWORD dwTorso;						//0x6C
	DWORD dwWeapon;						//0x70
	DWORD dwShield;						//0x74
	DWORD dwSkill[6];					//0x78
	DWORD dwChance[6];					//0x90
	DWORD dwChancePerLvl[6];			//0xA8
	BYTE nMode[6];						//0xC0
	BYTE nLevel[6];						//0xC6
	BYTE nSLvlPerLvl[6];				//0xCC
	BYTE nHireDesc;						//0xD2
	char szNameFirst[32];				//0xD3
	char szNameLast[32];				//0xF3
	BYTE pad0x113;						//0x113
	WORD wNameFirst;					//0x114
	WORD wNameLast;						//0x116
};

struct D2HitClassTxt
{
	DWORD dwCode;						//0x00
};

struct D2InventoryTxt
{
	D2InvRectStrc pRect;				//0x00
	D2InventoryGridInfoStrc pGridInfo;	//0x10
	D2InvCompGridStrc pComponents[10];	//0x28
};

struct D2ItemRatioTxt
{
	DWORD dwUnique;						//0x00
	DWORD dwUniqueDivisor;				//0x04
	DWORD dwUniqueMin;					//0x08
	DWORD dwRare;						//0x0C
	DWORD dwRareDivisor;				//0x10
	DWORD dwRareMin;					//0x14
	DWORD dwSet;						//0x18
	DWORD dwSetDivisor;					//0x1C
	DWORD dwSetMin;						//0x20
	DWORD dwMagic;						//0x24
	DWORD dwMagicDivisor;				//0x28
	DWORD dwMagicMin;					//0x2C
	DWORD dwHiQuality;					//0x30
	DWORD dwHiQualityDivisor;			//0x34
	DWORD dwNormal;						//0x38
	DWORD dwNormalDivisor;				//0x3C
	WORD wVersion;						//0x40
	BYTE nUber;							//0x42
	BYTE nClassSpecific;				//0x43 
};

struct D2ItemStatCostTxt
{
	WORD wStatId;						//0x00
	WORD pad0x02;						//0x02
	DWORD dwItemStatFlags;				//0x04
	BYTE nSendBits;						//0x08
	BYTE nSendParamBits;				//0x09
	BYTE nCsvBits;						//0x0A
	BYTE nCsvParam;						//0x0B
	DWORD dwDivide;						//0x0C
	DWORD dwMultiply;					//0x10
	DWORD dwAdd;						//0x14
	BYTE nValShift;						//0x18
	BYTE nSaveBits;						//0x19
	BYTE n09SaveBits;					//0x1A
	BYTE pad0x1B;						//0x1B
	DWORD dwSaveAdd;					//0x1C
	DWORD dw09SaveAdd;					//0x20
	DWORD dwSaveParamBits;				//0x24
	DWORD dw09SaveParamBits;			//0x28
	DWORD dwMinAccr;					//0x2C
	BYTE nEncode;						//0x30
	BYTE pad0x31;						//0x31
	WORD wMaxStat;						//0x32
	short nDescPriority;				//0x34
	BYTE nDescFunc;						//0x36
	BYTE nDescVal;						//0x37
	WORD wDescStrPos;					//0x38
	WORD wDescStrNeg;					//0x3A
	WORD wDescStr2;						//0x3C
	WORD wDescGrp;						//0x3E
	BYTE nDescGrpFunc;					//0x40
	BYTE nDescGrpVal;					//0x41
	WORD wDescGrpStrPos;				//0x42
	WORD wDescGrpStrNeg;				//0x44
	WORD wDescGrpStr2;					//0x46
	WORD wItemEvent[2];					//0x48
	WORD wItemEventFunc[2];				//0x4C
	BYTE nKeepZero;						//0x50
	BYTE unk0x51[3];					//0x51 - also related to op stats (see DATATBLS_LoadItemStatCostTxt)
	BYTE nOp;							//0x54
	BYTE nOpParam;						//0x55
	WORD wOpBase;						//0x56
	WORD wOpStat[3];					//0x58
	WORD unk0x5E[64];					//0x5E - also related to op stats (see DATATBLS_LoadItemStatCostTxt)
	D2OpStatDataStrc pOpStatData[16];	//0xDE
	WORD pad0x13E;						//0x13E
	DWORD dwStuff;						//0x140
};

struct D2ItemsTxt
{
	char szFlippyFile[32];				//0x00
	char szInvFile[32];					//0x20
	char szUniqueInvFile[32];			//0x40
	char szSetInvFile[32];				//0x60
	union
	{
		DWORD dwCode;					//0x80
		char szCode[4];					//0x80
	};
	DWORD dwNormCode;					//0x84
	DWORD dwUberCode;					//0x88
	DWORD dwUltraCode;					//0x8C
	DWORD dwAlternateGfx;				//0x90
	DWORD dwPspell;						//0x94
	WORD wState;						//0x98
	WORD wCurseState[2];				//0x9A
	WORD wStat[3];						//0x9E
	DWORD dwCalc[3];					//0xA4
	DWORD dwLen;						//0xB0
	BYTE nSpellDesc;					//0xB4
	BYTE pad0xB5;						//0xB5
	WORD wSpellDescStr;					//0xB6
	DWORD dwSpellDescCalc;				//0xB8
	DWORD dwBetterGem;					//0xBC
	DWORD dwWeapClass;					//0xC0
	DWORD dwWeapClass2Hand;				//0xC4
	DWORD dwTransmogrifyType;			//0xC8
	DWORD dwMinAc;						//0xCC
	DWORD dwMaxAc;						//0xD0
	DWORD dwGambleCost;					//0xD4
	DWORD dwSpeed;						//0xD8
	DWORD dwBitField1;					//0xDC
	DWORD dwCost;						//0xE0
	DWORD dwMinStack;					//0xE4
	DWORD dwMaxStack;					//0xE8
	DWORD dwSpawnStack;					//0xEC
	DWORD dwGemOffset;					//0xF0
	WORD wNameStr;						//0xF4
	WORD wVersion;						//0xF6
	WORD wAutoPrefix;					//0xF8
	WORD wMissileType;					//0xFA
	BYTE nRarity;						//0xFC
	BYTE nLevel;						//0xFD
	BYTE nMinDam;						//0xFE
	BYTE nMaxDam;						//0xFF
	BYTE nMinMisDam;					//0x100
	BYTE nMaxMisDam;					//0x101
	BYTE n2HandMinDam;					//0x102
	BYTE n2HandMaxDam;					//0x103
	WORD wRangeAdder;					//0x104
	WORD wStrBonus;						//0x106
	WORD wDexBonus;						//0x108
	WORD wReqStr;						//0x10A
	WORD wReqDex;						//0x10C
	BYTE nAbsorb;						//0x10E
	BYTE nInvWidth;						//0x10F
	BYTE nInvHeight;					//0x110
	BYTE nBlock;						//0x111
	BYTE nDurability;					//0x112
	BYTE nNoDurability;					//0x113
	BYTE nMissile;						//0x114
	BYTE nComponent;					//0x115
	BYTE nArmorComp[6];					//0x116
	BYTE n2Handed;						//0x11C
	BYTE nUseable;						//0x11D
	WORD wType[2];						//0x11E
	WORD wSubType;						//0x122
	WORD wDropSound;					//0x124
	WORD wUseSound;						//0x126
	BYTE nDropSfxFrame;					//0x128
	BYTE nUnique;						//0x129
	BYTE nQuest;						//0x12A
	BYTE nQuestDiffCheck;				//0x12B
	BYTE nTransparent;					//0x12C
	BYTE nTransTbl;						//0x12D
	BYTE pad0x12E;						//0x12E
	BYTE nLightRadius;					//0x12F
	BYTE nBelt;							//0x130
	BYTE nAutoBelt;						//0x131
	BYTE nStackable;					//0x132
	BYTE nSpawnable;					//0x133
	BYTE nSpellIcon;					//0x134
	BYTE nDurWarning;					//0x135
	BYTE nQuantityWarning;				//0x136
	BYTE nHasInv;						//0x137
	BYTE nGemSockets;					//0x138
	BYTE nTransmogrify;					//0x139
	BYTE nTmogMin;						//0x13A
	BYTE nTmogMax;						//0x13B
	BYTE nHitClass;						//0x13C
	BYTE n1or2Handed;					//0x13D
	BYTE nGemApplyType;					//0x13E
	BYTE nLevelReq;						//0x13F
	BYTE nMagicLevel;					//0x140
	BYTE nTransform;					//0x141
	BYTE nInvTrans;						//0x142
	BYTE nCompactSave;					//0x143
	BYTE nSkipName;						//0x144
	BYTE nNameable;						//0x145
	BYTE nVendorMin[17];				//0x146
	BYTE nVendorMax[17];				//0x157
	BYTE nVendorMagicMin[17];			//0x168
	BYTE nVendorMagicMax[17];			//0x179
	BYTE nVendorMagicLvl[17];			//0x18A
	BYTE pad0x19B;						//0x19B
	DWORD dwNightmareUpgrade;			//0x19C
	DWORD dwHellUpgrade;				//0x1A0
	BYTE nPermStoreItem;				//0x1A4
	BYTE nMultibuy;						//0x1A5
	WORD pad0x1A6;						//0x1A6
};

struct D2ItemTypesTxt
{
	char szCode[4];						//0x00
	short nEquiv1;						//0x04
	short nEquiv2;						//0x06
	BYTE nRepair;						//0x08
	BYTE nBody;							//0x09
	BYTE nBodyLoc1;						//0x0A
	BYTE nBodyLoc2;						//0x0B
	WORD wShoots;						//0x0C
	WORD wQuiver;						//0x0E
	BYTE nThrowable;					//0x10
	BYTE nReload;						//0x11
	BYTE nReEquip;						//0x12
	BYTE nAutoStack;					//0x13
	BYTE nMagic;						//0x14
	BYTE nRare;							//0x15
	BYTE nNormal;						//0x16
	BYTE nCharm;						//0x17
	BYTE nGem;							//0x18
	BYTE nBeltable;						//0x19
	BYTE nMaxSock1;						//0x1A
	BYTE nMaxSock25;					//0x1B
	BYTE nMaxSock40;					//0x1C
	BYTE nTreasureClass;				//0x1D
	BYTE nRarity;						//0x1E
	BYTE nStaffMods;					//0x1F
	BYTE nCostFormula;					//0x20
	BYTE nClass;						//0x21
	BYTE nStorePage;					//0x22
	BYTE nVarInvGfx;					//0x23
	char szInvGfx1[32];					//0x24
	char szInvGfx2[32];					//0x44
	char szInvGfx3[32];					//0x64
	char szInvGfx4[32];					//0x84
	char szInvGfx5[32];					//0xA4
	char szInvGfx6[32];					//0xC4
};

struct D2LevelDefBin
{
	DWORD dwQuestFlag;					//0x00
	DWORD dwQuestFlagEx;				//0x04
	DWORD dwLayer;						//0x08
	DWORD dwSizeX;						//0x0C
	DWORD dwSizeX_N;					//0x10
	DWORD dwSizeX_H;					//0x14
	DWORD dwSizeY;						//0x18
	DWORD dwSizeY_N;					//0x1C
	DWORD dwSizeY_H;					//0x20
	DWORD dwOffsetX;					//0x24
	DWORD dwOffsetY;					//0x28
	DWORD dwDepend;						//0x2C
	DWORD dwDrlgType;					//0x30
	DWORD dwLevelType;					//0x34
	DWORD dwSubType;					//0x38
	DWORD dwSubTheme;					//0x3C
	DWORD dwSubWaypoint;				//0x40
	DWORD dwSubShrine;					//0x44
	DWORD dwVis[8];						//0x48
	DWORD dwWarp[8];					//0x68
	BYTE nIntensity;					//0x88
	BYTE nRed;							//0x89
	BYTE nGreen;						//0x8A
	BYTE nBlue;							//0x8B
	DWORD dwPortal;						//0x8C
	DWORD dwPosition;					//0x90
	DWORD dwSaveMonsters;				//0x94
	DWORD dwLOSDraw;					//0x98
};

struct D2LevelsTxt
{
	WORD wLevelNo;						//0x00
	BYTE nPal;							//0x02
	BYTE nAct;							//0x03
	BYTE nTeleport;						//0x04
	BYTE nRain;							//0x05
	BYTE nMud;							//0x06
	BYTE nNoPer;						//0x07
	BYTE nIsInside;						//0x08
	BYTE nDrawEdges;					//0x09
	WORD unk0x0A;						//0x0A
	DWORD dwWarpDist;					//0x0C
	WORD wMonLvl[3];					//0x10
	WORD wMonLvlEx[3];					//0x16
	DWORD dwMonDen[3];					//0x1C
	BYTE nMonUMin[3];					//0x28
	BYTE nMonUMax[3];					//0x2B
	BYTE nMonWndr;						//0x2E
	BYTE nMonSpcWalk;					//0x2F
	BYTE nQuest;						//0x30
	BYTE nRangedSpawn;					//0x31
	BYTE nNumMon;						//0x32
	BYTE nNumNormMon;					//0x33
	BYTE nNumNMon;						//0x34
	BYTE nNumUMon;						//0x35
	short wMon[25];						//0x36
	short wNMon[25];					//0x68
	short wUMon[25];					//0x9A
	WORD wCMon[4];						//0xCC
	WORD wCPct[4];						//0xD4
	WORD wCAmt[4];						//0xDC
	BYTE nWaypoint;						//0xE4
	BYTE nObjGroup[8];					//0xE5
	BYTE nObjPrb[8];					//0xED
	char szLevelName[40];				//0xF5
	char szLevelWarp[40];				//0x11D
	char szEntryFile[40];				//0x145
	BYTE pad0x16D;						//0x16D
	wchar_t wszLevelName[40];			//0x16E
	wchar_t wszLevelWarp[40];			//0x1BE
	WORD pad0x20E;						//0x20E
	DWORD dwThemes;						//0x210
	DWORD dwFloorFilter;				//0x214
	DWORD dwBlankScreen;				//0x218
	DWORD dwSoundEnv;					//0x21C
};

struct D2LowQualityItemsTxt
{
	char szName[32];					//0x00
	WORD wTblId;						//0x20
};

struct D2LvlMazeTxt
{
	DWORD dwLevelId;					//0x00
	DWORD dwRooms[3];					//0x04
	DWORD dwSizeX;						//0x10
	DWORD dwSizeY;						//0x14
	DWORD dwMerge;						//0x18
};

struct D2LvlPrestTxt
{
	DWORD dwDef;						//0x00
	DWORD dwLevelId;					//0x04
	DWORD dwPopulate;					//0x08
	DWORD dwLogicals;					//0x0C
	DWORD dwOutdoors;					//0x10
	DWORD dwAnimate;					//0x14
	DWORD dwKillEdge;					//0x18
	DWORD dwFillBlanks;					//0x1C
	DWORD dwExpansion;					//0x20
	DWORD unk0x24;						//0x24
	DWORD dwSizeX;						//0x28
	DWORD dwSizeY;						//0x2C
	DWORD dwAutoMap;					//0x30
	DWORD dwScan;						//0x34
	DWORD dwPops;						//0x38
	DWORD dwPopPad;						//0x3C
	DWORD dwFiles;						//0x40
	char szFile[6][60];					//0x44
										//char szFile1[60];					//0x44
										//char szFile2[60];					//0x80
										//char szFile3[60];					//0xBC
										//char szFile4[60];					//0xF8
										//char szFile5[60];					//0x134
										//char szFile6[60];					//0x170
	DWORD dwDt1Mask;					//0x1AC
};

struct D2LvlSubTxt
{
	DWORD dwType;						//0x00
	char szFile[60];					//0x04
	DWORD dwCheckAll;					//0x40
	DWORD dwBordType;					//0x44
	DWORD dwDt1Mask;					//0x48
	DWORD dwGridSize;					//0x4C
	D2UnkLvlSubTypeStrc* unk0x50;		//0x50
	BYTE unk0x54[200];					//0x54
	DWORD dwProb[5];					//0x11C
	DWORD dwTrials[5];					//0x130
	DWORD dwMax[5];						//0x144
	DWORD dwExpansion;					//0x158
};

struct D2LvlTypesTxt
{
	char szFile[32][60];				//0x00
	DWORD dwAct;						//0x780
	DWORD dwExpansion;					//0x784
};

struct D2LvlWarpTxt
{
	DWORD dwLevelId;					//0x00
	DWORD dwSelectX;					//0x04
	DWORD dwSelectY;					//0x08
	DWORD dwSelectDX;					//0x0C
	DWORD dwSelectDY;					//0x10
	DWORD dwExitWalkX;					//0x14
	DWORD dwExitWalkY;					//0x18
	DWORD dwOffsetX;					//0x1C
	DWORD dwOffsetY;					//0x20
	DWORD dwLitVersion;					//0x24
	DWORD dwTiles;						//0x28
	char szDirection[4];				//0x30
};

struct D2MagicAffixTxt
{
	char szName[32];					//0x00
	WORD wTblIndex;						//0x20
	WORD wVersion;						//0x22
	D2PropertyStrc pProperties[3];		//0x24
	WORD wSpawnable;					//0x54
	WORD wTransformColor;				//0x56
	DWORD dwLevel;						//0x58
	DWORD dwGroup;						//0x5C
	DWORD dwMaxLevel;					//0x60
	BYTE nRare;							//0x64
	BYTE nLevelReq;						//0x65
	BYTE nClassSpecific;				//0x66
	BYTE nClass;						//0x67
	BYTE nClassLevelReq;				//0x68
	BYTE pad0x69;						//0x69
	WORD wIType[7];						//0x6A
	WORD wEType[5];						//0x78
	WORD wFrequency;					//0x82
	DWORD dwDivide;						//0x84
	DWORD dwMultiply;					//0x88
	DWORD dwAdd;						//0x8C
};

struct D2MissCalcTxt
{
	DWORD dwCode;						//0x00
};

//TODO: Check
struct D2MissilesTxt
{
	DWORD dwId;							//0x00
	DWORD dwMissileFlags;				//0x04
	WORD wCltDoFunc;					//0x08
	WORD wCltHitFunc;					//0x0A
	WORD wSrvDoFunc;					//0x0C
	WORD wSrvHitFunc;					//0x0E
	WORD wSrvDmgFunc;					//0x10
	WORD wTravelSound;					//0x12
	WORD wHitSound;						//0x14
	WORD wExplosionMissile;				//0x16
	WORD wSubMissile[3];				//0x18
	WORD wCltSubMissile[3];				//0x1E
	WORD wHitSubMissile[4];				//0x24
	WORD wCltHitSubMissile[4];			//0x2C
	WORD wProgSound;					//0x34
	WORD wProgOverlay;					//0x36
	DWORD dwParam[5];					//0x38
	DWORD dwHitPar[3];					//0x4C
	DWORD dwCltParam[5];				//0x58
	DWORD dwCltHitPar[3];				//0x6C
	DWORD dwDmgParam[2];				//0x78
	DWORD dwSrvCalc;					//0x80
	DWORD dwCltCalc;					//0x84
	DWORD dwHitCalc;					//0x88
	DWORD dwCltHitCalc;					//0x8C
	DWORD dwDmgCalc;					//0x90
	WORD wHitClass;						//0x94
	WORD wRange;						//0x96
	WORD wLevRange;						//0x98
	BYTE nVel;							//0x9A
	BYTE nVelLev;						//0x9B
	WORD wMaxVel;						//0x9C
	WORD wAccel;						//0x9E
	WORD wAnimRate;						//0xA0
	WORD wXoffset;						//0xA2
	WORD wYoffset;						//0xA4
	WORD wZoffset;						//0xA6
	DWORD dwHitFlags;					//0xA8
	WORD wResultFlags;					//0xAC
	WORD wKnockBack;					//0xAE
	DWORD dwMinDamage;					//0xB0
	DWORD dwMaxDamage;					//0xB4
	DWORD dwMinLevDam[5];				//0xB8
	DWORD dwMaxLevDam[5];				//0xCC
	DWORD dwDmgSymPerCalc;				//0xE0
	DWORD dwElemType;					//0xE4
	DWORD dwElemMin;					//0xE8
	DWORD dwElemMax;					//0xEC
	DWORD dwMinElemLev[5];				//0xF0
	DWORD dwMaxElemLev[5];				//0x104
	DWORD dwElemDmgSymPerCalc;			//0x118
	DWORD dwElemLen;					//0x11C
	DWORD dwElemLevLen[3];				//0x120
	BYTE unk0x12C;						//0x12C
	BYTE nSrcDamage;					//0x12D
	BYTE nSrcMissDmg;					//0x12E
	BYTE nHoly;							//0x12F
	BYTE nLight;						//0x130
	BYTE nFlicker;						//0x131
	BYTE nRGB[3];						//0x132
	BYTE nInitSteps;					//0x135
	BYTE nActivate;						//0x136
	BYTE nLoopAnim;						//0x137
	char szCelFile[64];					//0x138
	DWORD dwAnimLen;					//0x178
	DWORD dwRandStart;					//0x17C
	BYTE nSubLoop;						//0x180
	BYTE nSubStart;						//0x181
	BYTE nSubStop;						//0x182
	BYTE nCollideType;					//0x183
	BYTE nCollision;					//0x184
	BYTE nClientCol;					//0x185
	BYTE nCollideKill;					//0x186
	BYTE nCollideFriend;				//0x187
	BYTE nNextHit;						//0x188
	BYTE nNextDelay;					//0x189
	BYTE nSize;							//0x18A
	BYTE nToHit;						//0x18B
	BYTE nAlwaysExplode;				//0x18C
	BYTE nTrans;						//0x18D
	WORD wQty;							//0x18E
	DWORD dwSpecialSetup;				//0x190
	WORD wSkill;						//0x194
	BYTE nHitShift;						//0x196
	BYTE unk0x197[5];					//0x197
	DWORD dwDamageRate;					//0x19C
	BYTE nNumDirections;				//0x1A0
	BYTE nAnimSpeed;					//0x1A1
	BYTE nLocalBlood;					//0x1A2
	BYTE pad0x1A3;						//0x1A3
};

struct D2MonAiTxt
{
	WORD wAi;							//0x00
};

struct D2MonEquipTxt
{
	short nMonster;						//0x00
	WORD wLevel;						//0x02
	BYTE nOnInit;						//0x04
	BYTE pad0x05[3];					//0x05
	DWORD dwItem[3];					//0x08
	BYTE nLoc[3];						//0x14
	BYTE nMod[3];						//0x17
	WORD pad0x1A;						//0x1A
};

struct D2MonItemPercentTxt
{
	BYTE nHeartPercent;					//0x00
	BYTE nBodyPartPercent;				//0x01
	BYTE nTreasureClassPercent;			//0x02
	BYTE nComponentPercent;				//0x03
};

struct D2MonLvlTxt
{
	DWORD dwAC[3];						//0x00
	DWORD dwLAC[3];						//0x0C
	DWORD dwTH[3];						//0x18
	DWORD dwLTH[3];						//0x24
	DWORD dwHP[3];						//0x30
	DWORD dwLHP[3];						//0x3C
	DWORD dwDM[3];						//0x48
	DWORD dwLDM[3];						//0x54
	DWORD dwXP[3];						//0x60
	DWORD dwLXP[3];						//0x6C
};

struct D2MonModeTxtStub
{
	DWORD dwCode;						//0x00
};

struct D2MonModeTxt
{
	char szName[32];					//0x00
	DWORD dwToken;						//0x20
	BYTE nDTDir;						//0x24
	BYTE nNUDir;						//0x25
	BYTE nWLDir;						//0x26
	BYTE nGHDir;						//0x27
	BYTE nA1Dir;						//0x28
	BYTE nA2Dir;						//0x29
	BYTE nBLDir;						//0x2A
	BYTE nSCDir;						//0x2B
	BYTE nS1Dir;						//0x2C
	BYTE nS2Dir;						//0x2D
	BYTE nS3Dir;						//0x2E
	BYTE nS4Dir;						//0x2F
	BYTE nDDDir;						//0x30
	BYTE nKBDir;						//0x31
	BYTE nSQDir;						//0x32
	BYTE nRNDir;						//0x33
};

struct D2MonPlaceTxt
{
	WORD nCode;							//0x00
};

struct D2MonPresetTxt
{
	BYTE nAct;							//0x00
	BYTE nType;							//0x01
	WORD wPlace;						//0x02
};

struct D2MonPropTxt
{
	DWORD dwId;							//0x00
	D2PropertyStrc props[3][6];			//0x04
	BYTE nChance[3][6];					//0x124
	WORD pad0x136;						//0x136
};

struct D2MonSeqTxt
{
	WORD wSequence;						//0x00
	BYTE nMode;							//0x02
	BYTE nFrame;						//0x03
	BYTE nDir;							//0x04
	BYTE nEvent;						//0x05
};

struct D2MonSoundsTxt
{
	DWORD dwId;							//0x00
	DWORD dwAttack1;					//0x04
	DWORD dwAtt1Del;					//0x08
	DWORD dwAtt1Prb;					//0x0C
	DWORD dwAttack2;					//0x10
	DWORD dwAtt2Del;					//0x14
	DWORD dwAtt2Prb;					//0x18
	DWORD dwWeapon1;					//0x1C
	DWORD dwWea1Del;					//0x20
	DWORD dwWea1Vol;					//0x24
	DWORD dwWeapon2;					//0x28
	DWORD dwWea2Del;					//0x2C
	DWORD dwWea2Vol;					//0x30
	DWORD dwHitSound;					//0x34
	DWORD dwHitDelay;					//0x38
	DWORD dwDeathSound;					//0x3C
	DWORD dwDeaDelay;					//0x40
	DWORD dwSkill[4];					//0x44
	DWORD dwFootstep;					//0x54
	DWORD dwFootstepLayer;				//0x58
	DWORD dwFsCnt;						//0x5C
	DWORD dwFsOff;						//0x60
	DWORD dwFsPrb;						//0x64
	DWORD dwNeutral;					//0x68
	DWORD dwNeuTime;					//0x6C
	DWORD dwInit;						//0x70
	DWORD dwTaunt;						//0x74
	DWORD dwFlee;						//0x78
	BYTE nCvtMo1;						//0x7C
	BYTE nCvtTgt1;						//0x7D
	WORD pad0x7E;						//0x7E
	DWORD dwCvtSk1;						//0x80
	BYTE nCvtMo2;						//0x84
	BYTE nCvtTgt2;						//0x85
	WORD pad0x86;						//0x86
	DWORD dwCvtSk2;						//0x88
	BYTE nCvtMo3;						//0x8C
	BYTE nCvtTgt3;						//0x8D
	WORD pad0x8E;						//0x8E
	DWORD dwCvtSk3;						//0x90
};

struct D2MonStatsTxt
{
	short nId;							//0x00
	short nBaseId;						//0x02
	short nNextInClass;					//0x04
	WORD wNameStr;						//0x06
	WORD wDescStr;						//0x08
	WORD unk0x0A;						//0x0A
	DWORD dwMonStatsFlags;				//0x0C
	DWORD dwCode;						//0x10
	WORD wMonSound;						//0x14
	WORD wUMonSound;					//0x16
	WORD wMonStatsEx;					//0x18
	WORD wMonProp;						//0x1A
	WORD wMonType;						//0x1C
	WORD wAI;							//0x1E
	WORD wSpawn;						//0x20
	BYTE nSpawnX;						//0x22
	BYTE nSpawnY;						//0x23
	BYTE nSpawnMode;					//0x24
	BYTE unk0x25;						//0x25
	WORD wMinion1;						//0x26
	WORD wMinion2;						//0x28
	short nMonEquipTxtRecordId;			//0x2A
	BYTE nPartyMin;						//0x2C
	BYTE nPartyMax;						//0x2D
	BYTE nRarity;						//0x2E
	BYTE nMinGrp;						//0x2F
	BYTE nMaxGrp;						//0x30
	BYTE nSparsePopulate;				//0x31
	short nVelocity;					//0x32
	short nRun;							//0x34
	short unk0x36;						//0x36 - Special Velocity (see DATATBLS_LoadMonStatsTxt)
	short unk0x38;						//0x38 - Special RunSpeed (see DATATBLS_LoadMonStatsTxt)
	WORD wMissA1;						//0x3A
	WORD wMissA2;						//0x3C
	WORD wMissS1;						//0x3E
	WORD wMissS2;						//0x40
	WORD wMissS3;						//0x42
	WORD wMissS4;						//0x44
	WORD wMissC;						//0x46
	WORD wMissSQ;						//0x48
	BYTE unk0x4A;						//0x4A - Related to the monster chain (see DATATBLS_LoadMonStatsTxt)
	BYTE unk0x4B;						//0x4B - Related to the monster chain (see DATATBLS_LoadMonStatsTxt)
	BYTE nAlign;						//0x4C
	BYTE nTransLvl;						//0x4D
	BYTE nThreat;						//0x4E
	BYTE nAIdel[3];						//0x4F
	BYTE nAiDist[3];					//0x52
	BYTE unk0x55;						//0x55
	WORD wAiParam[8][3];				//0x56
	WORD wTreasureClass[3][4];			//0x86
	BYTE nTCQuestId;					//0x9E
	BYTE nTCQuestCP;					//0x9F
	BYTE nDrain[3];						//0xA0
	BYTE nToBlock[3];					//0xA3
	BYTE nCrit;							//0xA6
	BYTE unk0xA7;						//0xA7
	WORD wSkillDamage;					//0xA8
	WORD nLevel[3];						//0xAA
	WORD nMinHP[3];						//0xB0
	WORD nMaxHP[3];						//0xB6
	WORD nAC[3];						//0xBC
	WORD nA1TH[3];						//0xC2
	WORD nA2TH[3];						//0xC8
	WORD nS1TH[3];						//0xCE
	WORD nExp[3];						//0xD4
	WORD nA1MinD[3];					//0xDA
	WORD nA1MaxD[3];					//0xE0
	WORD nA2MinD[3];					//0xE6
	WORD nA2MaxD[3];					//0xEC
	WORD nS1MinD[3];					//0xF2
	WORD nS1MaxD[3];					//0xF8
	BYTE nElMode[3];					//0xFE
	BYTE nElType[3];					//0x101
	BYTE nElPct[3][3];					//0x104
	BYTE unk0x10D;						//0x10D
	WORD nElMinD[3][3];					//0x10E
	WORD nElMaxD[3][3];					//0x120
	WORD nElDur[3][3];					//0x132
	WORD nResistances[6][3];			//0x144
	BYTE nColdEffect[3];				//0x168
	BYTE unk0x16B;						//0x16B
	DWORD dwSendSkills;					//0x16C
	short nSkill[8];					//0x170
	BYTE nSkillMode[8];					//0x180
	WORD nSequence[8];					//0x188
	BYTE nSkLvl[8];						//0x198
	DWORD dwDamageRegen;				//0x1A0
	BYTE nSplEndDeath;					//0x1A4
	BYTE nSplGetModeChart;				//0x1A5
	BYTE nSplEndGeneric;				//0x1A6
	BYTE nSplClientEnd;					//0x1A7
};

//TODO: Check
struct D2MonStats2Txt
{
	DWORD dwId;							//0x00
	DWORD pad0x04;						//0x04
	BYTE nSizeX;						//0x08
	BYTE nSizeY;						//0x09
	BYTE nSpawnCol;						//0x0A
	BYTE nHeight;						//0x0B
	BYTE nOverlayHeight;				//0x0C
	BYTE nPixHeight;					//0x0D
	BYTE nMeleeRng;						//0x0E
	BYTE pad0x0F;						//0x0F
	char szBaseW[4];					//0x10
	BYTE nHitClass;						//0x14
	BYTE unk0x15[215];					//0x15
	BYTE nTotalPieces;					//0xEC
	BYTE unk0xED[7];					//0xED
	BYTE ndDT;							//0xF4
	BYTE ndNU;							//0xF5
	BYTE ndWL;							//0xF6
	BYTE ndGH;							//0xF7
	BYTE ndA1;							//0xF8
	BYTE ndA2;							//0xF9
	BYTE ndBL;							//0xFA
	BYTE ndSC;							//0xFB
	BYTE ndS1;							//0xFC
	BYTE ndS2;							//0xFD
	BYTE ndS3;							//0xFE
	BYTE ndS4;							//0xFF
	BYTE ndDD;							//0x100
	BYTE ndKB;							//0x101
	BYTE ndSQ;							//0x102
	BYTE ndRN;							//0x103
	BYTE unk0x104[4];					//0x104
	BYTE nInfernoLen;					//0x108
	BYTE nInfernoAim;					//0x109
	BYTE nInfernoRollback;				//0x10A
	BYTE nResurrectMode;				//0x10B
	WORD nResurrectSkill;				//0x10C
	WORD whtTop;						//0x10E
	WORD whtLeft;						//0x110
	WORD whtWidth;						//0x112
	WORD whtHeight;						//0x114
	WORD unk0x116;						//0x116
	DWORD dwAutomapCel;					//0x118
	BYTE nLocalBlood;					//0x11C
	BYTE nBleed;						//0x11D
	BYTE nLight;						//0x11E
	BYTE nLight_R;						//0x11F
	BYTE nLight_G;						//0x120
	BYTE nLight_B;						//0x121
	BYTE nUtrans[3];					//0x122
	BYTE unk0x125[3];					//0x125
	char szHeart[4];					//0x128
	char szBodyPart[4];					//0x12C
	BYTE nRestore;						//0x130
	BYTE pad0x131[3];					//0x131
};

struct D2MonTypeTxt
{
	WORD wType;							//0x00
	short nEquiv[3];					//0x02
	WORD wStrSing;						//0x08
	WORD wStrPlur;						//0x0A
};

struct D2MonUModTxt
{
	DWORD dwUniqueMod;					//0x00
	WORD wVersion;						//0x04
	BYTE nEnabled;						//0x06
	BYTE nXfer;							//0x07
	BYTE nChampion;						//0x08
	BYTE nFPick;						//0x09
	WORD wExclude[2];					//0x0A
	WORD wCPick[3];						//0x0E
	WORD wUPick[3];						//0x14
	WORD pad0x1A;						//0x1A
	DWORD dwConstants;					//0x1C
};

struct D2NpcTxt
{
	DWORD dwNpc;						//0x00
	DWORD dwSellMult;					//0x04
	DWORD dwBuyMult;					//0x08
	DWORD dwRepMult;					//0x0C
	DWORD dwQuestflagA;					//0x10
	DWORD dwQuestFlagB;					//0x14
	DWORD dwQuestFlagC;					//0x18
	DWORD dwQuestSellMultA;				//0x1C
	DWORD dwQuestSellMultB;				//0x20
	DWORD dwQuestSellMultC;				//0x24
	DWORD dwQuestBuyMultA;				//0x28
	DWORD dwQuestBuyMultB;				//0x2C
	DWORD dwQuestBuyMultC;				//0x30
	DWORD dwQuestRepMultA;				//0x34
	DWORD dwQuestRepMultB;				//0x38
	DWORD dwQuestRepMultC;				//0x3C
	DWORD dwMaxBuy;						//0x40
	DWORD dwMaxBuy_N;					//0x44
	DWORD dwMaxBuy_H;					//0x48
};

struct D2ObjectsTxt
{
	char szName[64];					//0x00
	wchar_t wszName[64];				//0x40
	char szToken[3];					//0xC0
	BYTE nSpawnMax;						//0xC3
	BYTE nSelectable[8];				//0xC4
	BYTE nTrapProb;						//0xCC
	BYTE pad0xCD[3];					//0xCD
	DWORD dwSizeX;						//0xD0
	DWORD dwSizeY;						//0xD4
	DWORD dwFrameCnt[8];				//0xD8
	WORD wFrameDelta[8];				//0xF8
	BYTE nCycleAnim[8];					//0x108
	BYTE nLit[8];						//0x110
	BYTE nBlocksLight[8];				//0x118
	BYTE nHasCollision[8];				//0x120
	BYTE nIsAttackable0;				//0x128
	BYTE nStart[8];						//0x129
	BYTE nOrderFlag[8];					//0x131
	BYTE nEnvEffect;					//0x139
	BYTE nIsDoor;						//0x13A
	BYTE nBlocksVis;					//0x13B
	BYTE nOrientation;					//0x13C
	BYTE nPreOperate;					//0x13D
	BYTE nTrans;						//0x13E
	BYTE nMode[8];						//0x13F
	BYTE pad0x147;						//0x147
	DWORD dwXOffset;					//0x148
	DWORD dwYOffset;					//0x14C
	BYTE nDraw;							//0x150
	BYTE nHD;							//0x151
	BYTE nTR;							//0x152
	BYTE nLG;							//0x153
	BYTE nRA;							//0x154
	BYTE nLA;							//0x155
	BYTE nRH;							//0x156
	BYTE nLH;							//0x157
	BYTE nSH;							//0x158
	BYTE nS1;							//0x159
	BYTE nS2;							//0x15A
	BYTE nS3;							//0x15B
	BYTE nS4;							//0x15C
	BYTE nS5;							//0x15D
	BYTE nS6;							//0x15E
	BYTE nS7;							//0x15F
	BYTE nS8;							//0x160
	BYTE nTotalPieces;					//0x161
	BYTE nXSpace;						//0x162
	BYTE nYSpace;						//0x163
	BYTE nRed;							//0x164
	BYTE nGreen;						//0x165
	BYTE nBlue;							//0x166
	BYTE nSubClass;						//0x167
	DWORD dwNameOffset;					//0x168
	BYTE pad0x16C;						//0x16C
	BYTE nMonsterOK;					//0x16D
	BYTE nOperateRange;					//0x16E
	BYTE nShrineFunction;				//0x16F
	BYTE nAct;							//0x170
	BYTE nLockable;						//0x171
	BYTE nGore;							//0x172
	BYTE nRestore;						//0x173
	BYTE nRestoreVirgins;				//0x174
	BYTE nSync;							//0x175
	WORD pad0x176;						//0x177
	DWORD dwParm[8];					//0x178
	BYTE nTgtFX;						//0x198
	BYTE nTgtFY;						//0x199
	BYTE nTgtBX;						//0x19A
	BYTE nTgtBY;						//0x19B
	BYTE nDamage;						//0x19C
	BYTE nCollisionSubst;				//0x19D
	WORD pad0x19E;						//0x19E
	DWORD dwLeft;						//0x1A0
	DWORD dwTop;						//0x1A4
	DWORD dwWidth;						//0x1A8
	DWORD dwHeight;						//0x1AC
	BYTE nBeta;							//0x1B0
	BYTE nInitFn;						//0x1B1
	BYTE nPopulateFn;					//0x1B2
	BYTE nOperateFn;					//0x1B3
	BYTE nClientFn;						//0x1B4
	BYTE nOverlay;						//0x1B5
	BYTE nBlockMissile;					//0x1B6
	BYTE nDrawUnder;					//0x1B7
	BYTE nOpenWarp;						//0x1B8
	BYTE pad0x1B9[3];					//0x1B9
	DWORD dwAutomap;					//0x1BC
};

struct D2ObjGroupTxt
{
	DWORD dwId[8];						//0x00
	BYTE nDensity[8];					//0x20
	BYTE nProbability[8];				//0x28
	BYTE nShrines;						//0x30
	BYTE nWells;						//0x31
	WORD pad0x32;						//0x32
};

struct D2ObjModeTypeTxt
{
	char szName[32];					//0x00
	char szToken[20];					//0x20
};

struct D2OverlayTxt
{
	WORD wOverlay;						//0x00
	char szFilename[64];				//0x02
	WORD wVersion;						//0x42
	DWORD dwFrames;						//0x44
	DWORD dwPreDraw;					//0x48
	DWORD dw1ofN;						//0x4C
	BYTE nDir;							//0x50
	BYTE nOpen;							//0x51
	BYTE nBeta;							//0x52
	BYTE pad0x53;						//0x53
	DWORD dwOffsetX;					//0x54
	DWORD dwOffsetY;					//0x58
	DWORD dwHeight[4];					//0x5C
	DWORD dwAnimRate;					//0x6C
	DWORD dwInitRadius;					//0x70
	DWORD dwRadius;						//0x74
	DWORD dwLoopWaitTime;				//0x78
	BYTE nTrans;						//0x7C
	BYTE nRed;							//0x7D
	BYTE nGreen;						//0x7E
	BYTE nBlue;							//0x7F
	BYTE nNumDirections;				//0x80
	BYTE nLocalBlood;					//0x81
	WORD pad0x82;						//0x82
};

struct D2PetTypeTxt
{
	DWORD dwHcIdx;						//0x00
	DWORD dwPetFlags;					//0x04
	WORD wGroup;						//0x08
	WORD wBaseMax;						//0x0A
	WORD wStringId;						//0x0C
	BYTE nIconType;						//0x0E
	char szBaseicon[32];				//0x0F
	char szIcon[4][32];					//0x2F
	BYTE pad0xAF[3];					//0xAF
	WORD wClass[4];						//0xB2
	BYTE unk0xBA[2];					//0xBA
	int nSkillCount;					//0xBC
	WORD wSkillIds[15];					//0xC0
	WORD unk0xDE;						//0xDE
};

struct D2PlayerClassTxt
{
	DWORD dwCode;						//0x00
};

struct D2PlrModeTxtStub
{
	DWORD dwCode;						//0x00
};

struct D2PlrModeTypeTxt
{
	char szName[32];					//0x00
	char szToken[20];					//0x20
};

struct D2PropertiesTxt
{
	WORD wProp;							//0x00
	BYTE nSet[7];						//0x02
	BYTE pad0x09;						//0x09
	WORD wVal[7];						//0x0A
	BYTE nFunc[7];						//0x18
	BYTE pad0x1F;						//0x1F
	WORD wStat[7];						//0x20
};

struct D2QualityItemsTxt
{
	BYTE nArmor;						//0x00
	BYTE nWeapon;						//0x01
	BYTE nShield;						//0x02
	BYTE nScepter;						//0x03
	BYTE nWand;							//0x04
	BYTE nStaff;						//0x05
	BYTE nBow;							//0x06
	BYTE nBoots;						//0x07
	BYTE nGloves;						//0x08
	BYTE nBelt;							//0x09
	BYTE nNumMods;						//0x0A
	BYTE pad0x0B;						//0x0B
	D2PropertyStrc pProperties[2];		//0x0C
	char szEffect1[32];					//0x2C
	char szEffect2[32];					//0x4C
	WORD wEffect1TblId;					//0x6C
	WORD wEffect2TblId;					//0x6E
};

struct D2RareAffixTxt
{
	DWORD unk0x00[3];					//0x00
	WORD wStringId;						//0x0C
	WORD wVersion;						//0x0E
	WORD wIType[7];						//0x10
	WORD wEType[4];						//0x1E
	char szName[32];					//0x26
	WORD pad0x46;						//0x46
};

struct D2RunesTxt
{
	char szName[64];						//0x00
	char szRuneName[64];					//0x40
	BYTE nComplete;							//0x80
	BYTE nServer;							//0x81
	WORD wStringId;							//0x82
	WORD pad0x84;							//0x84
	WORD wIType[6];							//0x86
	WORD wEType[3];							//0x92
	DWORD dwRune[6];						//0x98
	D2PropertyStrc pProperties[7];			//0xB0
};

struct D2SetItemsTxt
{
	WORD wSetItemId;						//0x00
	char szName[32];						//0x02
	WORD wVersion;							//0x22
	WORD wStringId;							//0x24
	WORD pad0x26;							//0x26
	DWORD szItemCode;						//0x28
	short nSetId;							//0x2C
	short nSetItems;						//0x2E
	WORD wLvl;								//0x30
	WORD wLvlReq;							//0x32
	DWORD dwRarity;							//0x34
	DWORD dwCostMult;						//0x38
	DWORD dwCostAdd;						//0x3C
	BYTE nChrTransform;						//0x40
	BYTE nInvTransform;						//0x41
	char szFlippyFile[32];					//0x42
	char szInvFile[32];						//0x62
	WORD wDropSound;						//0x82
	WORD wUseSound;							//0x84
	BYTE nDropSfxFrame;						//0x86
	BYTE nAddFunc;							//0x87
	D2PropertyStrc pProperties[9];			//0x88
	D2PropertyStrc pPartialBoni[10];		//0x118
};

struct D2SetsTxt
{
	WORD wSetId;						//0x00
	WORD wStringId;						//0x02
	WORD wVersion;						//0x04
	WORD pad0x06;						//0x06
	DWORD unk0x08;						//0x08
	int nSetItems;						//0x0C
	D2PropertyStrc pBoni2[2];			//0x10
	D2PropertyStrc pBoni3[2];			//0x30
	D2PropertyStrc pBoni4[2];			//0x50
	D2PropertyStrc pBoni5[2];			//0x70
	D2PropertyStrc pFBoni[8];			//0x90
	D2SetItemsTxt* pSetItem[6];			//0x110
};

struct D2ShrinesTxt
{
	DWORD dwCode;						//0x00
	DWORD dwArg0;						//0x04
	DWORD dwArg1;						//0x08
	DWORD dwDurationInFrames;			//0x0C
	BYTE nResetTimeInMins;				//0x10
	BYTE nRarity;						//0x11
	char szViewname[32];				//0x12
	char szNiftyphrase[128];			//0x32
	WORD wEffectClass;					//0xB2
	DWORD dwLevelMin;					//0xB4
};

struct D2SkillCalcTxt
{
	DWORD dwCode;						//0x00
};

struct D2SkillDescTxt
{
	WORD wSkillDesc;					//0x00
	BYTE nSkillPage;					//0x02
	BYTE nSkillRow;						//0x03
	BYTE nSkillColumn;					//0x04
	BYTE nListRow;						//0x05
	BYTE nListPool;						//0x06
	BYTE nIconCel;						//0x07
	WORD wStrName;						//0x08
	WORD wStrShort;						//0x0A
	WORD wStrLong;						//0x0C
	WORD wStrAlt;						//0x0E
	WORD wStrMana;						//0x10
	WORD wDescDam;						//0x12
	WORD wDescAtt;						//0x14
	WORD pad0x16;						//0x16
	DWORD dwDamCalc[2];					//0x18
	BYTE nPrgDamElem[3];				//0x20
	BYTE pad0x23;						//0x23
	DWORD dwProgDmgMin[3];				//0x24
	DWORD dwProgDmgMax[3];				//0x30
	WORD wDescMissile[3];				//0x3C
	BYTE nDescLine[17];					//0x42
	BYTE pad0x53;						//0x53
	WORD wDescTextA[17];				//0x54
	WORD wDescTextB[17];				//0x76
	DWORD dwDescCalcA[17];				//0x98
	DWORD dwDescCalcB[17];				//0xDC
};

struct D2SkillsTxt
{
	int nSkillId; 						//0x00
	DWORD dwFlags[2];					//0x04
	BYTE nCharClass;					//0x0C
	BYTE unk0x0D[3];					//0x0D
	BYTE nAnim;							//0x10
	BYTE nMonAnim;						//0x11
	BYTE nSeqTrans;						//0x12
	BYTE nSeqNum;						//0x13
	BYTE nRange;						//0x14
	BYTE nSelectProc;					//0x15
	BYTE nSeqInput;						//0x16
	BYTE pad0x17;						//0x17
	WORD wITypeA[3];					//0x18
	WORD wITypeB[3];					//0x1E
	WORD wETypeA[2];					//0x24
	WORD wETypeB[2];					//0x28
	WORD wSrvStartFunc;					//0x2C
	WORD wSrvDoFunc;					//0x2E
	WORD wSrvPrgFunc[3];				//0x30
	WORD pad0x36;						//0x36
	DWORD dwPrgCalc[3];					//0x38
	BYTE nPrgDamage;					//0x44
	BYTE pad0x45;						//0x45
	WORD wSrvMissile;					//0x46
	WORD wSrvMissileA;					//0x48
	WORD wSrvMissileB;					//0x4A
	WORD wSrvMissileC;					//0x4C
	WORD wSrvOverlay;					//0x4E
	DWORD dwAuraFilter;					//0x50
	WORD wAuraStat[6];					//0x54
	DWORD dwAuraLenCalc;				//0x60
	DWORD dwAuraRangeCalc;				//0x64
	DWORD dwAuraStatCalc[6];			//0x68
	WORD wAuraState;					//0x80
	WORD wAuraTargetState;				//0x82
	WORD wAuraEvent[3];					//0x84
	WORD wAuraEventFunc[3];				//0x8A
	WORD wAuraTgtEvent;					//0x90
	WORD wAuraTgtEventFunc;				//0x92
	short nPassiveState;				//0x94
	WORD wPassiveIType;					//0x96
	WORD wPassiveStat[5];				//0x98
	WORD pad0xA2;						//0xA2
	DWORD dwPassiveCalc[5];				//0xA4
	WORD wPassiveEvent;					//0xB8
	WORD wPassiveEventFunc;				//0xBA
	WORD wSummon;						//0xBC
	BYTE nPetType;						//0xBE
	BYTE nSumMode;						//0xBF
	DWORD dwPetMax;						//0xC0
	WORD wSumSkill[5];					//0xC4
	WORD pad0xCE;						//0xCE
	DWORD dwSumSkCalc[5];				//0xD0
	WORD wSumUMod;						//0xE4
	WORD wSumOverlay;					//0xE6
	WORD wCltMissile;					//0xE8
	WORD wCltMissileA;					//0xEA
	WORD wCltMissileB;					//0xEC
	WORD wCltMissileC;					//0xEE
	WORD wCltMissileD;					//0xF0
	WORD wCltStFunc;					//0xF2
	WORD wCltDoFunc;					//0xF4
	WORD wCltPrgFunc[3];				//0xF6
	WORD wStSound;						//0xFC
	WORD nStSoundClass;					//0x0FE	
	WORD wDoSound;						//0x100
	WORD wDoSoundA;						//0x102
	WORD wDoSoundB;						//0x104
	WORD wCastOverlay;					//0x106
	WORD wTgtOverlay;					//0x108
	WORD wTgtSound;						//0x10A
	WORD wPrgOverlay;					//0x10C
	WORD wPrgSound;						//0x10E
	WORD wCltOverlayA;					//0x110
	WORD wCltOverlayB;					//0x112
	DWORD dwCltCalc[3];					//0x114
	BYTE nItemTarget;					//0x120
	BYTE pad0x121;						//0x121
	WORD wItemCastSound;				//0x122
	WORD wItemCastOverlay;				//0x124
	WORD pad0x126;						//0x126
	DWORD dwPerDelay;					//0x128
	WORD wMaxLvl;						//0x12C
	WORD wResultFlags;					//0x12E
	DWORD dwHitFlags;					//0x130
	DWORD dwHitClass;					//0x134
	DWORD dwCalc[4];					//0x138
	DWORD dwParam[8];					//0x148
	BYTE nWeapSel;						//0x168
	BYTE pad0x169;						//0x169
	WORD wItemEffect;					//0x16A
	WORD wItemCltEffect;				//0x16C
	WORD pad0x16E;					//0x16E
	DWORD dwSkPoints;					//0x170
	WORD wReqLevel;						//0x174
	WORD wReqStr;						//0x176
	WORD wReqDex;						//0x178
	WORD wReqInt;						//0x17A
	WORD wReqVit;						//0x17C
	WORD wReqSkill[3];					//0x17E
	WORD wStartMana;					//0x184
	WORD wMinMana;						//0x186
	WORD wManaShift;					//0x188
	WORD wMana;							//0x18A
	WORD wLevelMana;					//0x18C
	BYTE nAttackRank;					//0x18E
	BYTE nLineOfSight;					//0x18F
	DWORD dwDelay;						//0x190
	WORD wSkillDesc;					//0x194
	WORD pad0x196;						//0x196
	DWORD dwToHit;						//0x198
	DWORD dwLevToHit;					//0x19C
	DWORD dwToHitCalc;					//0x1A0
	BYTE nToHitShift;					//0x1A4
	BYTE nSrcDam;						//0x1A5
	WORD pad0x1A6;						//0x1A6
	DWORD dwMinDam;						//0x1A8
	DWORD dwMaxDam;						//0x1AC
	DWORD dwMinLvlDam[5];				//0x1B0
	DWORD dwMaxLvlDam[5];				//0x1C4
	DWORD dwDmgSymPerCalc;				//0x1D8
	BYTE nEType;						//0x1DC
	BYTE pad0x1DD[3];					//0x1DD
	DWORD dwEMin;						//0x1E0
	DWORD dwEMax;						//0x1E4
	DWORD dwEMinLev[5];					//0x1E8
	DWORD dwEMaxLev[5];					//0x1FC
	DWORD dwEDmgSymPerCalc;				//0x210
	DWORD dwELen;						//0x214
	DWORD dwELevLen[3];					//0x218
	DWORD dwELenSymPerCalc;				//0x224
	BYTE nRestrict;						//0x228
	BYTE pad0x229;						//0x229
	WORD wState[3];						//0x22A
	BYTE nAiType;						//0x230
	BYTE pad0x231;						//0x231
	WORD wAiBonus;						//0x232
	DWORD dwCostMult;					//0x234
	DWORD dwCostAdd;					//0x238
};

struct D2StatesTxt
{
	WORD wState;						//0x00
	WORD wOverlay[4];					//0x02
	WORD wCastOverlay;					//0x0A
	WORD wRemoveOverlay;				//0x0C
	WORD wPrgOverlay;					//0x0E
	BYTE nStateFlags[4];				//0x10
	DWORD dwStateFlagsEx;				//0x14
	WORD wStat;							//0x18
	WORD wSetFunc;						//0x1A
	WORD wRemFunc;						//0x1C
	WORD wGroup;						//0x1E
	BYTE nColorPri;						//0x20
	BYTE nColorShift;					//0x21
	BYTE nLightRGB[3];					//0x22
	BYTE pad0x25;						//0x25
	WORD wOnSound;						//0x26
	WORD wOffSound;						//0x28
	WORD wItemType;						//0x2A
	BYTE nItemTrans;					//0x2C
	BYTE nGfxType;						//0x2D
	WORD wGfxClass;						//0x2E
	WORD wCltEvent;						//0x30
	WORD wCltEventFunc;					//0x32
	WORD wCltActiveFunc;				//0x34
	WORD wSrvActiveFunc;				//0x36
	WORD wSkill;						//0x38
	WORD wMissile;						//0x3A
};

struct D2SoundsTxtStub
{
	WORD wSound;						//0x00
};

struct D2StorePageTxt
{
	DWORD dwCode;						//0x00
};

struct D2SuperUniquesTxt
{
	WORD wSuperUnique;					//0x00
	WORD wNameStr;						//0x02
	DWORD dwClass;						//0x04
	DWORD dwHcIdx;						//0x08
	DWORD dwMod[3];						//0x0C
	DWORD dwMonSound;					//0x18
	DWORD dwMinGrp;						//0x1C
	DWORD dwMaxGrp;						//0x20
	BYTE nAutopos;						//0x24
	BYTE nEclass;						//0x25
	BYTE nStacks;						//0x26
	BYTE nReplaceable;					//0x27
	BYTE nUtrans[3];					//0x28
	BYTE pad0x2B;						//0x2B
	WORD dwTC[3];						//0x2C
	WORD pad0x32;						//0x32
};

struct D2TreasureClassExTxt
{
	char szTreasureClass[32];			//0x00
	int nPicks;							//0x20
	short nGroup;						//0x24
	short nLevel;						//0x26
	short nMagic;						//0x28
	short nRare;						//0x2A
	short nSet;							//0x2C
	short nUnique;						//0x2E
	short nSuperior;					//0x30
	short nNormal;						//0x32
	int nNoDrop;						//0x34
	char szItem[10][64];				//0x38
	int nProb[10];						//0x2B8
};

struct D2UniqueAppellationTxt
{
	WORD wStringId;						//0x00
};

struct D2UniqueItemsTxt
{
	WORD wId;							//0x00
	char szName[32];					//0x02
	WORD wTblIndex;						//0x22
	WORD wVersion;						//0x24
	WORD pad0x26;						//0x26
	DWORD dwBaseItemCode;				//0x28
	DWORD dwUniqueItemFlags;			//0x2C
	WORD wRarity;						//0x30
	WORD pad0x32;						//0x32
	WORD wLvl;							//0x34
	WORD wLvlReq;						//0x36
	BYTE nChrTransform;					//0x38
	BYTE nInvTransform;					//0x39
	char szFlippyFile[32];				//0x3A
	char szInvFile[32];					//0x5A
	WORD pad0x7A;						//0x7A
	DWORD dwCostMult;					//0x7C
	DWORD dwCostAdd;					//0x80
	WORD wDropSound;					//0x84
	WORD wUseSound;						//0x86
	BYTE nDropSfxFrame;					//0x88
	BYTE pad0x89[3];					//0x89
	D2PropertyStrc pProperties[12];		//0x8C
};

struct D2UniquePrefixTxt
{
	WORD wStringId;						//0x00
};

struct D2UniqueSuffixTxt
{
	WORD wStringId;						//0x00
};

struct D2UniqueTitleTxt
{
	WORD wStringId;						//0x00
};

struct D2WeaponClassTxt
{
	DWORD dwCode;						//0x00
};


////////////////////////////////////////////////////////////
//
//	sgptDataTables

//TODO: see comments
struct D2DataTablesStrc
{
	D2PlayerClassTxt* pPlayerClassTxt;				//0x00
	D2TxtLinkStrc* pPlayerClassLinker;				//0x04
	D2BodyLocsTxt* pBodyLocsTxt;					//0x08
	D2TxtLinkStrc* pBodyLocsLinker;					//0x0C
	D2StorePageTxt* pStorePageTxt;					//0x10
	D2TxtLinkStrc* pStorePageLinker;				//0x14
	D2ElemTypesTxt* pElemTypesTxt;					//0x18
	D2TxtLinkStrc* pElemTypesLinker;				//0x1C
	D2HitClassTxt* pHitClassTxt;					//0x20
	D2TxtLinkStrc* pHitClassLinker;					//0x24
	D2MonModeTxtStub* pMonModeTxtStub;				//0x28
	D2TxtLinkStrc* pMonModeLinker;					//0x2C
	D2PlrModeTxtStub* pPlrModeTxtStub;				//0x30
	D2TxtLinkStrc* pPlrModeLinker;					//0x34
	D2SkillCalcTxt* pSkillCalcTxt;					//0x38
	D2TxtLinkStrc* pSkillCalcLinker;				//0x3C
	char* pSkillsCode;								//0x40
	int nSkillsCodeSize;							//0x44
	int nSkillsCodeSizeEx;							//0x48
	char* pSkillDescCode;							//0x4C
	int nSkillDescCodeSize;							//0x50
	int nSkillDescCodeSizeEx;						//0x54
	D2MissCalcTxt* pMissileCalcTxt;					//0x58
	D2TxtLinkStrc* pMissileCalcLinker;				//0x5C
	char* pMissCode;								//0x60
	int nMissCodeSize;								//0x64
	int nMissCodeSizeEx;							//0x68

	/**/	const char* pSkillCode;                             //+0000006C   052C445C      skillcode.txt (Id from skills.txt)
	/**/	D2TxtLinkStrc* iSkillCode;                       //+00000070   0141F084      skillcode info

	D2EventsTxt* pEventsTxt;						//0x74
	D2TxtLinkStrc* pEventsLinker;					//0x78
	D2CompCodeTxt* pCompCodeTxt;					//0x7C
	D2TxtLinkStrc* pCompCodeLinker;					//0x80
	int nCompCodeTxtRecordCount;					//0x84
	D2MonAiTxt* pMonAiTxt;							//0x88
	D2TxtLinkStrc* pMonAiLinker;					//0x8C
	int nMonAiTxtRecordCount;						//0x90
	D2TxtLinkStrc* pItemsLinker;					//0x94
	char* pItemsCode;								//0x98
	int nItemsCodeSize;								//0x9C
	int nItemsCodeSizeEx;							//0xA0
	D2PropertiesTxt* pPropertiesTxt;				//0xA4
	D2TxtLinkStrc* pPropertiesLinker;				//0xA8
	int nPropertiesTxtRecordCount;					//0xAC
	D2TxtLinkStrc* pRunesLinker;					//0xB0
	D2HireDescTxt* pHireDescTxt;					//0xB4
	D2TxtLinkStrc* pHireDescLinker;					//0xB8
	D2StatesTxt* pStatesTxt;						//0xBC
	D2TxtLinkStrc* pStatesLinker;					//0xC0
	int nStatesTxtRecordCount;						//0xC4
	DWORD* pStateMasks;								//0xC8
	DWORD* fStateMasks[40];							//0xCC
	short* pProgressiveStates;						//0x16C
	int nProgressiveStates;							//0x170
	short* pCurseStates;							//0x174
	int nCurseStates;								//0x178
	short* pTransformStates;						//0x17C
	int nTransformStates;							//0x180
	short* pActionStates;							//0x184
	int nActionStates;								//0x188
	short* pColourStates;							//0x18C
	int nColourStates;								//0x190
	D2SoundsTxtStub* pSoundsTxtCodes;				//0x194
	D2TxtLinkStrc* pSoundsLinker;					//0x198
	int nSoundsTxtCodes;							//0x19C
	D2HirelingTxt* pHirelingTxt;					//0x1A0
	int nHirelingTxtRecordCount;					//0x1A4
	int nClassicHirelingStartRecordIds[256];		//0x1A8
	int nExpansionHirelingStartRecordIds[256];		//0x5A8
	D2NpcTxt* pNpcTxt;								//0x9A8
	int nNpcTxtRecordCount;							//0x9AC
	D2ColorsTxt* pColorsTxt;						//0x9B0
	D2TxtLinkStrc* pColorsLinker;					//0x9B4

	/**/	D2TxtLinkStrc* pTreasureClassExLinker;                 //+000009B8   014C4714      treasureclassex info
	D2TCExShortStrc* pTreasureClassEx;                  //+000009BC   05718D98      treasureclassex.txt (limit = 65534 - autotcs#)
	int nTreasureClassEx;                               //+000009C0   0000043C      # of treasureclassex records
	DWORD* aTreasureClass[45];                          //+000009C4   0571D074      chest treasureclassex list (an array of 45 pointers)

	D2MonStatsTxt* pMonStatsTxt;					//0xA78
	D2TxtLinkStrc* pMonStatsLinker;					//0xA7C
	int nMonStatsTxtRecordCount;					//0xA80
	D2MonSoundsTxt* pMonSoundsTxt;					//0xA84
	D2TxtLinkStrc* pMonSoundsLinker;				//0xA88
	int nMonSoundsTxtRecordCount;					//0xA8C
	D2MonStats2Txt* pMonStats2Txt;					//0xA90
	D2TxtLinkStrc* pMonStats2Linker;				//0xA94
	int nMonStats2TxtRecordCount;					//0xA98
	D2MonPlaceTxt* pMonPlaceTxt;					//0xA9C
	D2TxtLinkStrc* pMonPlaceLinker;					//0xAA0
	int nMonPlaceTxtRecordCount;					//0xAA4
	D2MonPresetTxt* pMonPresetTxt;					//0xAA8
	D2MonPresetTxt* pMonPresetTxtActSections[5];	//0xAAC
	int nMonPresetTxtActRecordCounts[5];			//0xAC0
	D2SuperUniquesTxt* pSuperUniquesTxt;			//0xAD4
	D2TxtLinkStrc* pSuperUniquesLinker;				//0xAD8
	int nSuperUniquesTxtRecordCount;				//0xADC
	short nSuperUniqueIds[66];						//0xAE0
	D2MissilesTxt* pMissilesTxt;					//0xB64
	D2TxtLinkStrc* pMissilesLinker;					//0xB68
	int nMissilesTxtRecordCount;					//0xB6C
	D2MonLvlTxt* pMonLvlTxt;						//0xB70
	int nMonLvlTxtRecordCount;						//0xB74
	D2MonSeqTxt* pMonSeqTxt;						//0xB78
	D2TxtLinkStrc* pMonSeqLinker;					//0xB7C
	int nMonSeqTxtRecordCount;						//0xB80
	D2SeqRecordStrc* pMonSeqTable;					//0xB84
	int nMonSeqTableRecordCount;					//0xB88
	D2SkillDescTxt* pSkillDescTxt;					//0xB8C
	D2TxtLinkStrc* pSkillDescLinker;				//0xB90
	int nSkillDescTxtRecordCount;					//0xB94
	D2SkillsTxt* pSkillsTxt;						//0xB98
	D2TxtLinkStrc* pSkillsLinker;					//0xB9C
	int nSkillsTxtRecordCount;						//0xBA0
	int* nClassSkillCount;							//0xBA4
	int nHighestClassSkillCount;					//0xBA8
	short* nClassSkillList;							//0xBAC
	int nPassiveSkills;								//0xBB0
	WORD* pPassiveSkills;							//0xBB4
	D2TxtLinkStrc* pOverlayLinker;					//0xBB8
	D2OverlayTxt* pOverlayTxt;						//0xBBC
	int nOverlayTxtRecordCount;						//0xBC0
	D2CharStatsTxt* pCharStatsTxt;					//0xBC4
	int nCharStatsTxtRecordCount;					//0xBC8
	D2ItemStatCostTxt* pItemStatCostTxt;			//0xBCC
	D2TxtLinkStrc* pItemStatCostLinker;				//0xBD0
	int nItemStatCostTxtRecordCount;				//0xBD4
	WORD* pStatsWithDescFunc;						//0xBD8
	int nStatsWithDescFunc;							//0xBDC
	D2MonEquipTxt* pMonEquipTxt;					//0xBE0
	int nMonEquipTxtRecordCount;					//0xBE4
	D2PetTypeTxt* pPetTypeTxt;						//0xBE8
	D2TxtLinkStrc* pPetTypeLinker;					//0xBEC
	int nPetTypeTxtRecordCount;						//0xBF0
	D2TxtLinkStrc* pItemTypesLinker;				//0xBF4
	D2ItemTypesTxt* pItemTypesTxt;					//0xBF8
	int nItemTypesTxtRecordCount;					//0xBFC
	int nItemTypesIndex;							//0xC00
	DWORD* pItemTypesNest;							//0xC04
	D2TxtLinkStrc* pSetsLinker;						//0xC08
	D2SetsTxt* pSetsTxt;							//0xC0C
	int nSetsTxtRecordCount;						//0xC10
	D2TxtLinkStrc* pSetItemsLinker;					//0xC14
	D2SetItemsTxt* pSetItemsTxt;					//0xC18
	int nSetItemsTxtRecordCount;					//0xC1C
	D2TxtLinkStrc* pUniqueItemsLinker;				//0xC20
	D2UniqueItemsTxt* pUniqueItemsTxt;				//0xC24
	int nUniqueItemsTxtRecordCount;					//0xC28
	D2TxtLinkStrc* pMonPropLinker;					//0xC2C
	D2MonPropTxt* pMonPropTxt;						//0xC30
	int nMonPropTxtRecordCount;						//0xC34
	D2TxtLinkStrc* pMonTypeLinker;					//0xC38
	D2MonTypeTxt* pMonTypeTxt;						//0xC3C
	int nMonTypeTxtRecordCount;						//0xC40
	DWORD* pMonTypeNest;							//0xC44
	int nMonTypeIndex;								//0xC48
	D2TxtLinkStrc* pMonUModLinker;					//0xC4C
	D2MonUModTxt* pMonUModTxt;						//0xC50
	int nMonUModTxtRecordCount;						//0xC54
	D2LevelsTxt* pLevelsTxt;						//0xC58
	int nLevelsTxtRecordCount;						//0xC5C
	D2LevelDefBin* pLevelDefBin;					//0xC60
	D2LvlPrestTxt* pLvlPrestTxt;					//0xC64
	int nLvlPrestTxtRecordCount;					//0xC68
	int nStuff;										//0xC6C
	int nShiftedStuff;								//0xC70

	void* pAnimData;//FileAnimDataTable* pAnimData;                       //+00000C74   052369C0      sgptAnimTables (see below)

	D2ExperienceTxt* pExperienceTxt;				//0xC78
	D2DifficultyLevelsTxt* pDifficultyLevelsTxt;	//0xC7C
	int nDifficultyLevelsTxtRecordCount;			//0xC80
	DWORD bCompileTxt;								//0xC84



	void* ExpFieldI[6];                           //+00000C88
	DWORD unk0[4];                              //+00000CA0
	void* pExpField;                           //+00000CB0
	void* ExpFieldII[4];                        //+00000CB4

	D2CubeMainTxt* pCubeMainTxt;					//0xCC4
	int nCubeMainTxtRecordCount;					//0xCC8
	int nInventoryTxtRecordCount;					//0xCCC
	D2InventoryTxt* pInventoryTxt;					//0xCD0

	DWORD unk0xCD4;									//0xCD4

	int nItemsTxtRecordCount;						//0xCD8
	D2ItemsTxt* pItemsTxt;							//0xCDC
	D2ItemsTxt* pWeapons;							//0xCE0
	int nWeaponsTxtRecordCount;						//0xCE4
	D2ItemsTxt* pArmor;								//0xCE8
	int nArmorTxtRecordCount;						//0xCEC
	D2ItemsTxt* pMisc;								//0xCF0
	int nMiscTxtRecordCount;						//0xCF4
	WORD* pIndexOldToCurrent;						//0xCF8	
	int nGemsTxtRecordCount;						//0xCFC
	D2GemsTxt* pGemsTxt;							//0xD00
	int nLowQualityItemsTxtRecordCount;				//0xD04
	D2LowQualityItemsTxt* pLowQualityItemsTxt;		//0xD08
	int nBooksTxtRecordCount;						//0xD0C
	D2BooksTxt* pBooksTxt;							//0xD10
	int nRareAffixTxtRecordCount;					//0xD14
	D2RareAffixTxt* pRareAffixTxt;					//0xD18
	D2RareAffixTxt* pRareSuffix;					//0xD1C
	D2RareAffixTxt* pRarePrefix;					//0xD20
	int nItemRatioTxtRecordCount;					//0xD24
	D2ItemRatioTxt* pItemRatioTxt;					//0xD28

	DWORD unk0xD2C;									//0xD2C

	int nGambleTxtRecordCount;						//0xD30
	DWORD* pGambleSelection;						//0xD34
	int pGambleChooseLimit[100];					//0xD38
	int nMagicAffixTxtRecordCount;					//0xEC8
	D2MagicAffixTxt* pMagicAffixTxt;				//0xECC
	D2MagicAffixTxt* pMagicSuffix;					//0xED0
	D2MagicAffixTxt* pMagicPrefix;					//0xED4
	D2MagicAffixTxt* pAutoMagic;					//0xED8
	int nRunesTxtRecordCount;						//0xEDC
	D2RunesTxt* pRunesTxt;							//0xEE0
	int nQualityItemsTxtRecordCount;				//0xEE4
	D2QualityItemsTxt* pQualityItemsTxt;			//0xEE8

	DWORD unk0xEEC;									//0xEEC
	DWORD dwHiSeed;                              //+00000EF0
	DWORD dwLoSeed;                              //+00000EF4

	D2AutomapRandStrc pAutoMapRand[36];				//0xEFC
	D2LvlTypesTxt* pLvlTypesTxt;					//0x1018
	int* pPortalLevels;								//0x101C
	int nPortalLevels;								//0x1020
	int nLvlTypesTxtRecordCount;					//0x1024
	D2LvlWarpTxt* pLvlWarpTxt;						//0x1028
	int nLvlWarpTxtRecordCount;						//0x102C
	D2LvlMazeTxt* pLvlMazeTxt;						//0x1030
	int nLvlMazeTxtRecordCount;						//0x1034
	D2LvlSubTxt* pLvlSubTxt;						//0x1038
	int nLvlSubTxtRecordCount;						//0x103C
	int* pLvlSubTypeStartIds;						//0x1040

	D2UnkLvlDataTableStrc* unk5;                                 //+00001044 *Pointer to 2D-Array ([128][32]) of 4096BYTE-sized struct

	D2UnkLvlSubTypeStrc** ppLvlPrestFiles;			//0x1048
	D2UnkLvlSubTypeStrc** ppLvlSubTypeFiles;		//0x104C

	D2AutomapShortStrc* pAutoMap;					//0x1050
	int nAutoMap;									//0x1054

	void* pMonLink;                              //+00001058

	int nMonItemPercentTxtRecordCount;				//0x105C
	D2MonItemPercentTxt* pMonItemPercentTxt;		//0x1060
	wchar_t wszDefault;								//0x1064
	WORD unk0x1066;									//0x1066
	D2UniqueTitleTxt* pUniqueTitleTxt;				//0x1068
	D2UniquePrefixTxt* pUniquePrefixTxt;			//0x106C
	D2UniqueSuffixTxt* pUniqueSuffixTxt;			//0x1070
	D2UniqueAppellationTxt* pUniqueAppellationTxt;	//0x1074
	int nUniqueTitleTxtRecordCount;					//0x1078
	int nUniquePrefixTxtRecordCount;				//0x107C
	int nUniqueSuffixTxtRecordCount;				//0x1080
	int nUniqueAppellationTxtRecordCount;			//0x1084

	DWORD unk7[2];                              //+00001088
	short unk0x1090;								//0x1090
	short unk0x1092;								//0x1092

	D2ShrinesTxt* pShrinesTxt;						//0x1094
	int nShrinesTxtRecordCount;						//0x1098
	D2ObjectsTxt* pObjectsTxt;						//0x109C
	int nObjectsTxtRecordCount;						//0x10A0
	D2ObjGroupTxt* pObjGroupTxt;					//0x10A4
	int nObjGroupTxtRecordCount;					//0x10A8
	D2ArmTypeTxt* pArmTypeTxt;						//0x10AC
	int nMonModeTxtRecordCount;						//0x10B0
	D2MonModeTxt* pMonModeTxt;						//0x10B4
	D2MonModeTxt* pMonMode[2];						//0x10B8
	D2CompositTxt* pCompositTxt;					//0x10C0
	int nObjModeTypeTxtRecordCount;					//0x10C4
	D2ObjModeTypeTxt* pObjModeTypeTxt;				//0x10C8
	D2ObjModeTypeTxt* pObjType;						//0x10CC
	D2ObjModeTypeTxt* pObjMode;						//0x10D0
	int nPlrModeTypeTxtRecordCount;					//0x10D4
	D2PlrModeTypeTxt* pPlrModeTypeTxt;				//0x10D8
	D2PlrModeTypeTxt* pPlayerType;					//0x10DC
	D2PlrModeTypeTxt* pPlayerMode;					//0x10E0
};

#pragma pack()
