#include "D2Widget_CharSelectList.hpp"
#include "D2Panel_CharSelect.hpp"

#define D2_NUM_VISIBLE_SAVES	8

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
static const char* gszClassTokens[D2CLASS_MAX] = {
	"AM", "SO", "NE", "PA", "BA", "DZ", "AI",
};

static const CharacterTitle TitleStatus_Classic[] =
{
	// Normal uncompleted = Nothing
	TITLE_NOGENDER(nullptr),

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
	TITLE_NOGENDER(nullptr),

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
	TITLE_NOGENDER_EXP(nullptr),

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
	TITLE_NOGENDER_EXP(nullptr),

	// Normal completed = "Destroyer"
	TITLE_NOGENDER_EXP(u"Destroyer"),

	// Nightmare completed = "Conqueror"
	TITLE_NOGENDER_EXP(u"Conqueror"),

	// Hell completed = "Guardian"
	TITLE_NOGENDER_EXP(u"Guardian")
};

/*
 *	Creates a Character Select list widget.
 *	This method is responsible for loading up all of the savegames.
 *	We should maybe cache the results of the savegame loading so that going to the charselect page doesn't take a while.
 */
D2Widget_CharSelectList::D2Widget_CharSelectList(int x, int y, int w, int h) 
	: D2Widget(x, y, w, h)
{
	// Blank out our own data
	pCharacterData = nullptr;
	nNumberSaves = 0;
	nCurrentScroll = 0;
	nCurrentSelection = -1;

	// Create the scrollbar - we manually draw it as part of this widget's display
	//pScrollBar = new D2Widget_Scrollbar()

	frameHandle = engine->renderer->TextureFromStitchedDC6("data/global/ui/CharSelect/charselectbox.dc6",
		"charselectbox", 0, 1, PAL_UNITS);
	greyFrameHandle = engine->renderer->TextureFromStitchedDC6("data/global/ui/CharSelect/charselectboxgrey.dc6",
		"charselectboxgrey", 0, 1, PAL_UNITS);
	engine->renderer->SetTextureBlendMode(frameHandle, BLEND_ALPHA);
	engine->renderer->SetTextureBlendMode(greyFrameHandle, BLEND_ALPHA);
}

/*
 *	Destroys the character select list widget
 */
D2Widget_CharSelectList::~D2Widget_CharSelectList()
{
	// Free out the entire linked list
	CharacterSaveData* pCurrent = pCharacterData;
	while (pCurrent != nullptr)
	{
		CharacterSaveData* pNext = pCurrent->pNext;
		if (pCurrent->tokenInstance != INVALID_HANDLE)
		{
			engine->TOK_DestroyTokenInstance(pCurrent->tokenInstance);
		}
		free(pCurrent);
		pCurrent = pNext;
	}
}

/*
 *	Add a savegame to the list
 */
void D2Widget_CharSelectList::AddSave(D2SaveHeader& header, char* path)
{
	// Allocate a character save entry
	auto* pSaveData = (CharacterSaveData*)malloc(sizeof(CharacterSaveData));

	// Copy the path, name, and header data
	D2Lib::strncpyz(pSaveData->path, path, MAX_D2PATH_ABSOLUTE);
	D2Lib::qmbtowc(pSaveData->name, 16, header.szCharacterName);
	pSaveData->header = header;

	// Register the animations for it.
	// TODO: use the actual anims (it just uses hth, lit, no weapon for now..)
	pSaveData->token = engine->TOK_Register(TOKEN_CHAR, gszClassTokens[header.nCharClass], "hth");
	pSaveData->tokenInstance = engine->TOK_CreateTokenAnimInstance(pSaveData->token);
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_HEAD, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_LEFTARM, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_LEGS, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_RIGHTARM, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_SHIELD, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_SPECIAL1, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_SPECIAL2, "lit");
	engine->TOK_SetTokenInstanceComponent(pSaveData->tokenInstance, COMP_TORSO, "lit");
	engine->TOK_SetTokenInstanceMode(pSaveData->tokenInstance, PLRMODE_TN);
	engine->TOK_SetTokenInstanceDirection(pSaveData->tokenInstance, 4);
	engine->TOK_SetInstanceActive(pSaveData->tokenInstance, true);	// always set it as active so scrolling is smooth

	// Add it to the linked list
	pSaveData->pNext = pCharacterData;
	pCharacterData = pSaveData;

	// Increment the save count.
	nNumberSaves++;
}

