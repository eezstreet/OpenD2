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

D2Widget_CharSelectSave::D2Widget_CharSelectSave(const char* characterSave, D2SaveHeader& header)
{
	bool bClassMale = Client_classMale(header.nCharClass);

	D2Lib::qmbtowc(charName, sizeof(charName), header.szCharacterName);

	nextInChain = nullptr;
	bIsSelected = false;
	bHasTitle = bIsDeadHardcore = bIsExpansion = bIsHardcore = false;

	selectionFrame = engine->renderer->AllocateObject(0);
	characterPreview = engine->renderer->AllocateObject(0);
	characterTitle = engine->renderer->AllocateObject(1);
	characterName = engine->renderer->AllocateObject(1);
	characterLevelAndClass = engine->renderer->AllocateObject(1);
	expansionText = engine->renderer->AllocateObject(1);

	characterTitle->AttachFontResource(cl.font16);
	characterName->AttachFontResource(cl.font16);
	characterLevelAndClass->AttachFontResource(cl.font16);
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

	// Handle the EXPANSION text (if present)
	if (header.nCharStatus & (1 << D2STATUS_EXPANSION))
	{
		bIsExpansion = true;
		expansionText->SetTextColor(TextColor_BrightGreen);
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
}

void D2Widget_CharSelectSave::SetDrawPosition(uint32_t x, uint32_t y)
{
	// TODO
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

void D2Widget_CharSelectSave::DrawLink(int counter, bool bDrawLeft)
{
	// set parameters

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