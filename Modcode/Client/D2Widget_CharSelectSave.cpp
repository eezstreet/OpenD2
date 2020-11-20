#include "D2Widget_CharSelectSave.hpp"

// So, funny story. The devs didn't actually tie these to .tbl entries, so they are not translated at all !
// If you look in patchstring.tbl, expansionstring.tbl, and string.tbl, they are not listed at all!
struct CharacterTitle {
	const char16_t* maleTitle;
	const char16_t* femaleTitle;
};

// NOTE: Expansion saves don't actually use every 4th slot. Because it skips over Diablo and increments 2 for Baal.
// Idk. Blizzard coding. (shrug)
#define TITLE_BIGENDER(x, y)	{x, y}, {x, y}, {x, y}, {x, y}
#define TITLE_NOGENDER(x)		{x, x}, {x, x}, {x, x}, {x, x}
#define TITLE_BIGENDER_EXP(x,y)	{x, y}, {x, y}, {x, y}, {x, y}, {x, y}
#define TITLE_NOGENDER_EXP(x)	{x, x}, {x, x}, {x, x}, {x, x}, {x, x}

// Mappings for the class token
static char* gszClassTokens[D2CLASS_MAX] = {
	"AM", "SO", "NE", "PA", "BA", "DZ", "AI",
};

static const CharacterTitle TitleStatus_Classic[] =
{
	// Normal uncompleted = Nothing
	TITLE_NOGENDER(0),

	// Normal completed = "Sir" or "Dame"
	TITLE_BIGENDER(u"Sir", u"Dame"),

	// Nightmare completed = "Lord" or "Lady"
	TITLE_BIGENDER(u"Lord", u"Lady"),

	// Hell completed = "Baron" or "Baronness"
	TITLE_BIGENDER(u"Baron", u"Baronness")
};

static const CharacterTitle TitleStatus_ClassicHardcore[] =
{
	// Normal uncompleted = Nothing
	TITLE_NOGENDER(0),

	// Normal completed = "Count" or "Countess"
	TITLE_BIGENDER(u"Count", u"Countess"),

	// Nightmare completed = "Duke" or "Duchess"
	TITLE_BIGENDER(u"Duke", u"Duchess"),

	// Hell completed = "King" or "Queen"
	TITLE_BIGENDER(u"King", u"Queen")
};

static const CharacterTitle TitleStatus_Expansion[] =
{
	// Normal uncompleted = Nothing
	TITLE_NOGENDER_EXP(0),

	// Normal completed = "Slayer"
	TITLE_NOGENDER_EXP(u"Slayer"),

	// Nightmare completed = "Champion"
	TITLE_NOGENDER_EXP(u"Champion"),

	// Hell completed = "Patriarch" or "Matriarch"
	TITLE_BIGENDER_EXP(u"Patriarch", u"Matriarch")
};

static const CharacterTitle TitleStatus_ExpansionHardcore[] =
{
	// Normal uncompleted = Nothing
	TITLE_NOGENDER_EXP(0),

	// Normal completed = "Destroyer"
	TITLE_NOGENDER_EXP(u"Destroyer"),

	// Nightmare completed = "Conqueror"
	TITLE_NOGENDER_EXP(u"Conqueror"),

	// Hell completed = "Guardian"
	TITLE_NOGENDER_EXP(u"Guardian")
};

struct AppearanceEntry
{
	const char* code;
	D2WeaponClass wc;
};