/*
 *	This widget got added to the panel. Let's go ahead and tell the parent what we have selected.
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::OnWidgetAdded()
{
	Selected(nCurrentSelection);
}

/*
 *	Draws a Character Select list widget.
 */
void D2Widget_CharSelectList::Draw()
{
	// Draw the savegames
	// This is pretty horrendously inefficient.
	// But it doesn't need to be super efficient, considering it's only rendering 8 savegames at a time!
	int i = 0;
	CharacterSaveData* pCurrent = pCharacterData;
	while (pCurrent != nullptr && i < nCurrentScroll + D2_NUM_VISIBLE_SAVES)
	{
		if (i >= nCurrentScroll)
		{
			// Draw the savegame!
			DrawSaveSlot(pCurrent, i - nCurrentScroll);
		}
		i++;
		pCurrent = pCurrent->pNext;
	}

	// Draw the scrollbar
}

/*
 *	Draws a CharacterSaveData on a slot in the display.
 */
void D2Widget_CharSelectList::DrawSaveSlot(D2Widget_CharSelectList::CharacterSaveData* pSaveData, int nSlot)
{
	// It draws a visual representation of the character on the left side of each slot, based on what the savegame says.
	bool bRightSlot = (nSlot % 2) > 0;
	int nSlotY = nSlot / 2;
	int nX, nY;
	const char16_t* szCharacterTitle;
	char16_t szCharacterLevelClass[32]{ 0 };
	char16_t szDisplayString[32]{ 0 };
	bool bClassMale = Client_classMale(pSaveData->header.nCharClass);

	// If this save slot is the selected one, draw the frame
	if (nCurrentSelection == nSlot)
	{
		if ((pSaveData->header.nCharStatus & (1u << D2STATUS_HARDCORE)) &&
			(pSaveData->header.nCharStatus & (1u << D2STATUS_DEAD)))
		{
			// Dead hardcore player gets a grey frame
			engine->renderer->DrawTexture(greyFrameHandle, (bRightSlot ? x : x + nSlotWidth), nSlotY * nSlotHeight,
				nSlotWidth, nSlotHeight, 0, 0);
		}
		else
		{
			// Use the normal frame
			engine->renderer->DrawTexture(frameHandle, (bRightSlot ? x + nSlotWidth : x), y + (nSlotY * nSlotHeight),
				nSlotWidth, nSlotHeight, 0, 0);
		}
	}

	// On the right, it draws text information:
	//		Character Title (if present)
	//		Character Name
	//		Level X <Character Class>
	//		EXPANSION CHARACTER (if it's flagged as being an expansion character)
	nX = x + (bRightSlot ? nSlotWidth : 0) + 76;
	nY = y + (nSlotY * nSlotHeight) + 4;

	// Draw character title. The title corresponds to the number of acts completed. EXCEPT IN THE EXPANSION
	// Set font color to be gold. Or red if this is a hardcore character.
	if (pSaveData->header.nCharStatus & (1u << D2STATUS_HARDCORE))
	{
		engine->renderer->ColorModFont(cl.font16, 186, 102, 100);
	}
	else
	{
		engine->renderer->ColorModFont(cl.font16, 171, 156, 135);
	}

	if (pSaveData->header.nCharStatus & (1u << D2STATUS_EXPANSION))
	{
		if (pSaveData->header.nCharStatus & (1u << D2STATUS_HARDCORE))
		{
			// Pull from the Expansion-Hardcore table
			szCharacterTitle = bClassMale ?
				TitleStatus_ExpansionHardcore[pSaveData->header.nCharTitle].maleTitle :
				TitleStatus_ExpansionHardcore[pSaveData->header.nCharTitle].femaleTitle;
		}
		else
		{
			// Pull from the Expansion table
			szCharacterTitle = bClassMale ?
				TitleStatus_Expansion[pSaveData->header.nCharTitle].maleTitle :
				TitleStatus_Expansion[pSaveData->header.nCharTitle].femaleTitle;
		}
	}
	else if (pSaveData->header.nCharStatus & (1u << D2STATUS_HARDCORE))
	{
		// Pull from the Classic-Hardcore table
		szCharacterTitle = bClassMale ?
			TitleStatus_ClassicHardcore[pSaveData->header.nCharTitle].maleTitle :
			TitleStatus_ClassicHardcore[pSaveData->header.nCharTitle].femaleTitle;
	}
	else
	{
		// Pull from the Classic table
		szCharacterTitle = bClassMale ?
			TitleStatus_Classic[pSaveData->header.nCharTitle].maleTitle :
			TitleStatus_Classic[pSaveData->header.nCharTitle].femaleTitle;
	}

	if (szCharacterTitle && szCharacterTitle[0])
	{
		// Draw the character title if we have one.
		engine->renderer->DrawText(cl.font16, (char16_t*)szCharacterTitle, nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
	}
	nY += 15;

	// Draw character name
	engine->renderer->DrawText(cl.font16, pSaveData->name, nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
	nY += 15;

	// Draw character level and class
	engine->renderer->ColorModFont(cl.font16, 255, 255, 255);
	// Format it so that it will read "Level %d <Class>
	D2Lib::qsnprintf(szCharacterLevelClass, 32, u"%s %s",
		engine->TBL_FindStringFromIndex(5017),
		Client_className(pSaveData->header.nCharClass));
	// Reformat it again so that the level is filled in
	D2Lib::qsnprintf(szDisplayString, 32, szCharacterLevelClass, pSaveData->header.nCharLevel);
	// Now draw it!
	engine->renderer->DrawText(cl.font16, szDisplayString, nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
	nY += 15;

	// Draw whether this is an expansion character
	if (pSaveData->header.nCharStatus & (1u << D2STATUS_EXPANSION))
	{
		engine->renderer->ColorModFont(cl.font16, 65, 200, 50);
		engine->renderer->DrawText(cl.font16, engine->TBL_FindStringFromIndex(22731), nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
	}
	engine->renderer->ColorModFont(cl.font16, 255, 255, 255);

	// Draw the token instance
	engine->renderer->DrawTokenInstance(pSaveData->tokenInstance, nX - 40, nY + 30, 0, PAL_UNITS);
}

/*
 *	Returns the name of the currently selected character.
 *	@author	eezstreet
 */
char16_t* D2Widget_CharSelectList::GetSelectedCharacterName()
{
	int i = 0;
	CharacterSaveData* pCharacterSave = pCharacterData;

	if (nCurrentSelection == -1)
	{
		return (char16_t *)"";
	}

	while (i != nCurrentSelection && pCharacterSave != nullptr)
	{
		pCharacterSave = pCharacterSave->pNext;
		i++;
	}

	if (i == nCurrentSelection)
	{
		return pCharacterSave->name;
	}

        return (char16_t *)"";
}

/*
 *	Handles a mouse-down event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseDown(DWORD dwX, DWORD dwY)
{
        return dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h;
}

/*
 *	Handles a mouse click event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseClick(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		Clicked(dwX - x, dwY - y);
		return true;
	}
	return false;
}

/*
 *	Handles a click in a relative area
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::Clicked(DWORD dwX, DWORD dwY)
{
	bool bClickedRight = dwX > (w / 2);
	int nClickY = dwY / (h / 4);
	int nClickSlot = (nClickY * 2) + bClickedRight;
	int nNewSelection = nClickSlot + nCurrentScroll;

	if (nNewSelection >= nNumberSaves)
	{
		nNewSelection = -1;
	}

	nCurrentSelection = nNewSelection;
	Selected(nCurrentSelection);
}

/*
 *	A new character slot was selected
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::Selected(int nNewSelection)
{
	if (nNewSelection >= nNumberSaves)
	{
		nNewSelection = -1;
	}

	nCurrentSelection = nNewSelection;
	if (nCurrentSelection == -1)
	{
		// Grey out the "OK", "Delete Character" and "Convert to Expansion" buttons
		dynamic_cast<D2Panel_CharSelect*>(m_pOwner)->InvalidateSelection();
	}
	else
	{
		dynamic_cast<D2Panel_CharSelect*>(m_pOwner)->ValidateSelection();
	}
}

/*
 *	We need to load up the selected save.
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::LoadSave()
{
	CharacterSaveData* pCurrent;

	if (nCurrentSelection == -1)
	{
		return;
	}

	// Advance nCurrentSelection times through the linked list
	pCurrent = pCharacterData;
	for (int i = 0; i < nCurrentSelection && pCurrent != nullptr; i++, pCurrent = pCurrent->pNext);

	if (pCurrent == nullptr)
	{	// invalid selection
		return;
	}

	memcpy(&cl.currentSave.header, &pCurrent->header, sizeof(pCurrent->header));
	D2Lib::strncpyz(cl.szCurrentSave, pCurrent->path, MAX_D2PATH_ABSOLUTE);
}
