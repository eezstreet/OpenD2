#include "D2Widget_CharSelectList.hpp"
#include "D2Panel_CharSelect.hpp"
#include "D2Widget_CharSelectSave.hpp"

#define D2_NUM_VISIBLE_SAVES	8

// Mappings for the class token
static char* gszClassTokens[D2CLASS_MAX] = {
	"AM", "SO", "NE", "PA", "BA", "DZ", "AI",
};

// So, funny story. The devs didn't actually tie these to .tbl entries, so they are not translated at all !
// If you look in patchstring.tbl, expansionstring.tbl, and string.tbl, they are not listed at all!
struct CharacterTitle {
	const char16_t* maleTitle;
	const char16_t* femaleTitle;
};

// NOTE: Expansion saves don't actually use every 4th slot.
// This is because it skips over Diablo. When Baal is killed, it increments by one.
// When the Eve of Destruction has completed, it is incremented again. So, twice.
#define TITLE_BIGENDER(x, y)	{x, y}, {x, y}, {x, y}, {x, y}
#define TITLE_NOGENDER(x)		{x, x}, {x, x}, {x, x}, {x, x}
#define TITLE_BIGENDER_EXP(x,y)	{x, y}, {x, y}, {x, y}, {x, y}, {x, y}
#define TITLE_NOGENDER_EXP(x)	{x, x}, {x, x}, {x, x}, {x, x}, {x, x}

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

/*
 *	Creates a Character Select list widget.
 *	This method is responsible for loading up all of the savegames.
 *	We should maybe cache the results of the savegame loading so that going to the charselect page doesn't take a while.
 */
D2Widget_CharSelectList::D2Widget_CharSelectList(int x, int y, int w, int h) 
	: D2Widget(x, y, w, h)
{
	// Blank out our own data
	nNumberSaves = 0;
	nCurrentScroll = 0;
	nCurrentSelection = -1;
	saves = nullptr;
	pCharacterData = nullptr;

	greyFrameRef = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\charselectboxgrey.dc6", UsagePolicy_Permanent);
	frameRef = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\charselectbox.dc6", UsagePolicy_Permanent);

	// Create the scrollbar - we manually draw it as part of this widget's display
	//pScrollBar = new D2Widget_Scrollbar()
}

/*
 *	Destroys the character select list widget
 */
D2Widget_CharSelectList::~D2Widget_CharSelectList()
{
	// Free out the entire linked list
	if (saves)
	{
		delete saves;
	}

	engine->graphics->DeleteReference(greyFrameRef);
	engine->graphics->DeleteReference(frameRef);
}

/*
 *	Add a savegame to the list
 */