static AppearanceEntry AppearanceForByte[] =
{
	{ "", WC_NONE }, // null
	{ "lit", WC_NONE },
	{ "med", WC_NONE },
	{ "hvy", WC_NONE },
	{ "cap", WC_NONE },
	{ "hlm", WC_NONE },
	{ "fhl", WC_NONE },
	{ "ghm", WC_NONE },
	{ "crn", WC_NONE },
	{ "msk", WC_NONE },
	{ "cpe", WC_NONE },
	{ "clk", WC_NONE },
	{ "rob", WC_NONE },
	{ "qui", WC_NONE },
	{ "lea", WC_NONE },
	{ "hla", WC_NONE },
	{ "stu", WC_NONE },
	{ "rng", WC_NONE },
	{ "scl", WC_NONE },
	{ "chn", WC_NONE },
	{ "brs", WC_NONE },
	{ "spl", WC_NONE },
	{ "plt", WC_NONE },
	{ "fld", WC_NONE },
	{ "gth", WC_NONE },
	{ "ful", WC_NONE },
	{ "buc", WC_NONE },
	{ "lrg", WC_NONE },
	{ "kit", WC_NONE },
	{ "tow", WC_NONE },
	{ "lgl", WC_NONE },
	{ "mgl", WC_NONE },
	{ "hgl", WC_NONE },
	{ "lbt", WC_NONE },
	{ "mbt", WC_NONE },
	{ "hbt", WC_NONE },
	{ "lbl", WC_NONE },
	{ "mbl", WC_NONE },
	{ "hbl", WC_NONE },
	{ "bhm", WC_NONE },
	{ "bsh", WC_NONE },
	{ "spk", WC_NONE },
	{ "wnd", WC_1HS },
	{ "ywn", WC_1HS },
	{ "bwn", WC_1HS },
	{ "clb", WC_1HS },
	{ "hax", WC_1HS },
	{ "ssd", WC_1HS },
	{ "scm", WC_1HS },
	{ "mac", WC_1HS },
	{ "axe", WC_1HS },
	{ "flc", WC_1HS },
	{ "lsd", WC_1HS },
	{ "fla", WC_1HS },
	{ "hdm", WC_1HS },
	{ "whm", WC_1HS },
	{ "crs", WC_1HS },
	{ "fls", WC_1HT },
	{ "gpl", WC_1HT },
	{ "gps", WC_1HT },
	{ "opl", WC_1HT },
	{ "ops", WC_1HT },
	{ "dgr", WC_1HT },
	{ "d33", WC_1HT },
	{ "g33", WC_1HT },
	{ "dir", WC_1HT },
	{ "jav", WC_1HT },
	{ "glv", WC_1HT },
	{ "pil", WC_1HT },
	{ "clm", WC_2HS },
	{ "gsd", WC_2HS },
	{ "spr", WC_2HT },
	{ "tri", WC_2HT },
	{ "pik", WC_2HT },
	{ "spr", WC_2HT }, // duplicate?
	{ "sbw", WC_BOW },
	{ "lbw", WC_BOW },
	{ "sbb", WC_BOW },
	{ "lbb", WC_BOW },
	{ "sst", WC_STF },
	{ "lst", WC_STF },
	{ "cst", WC_STF },
	{ "bst", WC_STF },
	{ "lax", WC_STF },
	{ "scy", WC_STF },
	{ "gix", WC_STF },
	{ "btx", WC_STF },
	{ "hal", WC_STF },
	{ "mau", WC_STF },
	{ "pax", WC_STF },
	{ "hal", WC_STF }, // duplicate?
	{ "hxb", WC_XBW },
	{ "lxb", WC_XBW },
	{ "tax", WC_1HS },
	{ "spc", WC_1HS },
	{ "sbr", WC_1HS },
	{ "mst", WC_1HS },
	{ "scp", WC_1HS },
	{ "gpm", WC_1HT },
	{ "opm", WC_1HT },
	{ "tkf", WC_1HT },
	{ "bld", WC_1HT },
	{ "bsd", WC_1HS },
	{ "bsw", WC_2HS },
	{ "2hs", WC_2HS },
	{ "brn", WC_2HT },
	{ "hbw", WC_BOW },
	{ "cbw", WC_BOW },
	{ "swb", WC_BOW },
	{ "lwb", WC_BOW },
	{ "wst", WC_STF },
	{ "bax", WC_STF },
	{ "bar", WC_STF },
	{ "gax", WC_STF },
	{ "rxb", WC_XBW },
	{ "mxb", WC_XBW },
	{ "zbt", WC_NONE },
	{ "aar", WC_NONE },
	{ "ltp", WC_NONE },
	{ "sml", WC_NONE },
	{ "gts", WC_NONE },
	{ "aqv", WC_NONE },
	{ "cqv", WC_NONE },
	{ "vbl", WC_NONE },
	{ "tbl", WC_NONE },
	{ "vgl", WC_NONE },
	{ "tgl", WC_NONE },
	{ "wbt", WC_NONE },
	{ "tbt", WC_NONE },
	{ "flb", WC_2HS },
	{ "mpi", WC_1HS },
	{ "bal", WC_1HS },
	{ "leg", WC_1HS },
	{ "qlt", WC_NONE },
	{ "2ax", WC_1HS },
	{ "wax", WC_1HS },
	{ "gwn", WC_1HS },
	{ "gma", WC_STF },
	{ "wsd", WC_1HS },
	{ "gis", WC_2HS },
	{ "bld", WC_1HT }, // duplicate?
	{ "bkf", WC_1HT },
	{ "ssp", WC_1HT },
	{ "tsp", WC_1HT },
	{ "spe", WC_2HT }, // supposed to be Spetum? Spear? but incorrect code
	{ "vou", WC_STF },
	{ "wsc", WC_STF },
	{ "kri", WC_1HT },
	{ "spt", WC_2HT },
	{ "gsc", WC_1HS },
	{ "wsp", WC_1HS },
	{ "hst", WC_STF },
	{ "msf", WC_STF },
	{ "hfh", WC_1HS },
	{ "9ha", WC_1HS },
	{ "9ax", WC_1HS },
	{ "92a", WC_1HS },
	{ "9mp", WC_1HS },
	{ "9wa", WC_1HS },
	{ "9la", WC_STF },
	{ "9ba", WC_STF },
	{ "9bt", WC_STF },
	{ "9ga", WC_STF },
	{ "9gi", WC_STF },
	{ "9wn", WC_1HS },
	{ "9yw", WC_1HS },
	{ "9bw", WC_1HS },
	{ "9gw", WC_1HS },
	{ "9cl", WC_1HS },
	{ "9sc", WC_1HS },
	{ "9qs", WC_1HS },
	{ "9ws", WC_1HS },
	{ "9sp", WC_1HS },
	{ "9ma", WC_1HS },
	{ "9mt", WC_1HS },
	{ "9fl", WC_1HS },
	{ "9wh", WC_1HS },
	{ "9m9", WC_STF },
	{ "9gm", WC_STF },
	{ "9ss", WC_1HS },
	{ "9sm", WC_1HS },
	{ "9sb", WC_1HS },
	{ "9fc", WC_1HS },
	{ "9cr", WC_1HS },
	{ "9bs", WC_1HS },
	{ "9ls", WC_1HS },
	{ "9wd", WC_1HS },
	{ "92h", WC_2HS },
	{ "9cm", WC_2HS },
	{ "9gs", WC_2HS },
	{ "9b9", WC_2HS },
	{ "9fb", WC_2HS },
	{ "9gd", WC_2HS },
	{ "9dg", WC_1HT },
	{ "9di", WC_1HT },
	{ "9kr", WC_1HT },
	{ "9bl", WC_1HT },
	{ "9tk", WC_1HT },
	{ "9ta", WC_1HS },
	{ "9bk", WC_1HT },
	{ "9b8", WC_1HS },
	{ "9ja", WC_1HT },
	{ "9pi", WC_1HT },
	{ "9s9", WC_1HT },
	{ "9gl", WC_1HT },
	{ "9ts", WC_1HT },
	{ "9sr", WC_2HT },
	{ "9tr", WC_2HT },
	{ "9br", WC_2HT },
	{ "9st", WC_2HT },
	{ "9p9", WC_2HT },
	{ "9b7", WC_STF },
	{ "9vo", WC_STF },
	{ "9s8", WC_STF },
	{ "9pa", WC_STF },
	{ "9h9", WC_STF },
	{ "9wc", WC_STF },
	{ "8ss", WC_STF },
	{ "8ls", WC_STF },
	{ "8cs", WC_STF },
	{ "8bs", WC_STF },
	{ "8ws", WC_STF },
	{ "8sb", WC_BOW },
	{ "8hb", WC_BOW },
	{ "8lb", WC_BOW },
	{ "8cb", WC_BOW },
	{ "8s8", WC_BOW },
	{ "8l8", WC_BOW },
	{ "8sw", WC_BOW },
	{ "8lw", WC_BOW },
	{ "8lx", WC_XBW },
	{ "8mx", WC_XBW },
	{ "8hx", WC_XBW },
	{ "8rx", WC_XBW },
	{ "xap", WC_NONE },
	{ "xkp", WC_NONE },
	{ "xlm", WC_NONE },
	{ "xhl", WC_NONE },
	{ "xhm", WC_NONE },
	{ "xrn", WC_NONE },
	{ "xsk", WC_NONE },
	{ "xui", WC_NONE },
	{ "xea", WC_NONE },
	{ "xla", WC_NONE },
	{ "xtu", WC_NONE },
	{ "xng", WC_NONE },
	{ "xcl", WC_NONE },
	{ "xhn", WC_NONE },
	{ "xrs", WC_NONE },
	{ "xpl", WC_NONE },
	{ "xlt", WC_NONE },
	{ "xld", WC_NONE },
	{ "xth", WC_NONE },
	{ "xul", WC_NONE },
	{ "xar", WC_NONE },
	{ "xtp", WC_NONE },
	{ "xuc", WC_NONE },
	{ "xml", WC_NONE },
	{ "xrg", WC_NONE },
	{ "xit", WC_NONE },
	{ "xow", WC_NONE },
	{ "xts", WC_NONE },
	{ "xlg", WC_NONE },
	{ "xvg", WC_NONE },
	{ "xmg", WC_NONE },
	{ "xtg", WC_NONE },
	{ "xhg", WC_NONE },
	{ "xlb", WC_NONE },
	{ "xvb", WC_NONE },
	{ "xmb", WC_NONE },
	{ "xtb", WC_NONE },
	{ "xhb", WC_NONE },
	{ "zlb", WC_NONE },
	{ "zvb", WC_NONE },
	{ "zmb", WC_NONE },
	{ "ztb", WC_NONE },
	{ "zhb", WC_NONE },
	{ "xh9", WC_NONE },
	{ "xsh", WC_NONE },
	{ "xpk", WC_NONE },
	{ "qf1", WC_1HS },
	{ "qf2", WC_1HS },
		// FIXME: how are class specific items handled...?
};

