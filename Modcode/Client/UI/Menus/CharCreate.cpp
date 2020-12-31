#include "CharCreate.hpp"
#include "../Panels/CharCreate.hpp"
#include <time.h>

static const char* szCharacterClassFolders[D2CLASS_MAX] = {
	"Amazon",
	"Sorceress",
	"Necromancer",
	"Paladin",
	"Barbarian",
	"Druid",
	"Assassin",
};

static const char* szCharacterClassShort[D2CLASS_MAX] = {
	"am",
	"so",
	"ne",
	"pa",
	"ba",
	"dz",
	"as",
};

static const char* szAnimName[CCA_MAX] = {
	"nu1",
	"nu2",
	"nu3",
	"fw",
	"bw",
};

static const char* szAnimNameSpecial[CCA_MAX] = {
	"nu1s",
	"nu2s",
	"nu3s",
	"fws",
	"bws",
};

void CharCreateData::OnCharacterCameForward(class IRenderObject* caller, void* extraData)
{
	CharCreateData* data = (CharCreateData*)extraData;
	data->displayObject->AttachAnimationResource(data->animationHandle[CCA_IdleFront], true);
	data->status = CCA_IdleFront;
	if (data->bSpecialAnimPresent[CCA_IdleFront])
	{
		data->specialAnimationObject->AttachAnimationResource(data->specialAnimationHandle[CCA_IdleFront], true);
	}
}

void CharCreateData::OnCharacterSteppedBack(class IRenderObject* caller, void* extraData)
{
	CharCreateData* data = (CharCreateData*)extraData;
	data->displayObject->AttachAnimationResource(data->animationHandle[CCA_IdleBack], true);
	data->status = CCA_IdleBack;
}

/*
 *	Creates the character creation menu. Lots of creation.
 */