void D2Widget_CharSelectList::AddSave(D2SaveHeader& header, char* path)
{
	// Allocate a character save entry
	D2Widget_CharSelectSave* newSave = new D2Widget_CharSelectSave(path, header);
	
	newSave->SetNextInChain(saves);
	saves = newSave;

	CharacterSaveData* pSaveData = (CharacterSaveData*)malloc(sizeof(CharacterSaveData));

	// Copy the path, name, and header data
	D2Lib::strncpyz(pSaveData->path, path, MAX_D2PATH_ABSOLUTE);
	D2Lib::qmbtowc(pSaveData->name, 16, header.szCharacterName);
	pSaveData->header = header;

	// Register the animations for it.
	// TODO: use the actual anims (it just uses hth, lit, no weapon for now..)
	pSaveData->token = engine->graphics->CreateReference(TOKEN_CHAR, gszClassTokens[header.nCharClass]);
	pSaveData->renderedToken = engine->renderer->AllocateObject(0);
	pSaveData->renderedToken->AttachTokenResource(pSaveData->token);
	pSaveData->renderedToken->SetTokenHitClass(WC_HTH);
	pSaveData->renderedToken->SetTokenMode(PLRMODE_TN);
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_HEAD, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_LEFTARM, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_LEFTHAND, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_LEGS, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_RIGHTARM, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_RIGHTHAND, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_SHIELD, "");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_SPECIAL1, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_SPECIAL2, "lit");
	pSaveData->renderedToken->SetTokenArmorLevel(COMP_TORSO, "lit");
	pSaveData->renderedToken->SetAnimationDirection(0);

	pSaveData->frame = engine->renderer->AllocateObject(0);
	pSaveData->title = engine->renderer->AllocateObject(1);
	pSaveData->charName = engine->renderer->AllocateObject(1);
	pSaveData->classAndLevel = engine->renderer->AllocateObject(1);
	pSaveData->expansionChar = engine->renderer->AllocateObject(1);
	pSaveData->title->AttachFontResource(cl.font16);
	pSaveData->charName->AttachFontResource(cl.font16);
	pSaveData->classAndLevel->AttachFontResource(cl.font16);
	pSaveData->expansionChar->AttachFontResource(cl.font16);

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
	if (saves)
	{
		saves->DrawLink(8, true);
	}
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
		if ((pSaveData->header.nCharStatus & (1 << D2STATUS_HARDCORE)) &&
			(pSaveData->header.nCharStatus & (1 << D2STATUS_DEAD)))
		{
			// Dead hardcore player gets a grey frame
			pSaveData->frame->AttachCompositeTextureResource(greyFrameRef, 0, -1);
		}
		else
		{
			// Use the normal frame
			pSaveData->frame->AttachCompositeTextureResource(frameRef, 0, -1);
		}

		pSaveData->frame->SetDrawCoords(bRightSlot ? x : x + nSlotWidth, nSlotY * nSlotHeight,
			nSlotWidth, nSlotHeight);
		pSaveData->frame->Draw();
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
	if (pSaveData->header.nCharStatus & (1 << D2STATUS_HARDCORE))
	{
		pSaveData->title->SetTextColor(TextColor_Red);
	}
	else
	{
		pSaveData->title->SetTextColor(TextColor_White);
	}
	
	if (pSaveData->header.nCharStatus & (1 << D2STATUS_EXPANSION))
	{
		if (pSaveData->header.nCharStatus & (1 << D2STATUS_HARDCORE))
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
	else if (pSaveData->header.nCharStatus & (1 << D2STATUS_HARDCORE))
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
		// Draw the character title if we have one. -- FIXME, we don't need to set everything here, we should just do a draw() here
		pSaveData->title->SetText(szCharacterTitle);
		pSaveData->title->SetTextAlignment(nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
		pSaveData->title->Draw();
	}
	nY += 15;

	// Draw character name - FIXME, we don't need to set everything here, we should just do a draw() here
	pSaveData->charName->SetText(pSaveData->name);
	pSaveData->charName->SetTextColor(TextColor_White);
	pSaveData->charName->SetTextAlignment(nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
	pSaveData->charName->Draw();
	nY += 15;

	// Draw character level and class
	pSaveData->classAndLevel->SetTextColor(TextColor_White);
	pSaveData->classAndLevel->SetTextAlignment(nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
	auto x = Client_className(pSaveData->header.nCharClass);
	// Format it so that it will read "Level %d <Class>
	D2Lib::qsnprintf(szCharacterLevelClass, 32, u"%s %s",
		engine->TBL_FindStringFromIndex(5017),
		Client_className(pSaveData->header.nCharClass));
	// Reformat it again so that the level is filled in
	D2Lib::qsnprintf(szDisplayString, 32, szCharacterLevelClass, pSaveData->header.nCharLevel);
	// Now draw it!
	pSaveData->classAndLevel->SetText(szDisplayString);
	pSaveData->classAndLevel->Draw();
	nY += 15;

	// Draw whether this is an expansion character
	if (pSaveData->header.nCharStatus & (1 << D2STATUS_EXPANSION))
	{
		pSaveData->expansionChar->SetTextColor(TextColor_BrightGreen);
		pSaveData->expansionChar->SetTextAlignment(nX, nY, 194, 15, ALIGN_LEFT, ALIGN_TOP);
		pSaveData->expansionChar->Draw();
	}

	// Draw the token instance
	pSaveData->renderedToken->SetDrawCoords(nX - 40, nY - 140, 0, 0);
	pSaveData->renderedToken->Draw();
}

/*
 *	Returns the name of the currently selected character.
 *	@author	eezstreet
 */
char16_t* D2Widget_CharSelectList::GetSelectedCharacterName()
{
	if (saves)
	{
		return saves->GetSelectedCharacterName();
	}

	return u"";
}

/*
 *	Handles a mouse-down event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		return true;
	}
	return false;
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