D2Widget_CharSelectSave::D2Widget_CharSelectSave(const char* characterSave, D2SaveHeader& header)
{
	bool bClassMale = Client_classMale(header.nCharClass);

	D2Lib::strncpyz(path, characterSave, MAX_D2PATH_ABSOLUTE);
	D2Lib::qmbtowc(charName, sizeof(charName), header.szCharacterName);
	D2Lib::qsnprintf(charClassAndLevel, 32, u"%s %s",
		engine->TBL_FindStringFromIndex(5017),
		Client_className(header.nCharClass));
	D2Lib::qsnprintf(charClassAndLevel, 32, charClassAndLevel, header.nCharLevel);
	saveHeader = header;

	nextInChain = nullptr;
	bIsSelected = false;
	bHasTitle = bIsDeadHardcore = bIsExpansion = bIsHardcore = false;

	selectionFrame = engine->renderer->AllocateObject(0);
	characterPreview = engine->renderer->AllocateObject(0);
	characterTitle = engine->renderer->AllocateObject(1);
	characterName = engine->renderer->AllocateObject(1);
	characterLevelAndClass = engine->renderer->AllocateObject(1);
	expansionText = engine->renderer->AllocateObject(1);
	token = engine->graphics->CreateReference(TOKEN_CHAR, gszClassTokens[header.nCharClass]);
	characterRender = engine->renderer->AllocateObject(1);
	characterRender->AttachTokenResource(token);
	// for each comp, set the armor class
	characterRender->SetTokenHitClass(WC_HTH);
	characterRender->SetTokenMode(PLRMODE_TN);
	for (int i = 0; i < COMP_MAX; i++)
	{
		if (i != COMP_HEAD)
		{
			characterRender->SetTokenArmorLevel(i, AppearanceForByte[header.nAppearance[i]].code);
		}
		else
		{
			characterRender->SetTokenArmorLevel(i, "lit");
		}
	}

	// set our weapon class to be based on what the weapon is
	int leftWeaponClass = AppearanceForByte[header.nAppearance[COMP_LEFTHAND]].wc;
	int rightWeaponClass = AppearanceForByte[header.nAppearance[COMP_RIGHTHAND]].wc;
	int desiredWeaponClass = WC_HTH;
	if ((leftWeaponClass == WC_HT1 || leftWeaponClass == WC_HT2) && header.nCharClass != D2CLASS_ASSASSIN)
	{
		// sorry, this is an assassin only item
		leftWeaponClass = WC_NONE;
	}
	if ((rightWeaponClass == WC_HT1 || rightWeaponClass == WC_HT2) && header.nCharClass != D2CLASS_ASSASSIN)
	{
		// sorry, this is an assassin only item
		rightWeaponClass = WC_NONE;
	}

	// This is totally fucked in D2Win.dll. The ordering there is:
	// WC_NONE
	// WC_HTH
	// WC_1HT
	// WC_2HT
	// WC_1HS
	// WC_2HS
	// WC_BOW
	// WC_XBW
	// WC_STF
	// WC_1JS
	// WC_1JT
	// WC_1SS
	// WC_1ST
	// WC_HT1
	// WC_HT2

	if (leftWeaponClass != WC_NONE)
	{
		if (leftWeaponClass == rightWeaponClass && (leftWeaponClass == WC_BOW || leftWeaponClass == WC_XBW))
		{
			desiredWeaponClass = leftWeaponClass;
		}
		else if (leftWeaponClass == WC_STF)
		{
			desiredWeaponClass = WC_STF;
		}
		else if (rightWeaponClass == WC_NONE)
		{
			desiredWeaponClass = leftWeaponClass;
		}
		else if (leftWeaponClass == WC_1HS || leftWeaponClass == WC_2HS)
		{
			if (rightWeaponClass == WC_1HS || rightWeaponClass == WC_2HS)
			{
				desiredWeaponClass = WC_1SS;
			}
			else if (rightWeaponClass == WC_1HT)
			{
				desiredWeaponClass = WC_1JS;
			}
			else if (rightWeaponClass == WC_NONE && leftWeaponClass == WC_2HS)
			{
				desiredWeaponClass = WC_2HS;
			}
			else
			{
				desiredWeaponClass = leftWeaponClass;
			}
		}
		else if (leftWeaponClass == WC_1HT)
		{
			if (rightWeaponClass == WC_1HS || rightWeaponClass == WC_2HS)
			{
				desiredWeaponClass = WC_1JS;
			}
			else if (rightWeaponClass == WC_1HT)
			{
				desiredWeaponClass = WC_1JT;
			}
			else
			{
				desiredWeaponClass = leftWeaponClass;
			}
		}
		else
		{
			desiredWeaponClass = leftWeaponClass;
		}
	}
	else
	{
		desiredWeaponClass = rightWeaponClass;
	}

	// special case, assassin gets rendered a lil differently
	// this is a massive ugly hack to avoid loading the item tables right now
	if (header.nCharClass == D2CLASS_ASSASSIN && desiredWeaponClass == WC_1SS)
	{
		switch (header.nAppearance[COMP_LEFTHAND])
		{
		case 43:
			characterRender->SetTokenArmorLevel(COMP_LEFTHAND, "ktr");
			break;
		case 44:
			characterRender->SetTokenArmorLevel(COMP_LEFTHAND, "axf");
			break;
		case 45:
			characterRender->SetTokenArmorLevel(COMP_LEFTHAND, "clw");
			break;
		case 46:
			characterRender->SetTokenArmorLevel(COMP_LEFTHAND, "skr");
			break;
		}
		switch (header.nAppearance[COMP_RIGHTHAND])
		{
		case 43:
			characterRender->SetTokenArmorLevel(COMP_RIGHTHAND, "ktr");
			break;
		case 44:
			characterRender->SetTokenArmorLevel(COMP_RIGHTHAND, "axf");
			break;
		case 45:
			characterRender->SetTokenArmorLevel(COMP_RIGHTHAND, "clw");
			break;
		case 46:
			characterRender->SetTokenArmorLevel(COMP_RIGHTHAND, "skr");
			break;
		}
		desiredWeaponClass = WC_HT2;
	}
	characterRender->SetTokenHitClass(desiredWeaponClass);

	characterTitle->AttachFontResource(cl.font16);
	characterName->AttachFontResource(cl.font16);
	characterName->SetText(charName);
	characterLevelAndClass->AttachFontResource(cl.font16);
	characterLevelAndClass->SetText(charClassAndLevel);
	expansionText->AttachFontResource(cl.font16);

	// Check to see if we're hardcore.
	if (header.nCharStatus & (1 << D2STATUS_HARDCORE))
	{
		bIsHardcore = true;
		if (header.nCharClass & (1 << D2STATUS_DEAD))
		{
			bIsDeadHardcore = true;
		}

		characterTitle->SetTextColor(TextColor_Red);
	}
	else
	{
		characterTitle->SetTextColor(TextColor_Gold);
	}

	// Handle the EXPANSION text (if present)
	if (header.nCharStatus & (1 << D2STATUS_EXPANSION))
	{
		bIsExpansion = true;
		expansionText->SetTextColor(TextColor_BrightGreen);
		expansionText->SetText(u"EXPANSION CHARACTER"); // HACK
	}

	// Handle the title.
	if (header.nCharTitle != 0)
	{
		bHasTitle = true;

		if (bIsExpansion)
		{
			if (bIsHardcore)
			{
				characterTitle->SetText(bClassMale ?
					TitleStatus_ExpansionHardcore[header.nCharTitle].maleTitle :
					TitleStatus_ExpansionHardcore[header.nCharTitle].femaleTitle);
			}
			else
			{
				characterTitle->SetText(bClassMale ?
					TitleStatus_Expansion[header.nCharTitle].maleTitle :
					TitleStatus_Expansion[header.nCharTitle].femaleTitle);
			}
		}
		else if(bIsHardcore)
		{
			characterTitle->SetText(bClassMale ?
				TitleStatus_ClassicHardcore[header.nCharTitle].maleTitle :
				TitleStatus_ClassicHardcore[header.nCharTitle].femaleTitle);
		}
		else
		{
			characterTitle->SetText(bClassMale ?
				TitleStatus_Classic[header.nCharTitle].maleTitle :
				TitleStatus_Classic[header.nCharTitle].femaleTitle);
		}
	}

	// Attach the frame.
	IGraphicsReference* regularFrame = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\charselectbox.dc6",
		UsagePolicy_Permanent);
	IGraphicsReference* grayFrame = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\charselectboxgrey.dc6",
		UsagePolicy_Permanent);

	if (bIsDeadHardcore)
	{	// dead hardcore characters have a gray frame. we can't go ingame with them but we can see them here
		selectionFrame->AttachCompositeTextureResource(grayFrame, 0, 1);
		characterPreview->SetColorModulate(1.0f, 1.0f, 1.0f, 0.75f);
	}
	else
	{
		selectionFrame->AttachCompositeTextureResource(regularFrame, 0, 1);
	}
}