namespace D2Menus
{
	CharCreate::CharCreate(bool bFromCharSelect) : D2Menu()
	{
		engine->renderer->SetGlobalPalette(PAL_FECHAR);
		backgroundTexture = engine->renderer->AllocateObject(0);
		fireAnimation = engine->renderer->AllocateObject(0);

		IGraphicsReference* backgroundGraphic = engine->graphics->CreateReference("data\\global\\ui\\FrontEnd\\characterCreationScreenEXP.dc6", UsagePolicy_SingleUse);
		fireGraphic = engine->graphics->CreateReference("data\\global\\ui\\FrontEnd\\fire.DC6", UsagePolicy_SingleUse);

		backgroundTexture->AttachCompositeTextureResource(backgroundGraphic, 0, -1);
		backgroundTexture->SetDrawCoords(0, 0, 800, 600);
		fireAnimation->AttachAnimationResource(fireGraphic, true);
		fireAnimation->SetDrawCoords(375, -80, 0, 0);
		fireAnimation->SetDrawMode(3);

		engine->graphics->DeleteReference(backgroundGraphic);

		// start gobbling text input events. any keystroke will be interpreted as text
		engine->In_StartTextEditing();

		pStaticPanel = new D2Panels::CharCreate_Static();
		pDynamicPanel = new D2Panels::CharCreate_Dynamic();

		AddPanel(pStaticPanel, true);
		AddPanel(pDynamicPanel, false);

		// Load all of the information needed by the char create menu
		szChooseClassStr = engine->TBL_FindStringFromIndex(5127);

		chooseClassTitle = engine->renderer->AllocateObject(1);
		chooseClassTitle->AttachFontResource(cl.font30);
		chooseClassTitle->SetText(szChooseClassStr);

		int w;
		chooseClassTitle->GetDrawCoords(nullptr, nullptr, &w, nullptr);
		chooseClassTitle->SetDrawCoords(400 - (w / 2), 25, 0, 0);

		chosenClassName = engine->renderer->AllocateObject(1);
		chosenClassName->AttachFontResource(cl.font30);

		chosenClassDescription = engine->renderer->AllocateObject(1);
		chosenClassDescription->AttachFontResource(cl.font16);

		memset(CreateData, 0, sizeof(CharCreateData) * D2CLASS_MAX);
		for (int i = 0; i < D2CLASS_MAX; i++)
		{
			char szPath[MAX_D2PATH]{ 0 };

			// flag special animations that need to be done
			switch (i) {
			case D2CLASS_SORCERESS:
			case D2CLASS_NECROMANCER:
				CreateData[i].bSpecialAnimPresent[CCA_IdleFront] = true;
				CreateData[i].bSpecialAnimPresent[CCA_BackToFront] = true;
				CreateData[i].bSpecialAnimPresent[CCA_FrontToBack] = true;
				break;
			case D2CLASS_BARBARIAN:
			case D2CLASS_PALADIN:
				CreateData[i].bSpecialAnimPresent[CCA_BackToFront] = true;
				break;
			}

			CreateData[i].displayObject = engine->renderer->AllocateObject(0);
			CreateData[i].specialAnimationObject = engine->renderer->AllocateObject(0);

			// iterate through all of the animations
			for (int j = 0; j < CCA_MAX; j++)
			{
				snprintf(szPath, MAX_D2PATH,
					"data\\global\\ui\\FrontEnd\\%s\\%s%s.dc6",
					szCharacterClassFolders[i], szCharacterClassShort[i], szAnimName[j]);
				CreateData[i].animationHandle[j] = engine->graphics->CreateReference(szPath, UsagePolicy_SingleUse);

				if (CreateData[i].bSpecialAnimPresent[j])
				{
					// register special animations
					snprintf(szPath, MAX_D2PATH,
						"data\\global\\ui\\FrontEnd\\%s\\%s%s.dc6",
						szCharacterClassFolders[i], szCharacterClassShort[i], szAnimNameSpecial[j]);
					CreateData[i].specialAnimationHandle[j] = engine->graphics->CreateReference(szPath, UsagePolicy_SingleUse);
				}
				else
				{
					CreateData[i].specialAnimationHandle[j] = nullptr;
				}
			}

			// get strings and positions
			switch (i)
			{
			case D2CLASS_AMAZON:
				CreateData[i].nDrawXPos = 100;
				CreateData[i].nDrawYPos = -70;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4011);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5128);
				break;
			case D2CLASS_SORCERESS:
				CreateData[i].nDrawXPos = 626;
				CreateData[i].nDrawYPos = -70;
				CreateData[i].nSpecialYOffset = 85;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4010);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5131);
				break;
			case D2CLASS_NECROMANCER:
				CreateData[i].nDrawXPos = 301;
				CreateData[i].nDrawYPos = -70;
				CreateData[i].nSpecialYOffset = -60;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4009);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5129);
				break;
			case D2CLASS_PALADIN:
				CreateData[i].nDrawXPos = 520;
				CreateData[i].nDrawYPos = -70;
				CreateData[i].nSpecialYOffset = 54;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4008);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5132);
				break;
			case D2CLASS_BARBARIAN:
				CreateData[i].nDrawXPos = 400;
				CreateData[i].nDrawYPos = -70;
				CreateData[i].nSpecialYOffset = 30;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4007);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5130);
				break;
			case D2CLASS_DRUID:
				CreateData[i].nDrawXPos = 720;
				CreateData[i].nDrawYPos = -50;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4012);	// in classic strings, wtf? :D
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(22518);
				break;
			case D2CLASS_ASSASSIN:
				CreateData[i].nDrawXPos = 232;
				CreateData[i].nDrawYPos = -50;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4013);	// in classic strings, wtf? :D
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(22519);
				break;
			}

			// set our state to be idle
			CreateData[i].status = CCA_IdleBack;
			CreateData[i].displayObject->AttachAnimationResource(CreateData[i].animationHandle[CCA_IdleBack], true);
			CreateData[i].displayObject->SetDrawCoords(CreateData[i].nDrawXPos, CreateData[i].nDrawYPos, 0, 0);
			CreateData[i].specialAnimationObject->SetDrawCoords(CreateData[i].nDrawXPos, CreateData[i].nDrawYPos, 0, 0);
			if (i == D2CLASS_NECROMANCER || i == D2CLASS_SORCERESS)
			{
				CreateData[i].specialAnimationObject->SetDrawMode(3);
			}

			// set our initial baseline to be 0
			CreateData[i].nDrawBaselineY[0] = 0;
		}

		// set us up to not have anything highlighted
		m_nHighlightedClass = D2CLASS_MAX;
		m_nSelectedClass = D2CLASS_MAX;

		// other data
		m_bFromCharSelect = bFromCharSelect;
	}

	/*
 *	Destroys the character creation menu
 */
	CharCreate::~CharCreate()
	{
		engine->renderer->Remove(backgroundTexture);
		engine->renderer->Remove(fireAnimation);
		engine->renderer->Remove(chooseClassTitle);
		engine->renderer->Remove(chosenClassName);
		engine->renderer->Remove(chosenClassDescription);
		engine->graphics->DeleteReference(fireGraphic);

		for (int i = 0; i < D2CLASS_MAX; i++)
		{
			engine->renderer->Remove(CreateData[i].displayObject);
			engine->renderer->Remove(CreateData[i].specialAnimationObject);

			for (int j = 0; j < CCA_MAX; j++)
			{
				engine->graphics->DeleteReference(CreateData[i].animationHandle[j]);
				if (CreateData[i].bSpecialAnimPresent[j])
				{
					engine->graphics->DeleteReference(CreateData[i].specialAnimationHandle[j]);
				}
			}
		}

		engine->In_StopTextEditing();

		delete pStaticPanel;
		delete pDynamicPanel;
	}

	/*
	 *	Draws the character creation menu
	 */
	void CharCreate::Draw()
	{
		// Draw the background texture
		backgroundTexture->Draw();

		// Draw the characters
		int i = D2CLASS_BARBARIAN;
		while (i != D2CLASS_MAX)
		{
			if (CreateData[i].status == CCA_IdleBack || CreateData[i].status == CCA_IdleBackSel)
			{
				CreateData[i].displayObject->SetFramerate(8);

				if (CreateData[i].status == CCA_IdleBack && CreateData[i].displayObject->PixelPerfectDetection(cl.dwMouseX, cl.dwMouseY))
				{
					// set status to selected
					CreateData[i].displayObject->AttachAnimationResource(CreateData[i].animationHandle[CCA_IdleBackSel], false);
					CreateData[i].status = CCA_IdleBackSel;
					m_nHighlightedClass = i;
				}
				else if (CreateData[i].status == CCA_IdleBackSel && !CreateData[i].displayObject->PixelPerfectDetection(cl.dwMouseX, cl.dwMouseY))
				{
					// set status to be unhighlighted
					CreateData[i].displayObject->AttachAnimationResource(CreateData[i].animationHandle[CCA_IdleBack], false);
					CreateData[i].status = CCA_IdleBack;
					m_nHighlightedClass = D2CLASS_MAX;
				}
				CreateData[i].displayObject->SetAnimationLoop(true);
			}
			else if (CreateData[i].status == CCA_IdleFront)
			{
				CreateData[i].displayObject->SetFramerate(25);
				CreateData[i].displayObject->SetAnimationLoop(true);
			}
			else
			{
				CreateData[i].displayObject->SetFramerate(25);
				CreateData[i].displayObject->SetAnimationLoop(false);
			}

			CreateData[i].displayObject->Draw();
			if (CreateData[i].bSpecialAnimPresent[CreateData[i].status])
			{
				CreateData[i].specialAnimationObject->Draw();
			}
			// this is really horribly ugly but necessary in order to draw everything correctly
			switch (i)
			{
			case D2CLASS_BARBARIAN:
				i = D2CLASS_NECROMANCER;
				break;
			case D2CLASS_NECROMANCER:
				i = D2CLASS_PALADIN;
				break;
			case D2CLASS_PALADIN:
				i = D2CLASS_ASSASSIN;
				break;
			case D2CLASS_ASSASSIN:
				i = D2CLASS_SORCERESS;
				break;
			case D2CLASS_SORCERESS:
				i = D2CLASS_AMAZON;
				break;
			case D2CLASS_AMAZON:
				i = D2CLASS_DRUID;
				break;
			case D2CLASS_DRUID:
				i = D2CLASS_MAX;
				break;
			}
		}

		// Draw the "Select Hero Class" title
		chooseClassTitle->Draw();

		// Draw the name and description of the highlighted class
		if (m_nSelectedClass != D2CLASS_MAX && m_nHighlightedClass == D2CLASS_MAX)
		{	// if we have selected a class and have none highlighted, use that one for its text
			m_nHighlightedClass = m_nSelectedClass;
		}

		if (m_nHighlightedClass != D2CLASS_MAX)
		{
			chosenClassName->SetText(CreateData[m_nHighlightedClass].szCharClassName);
			chosenClassDescription->SetText(CreateData[m_nHighlightedClass].szCharClassDescription);
			chosenClassName->SetTextAlignment(0, 75, 800, 600, ALIGN_CENTER, ALIGN_TOP);
			chosenClassDescription->SetTextAlignment(0, 105, 800, 600, ALIGN_CENTER, ALIGN_TOP);

			chosenClassName->Draw();
			chosenClassDescription->Draw();
		}

		// Draw all of the sub panels
		DrawAllPanels();

		// Draw the fire over top of everything
		fireAnimation->Draw();
	}

	void CharCreate::SelectCharacterClass(int classNum)
	{
		CharCreateData* selData = &CreateData[classNum];
		selData->displayObject->AttachAnimationResource(selData->animationHandle[CCA_BackToFront], true);
		selData->displayObject->RemoveAnimationFinishCallbacks();
		selData->displayObject->AddAnimationFinishedCallback(selData, CharCreateData::OnCharacterCameForward);
		selData->status = CCA_BackToFront;

		if (selData->bSpecialAnimPresent[CCA_BackToFront])
		{
			selData->specialAnimationObject->AttachAnimationResource(selData->specialAnimationHandle[CCA_BackToFront], true);
		}
	}

	void CharCreate::DeselectCharacterClass(int classNum)
	{
		CharCreateData* selData = &CreateData[classNum];
		selData->displayObject->AttachAnimationResource(selData->animationHandle[CCA_FrontToBack], true);
		selData->displayObject->RemoveAnimationFinishCallbacks();
		selData->displayObject->AddAnimationFinishedCallback(selData, CharCreateData::OnCharacterSteppedBack);
		selData->status = CCA_FrontToBack;

		if (selData->bSpecialAnimPresent[CCA_FrontToBack])
		{
			selData->specialAnimationObject->AttachAnimationResource(selData->specialAnimationHandle[CCA_FrontToBack], true);
		}
	}

	/*
	 *	Handle mouse down event on the character creation screen
	 */
	bool CharCreate::HandleMouseClicked(DWORD dwX, DWORD dwY)
	{
		CharCreateData* selData = &CreateData[m_nSelectedClass];

		// check to see if we clicked on a character
		for (int i = 0; i < D2CLASS_MAX; i++)
		{
			CharCreateData* charData = &CreateData[i];

			if (m_nSelectedClass == i && charData->displayObject->PixelPerfectDetection(dwX, dwY))
			{	// clicked on the currently selected character
				DeselectCharacterClass(i);
				HidePanel(pDynamicPanel);
				m_nSelectedClass = D2CLASS_MAX;
				pStaticPanel->DisableOKButton();
				return true;
			}

			if (charData->displayObject->PixelPerfectDetection(dwX, dwY))
			{
				SelectCharacterClass(i);

				if (m_nSelectedClass != D2CLASS_MAX)
				{
					DeselectCharacterClass(m_nSelectedClass);
				}
				else
				{
					ShowPanel(pDynamicPanel);
				}

				if (pDynamicPanel->GetNameLength() > 1)
				{
					pStaticPanel->EnableOKButton();
				}
				m_nSelectedClass = i;
				return true;
			}
		}
		return D2Menu::HandleMouseClicked(dwX, dwY);
	}

	/*
	 *	Handle a text input event
	 */
	void CharCreate::HandleTextInput(char* szText)
	{
		if (m_nSelectedClass == D2CLASS_MAX)
		{
			return;
		}

		D2Menu::HandleTextInput(szText);

		// check the length of the text entry field
		if (pDynamicPanel->GetNameLength() > 1)
		{
			pStaticPanel->EnableOKButton();
		}
		else
		{
			pStaticPanel->DisableOKButton();
		}
	}

	/*
	 *	Handle a key down event
	 */
	bool CharCreate::HandleKeyDown(DWORD dwKey)
	{
		D2Menu::HandleKeyDown(dwKey);

		// check the length of the text entry field
		if (pDynamicPanel->GetNameLength() > 1)
		{
			pStaticPanel->EnableOKButton();
		}
		else
		{
			pStaticPanel->DisableOKButton();
		}

		return true;
	}

	/*
	 *	Tries to save the character
	 *	@author	eezstreet
	 */
	bool CharCreate::TrySaveCharacter()
	{
		char		szSavePath[MAX_D2PATH]{ 0 };
		char		szPlayerName[16]{ 0 };
		int			nIteration = 0;
		const int	nMaxIterations = 10;
		fs_handle	f;

		memset(&cl.currentSave, 0, sizeof(cl.currentSave));

		// convert player name to ascii
		D2Lib::qwctomb(szPlayerName, 16, pDynamicPanel->GetName());

		// create save directory
		engine->FS_CreateSubdirectory(GAME_SAVE_PATH);

		// open the savegames, starting with "name.d2s"
		// if it already exists, append a number to the end, like "name-1.d2s"
		snprintf(szSavePath, MAX_D2PATH, GAME_SAVE_PATH "/%s.d2s", szPlayerName);
		do
		{
			engine->FS_Open(szSavePath, &f, FS_READ, true);
			if (f == INVALID_HANDLE)
			{
				break;
			}
			engine->FS_CloseFile(f);
			nIteration++;
			snprintf(szSavePath, MAX_D2PATH, GAME_SAVE_PATH "/%s-%i.d2s", szPlayerName, nIteration);
		} while (nIteration <= nMaxIterations);

		if (nIteration > nMaxIterations)
		{
			// couldn't find a free file, should fail
			return false;
		}

		engine->FS_Open(szSavePath, &f, FS_WRITE, true);

		// set some basic flags about the save
		cl.currentSave.header.dwMagic = D2SAVE_MAGIC;
		cl.currentSave.header.dwVersion = D2SAVE_VERSION;
		cl.currentSave.header.dwFileSize = sizeof(cl.currentSave.header);
		cl.currentSave.header.dwCreationTime = time(nullptr);
		cl.currentSave.header.dwModificationTime = cl.currentSave.header.dwCreationTime;

		cl.currentSave.header.nCharStatus |= (1 << D2STATUS_NEWBIE);
		if (pDynamicPanel->HardcoreChecked())
		{
			cl.currentSave.header.nCharStatus |= (1 << D2STATUS_HARDCORE);
		}

		cl.currentSave.header.nCharClass = m_nSelectedClass;
		if (m_nSelectedClass == D2CLASS_ASSASSIN || m_nSelectedClass == D2CLASS_DRUID || pDynamicPanel->ExpansionChecked())
		{	// FIXME: assassin and druid should probably have the expansion checkbox greyed out
			cl.currentSave.header.nCharStatus |= (1 << D2STATUS_EXPANSION);
		}

		// write the save header!
		// the rest of the data gets filled out later
		engine->FS_Write(f, &cl.currentSave.header, sizeof(cl.currentSave.header), 1);
		engine->FS_CloseFile(f);

		D2Lib::strncpyz(cl.szCurrentSave, szSavePath, MAX_D2PATH);

		return true;
	}
}