D2Widget_CharSelectSave::~D2Widget_CharSelectSave()
{
	if (nextInChain)
	{
		delete nextInChain;
	}
	engine->renderer->Remove(selectionFrame);
	engine->renderer->Remove(characterPreview);
	engine->renderer->Remove(characterTitle);
	engine->renderer->Remove(characterName);
	engine->renderer->Remove(characterLevelAndClass);
	engine->renderer->Remove(expansionText);
	engine->renderer->Remove(characterRender);
}

void D2Widget_CharSelectSave::SetDrawPosition(uint32_t x, uint32_t y)
{
	this->x = x;
	this->y = y;
	characterPreview->SetDrawCoords(x + 10, y + 12, 0, 0);
	characterName->SetDrawCoords(x + 80, y + 14, 0, 0);
	characterLevelAndClass->SetDrawCoords(x + 80, y + 28, 0, 0);
	expansionText->SetDrawCoords(x + 80, y + 42, 0, 0);
	characterRender->SetDrawCoords(x + 25, y + 80, 0, 0);
	characterTitle->SetDrawCoords(x + 80, y, 0, 0);
	selectionFrame->SetDrawCoords(x, y, -1, -1);
}

void D2Widget_CharSelectSave::Draw()
{
	// if we are selected, draw the selection frame
	if (bIsSelected)
	{
		selectionFrame->Draw();
	}

	// draw this stuff always (character preview, name, level and class)
	characterPreview->Draw();
	characterName->Draw();
	characterLevelAndClass->Draw();
	characterRender->Draw();

	// draw the title if the character has one
	if (bHasTitle)
	{
		characterTitle->Draw();
	}

	// draw expansion text if the character is expansion
	if (bIsExpansion)
	{
		expansionText->Draw();
	}
}

bool D2Widget_CharSelectSave::CheckMouseDownInChain(DWORD dwX, DWORD dwY, int& counter)
{
	if (1)
	{	// mouse down...
		return true;
	}

	counter--;

	if (!nextInChain || counter <= 0)
	{
		return false;
	}

	return nextInChain->CheckMouseDownInChain(dwX, dwY, counter);
}

bool D2Widget_CharSelectSave::CheckMouseClickInChain(DWORD dwX, DWORD dwY, int& counter)
{
	if (1)
	{	// mouse clicked...
		return true;
	}

	counter--;

	if (!nextInChain || counter <= 0)
	{
		return false;
	}

	
	return nextInChain->CheckMouseClickInChain(dwX, dwY, counter);
}

void D2Widget_CharSelectSave::SetNextInChain(D2Widget_CharSelectSave* next)
{
	nextInChain = next;
}

D2Widget_CharSelectSave* D2Widget_CharSelectSave::GetInChain(int counter)
{
	if (counter == 0 || nextInChain == nullptr)
	{
		return this;
	}
	return nextInChain->GetInChain(counter - 1);
}

void D2Widget_CharSelectSave::DrawLink(int counter, bool bDrawLeft)
{
	// set parameters
	SetDrawPosition(bDrawLeft ? 35 : 305, 85 + (((D2_NUM_VISIBLE_SAVES - counter) % 2) * 95));

	// draw it!
	Draw();

	// decrement counter, draw next link in chain
	counter--;
	if (counter <= 0 || nextInChain == nullptr)
	{
		return;
	}

	nextInChain->DrawLink(counter, !bDrawLeft);
}

void D2Widget_CharSelectSave::Select(int counter)
{
	if (counter <= 0 || nextInChain == nullptr)
	{
		OnSelected();
		return;
	}

	nextInChain->Select(counter - 1);
}

void D2Widget_CharSelectSave::OnSelected()
{
	bIsSelected = true;
}

void D2Widget_CharSelectSave::DeselectAllInChain()
{
	bIsSelected = false;
	if (nextInChain)
	{
		nextInChain->DeselectAllInChain();
	}
}

char16_t* D2Widget_CharSelectSave::GetSelectedCharacterName()
{
	if (bIsSelected)
	{
		return charName;
	}

	if (nextInChain)
	{
		return nextInChain->GetSelectedCharacterName();
	}

	return u"";
}