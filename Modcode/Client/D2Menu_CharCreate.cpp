#include "D2Menu_CharCreate.hpp"
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

/*
 *	Creates the character creation menu. Lots of creation.
 */
D2Menu_CharCreate::D2Menu_CharCreate(bool bFromCharSelect)
{
#if 0
	tex_handle fireTex = engine->renderer->TextureFromAnimatedDC6("data\\global\\ui\\FrontEnd\\fire.DC6", "ccfire", PAL_FECHAR);

	backgroundTex = engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\characterCreationScreenEXP.dc6", 
		"ccback", 0, 11, PAL_FECHAR);
	fireAnim = engine->renderer->RegisterDC6Animation(fireTex, "ccfire", 0);
	engine->renderer->SetTextureBlendMode(fireTex, BLEND_ADD);

	// start gobbling text input events. any keystroke will be interpreted as text
	engine->In_StartTextEditing();

	pStaticPanel = new D2Panel_CharCreate_Static();
	pDynamicPanel = new D2Panel_CharCreate_Dynamic();

	AddPanel(pStaticPanel, true);
	AddPanel(pDynamicPanel, false);

	// Load all of the information needed by the char create menu
	szChooseClassStr = engine->TBL_FindStringFromIndex(5127);

	memset(CreateData, 0, sizeof(CharCreateData) * D2CLASS_MAX);
	for (int i = 0; i < D2CLASS_MAX; i++)
	{
		char szPath[MAX_D2PATH]{ 0 };
		char szHandle[32]{ 0 };

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

		// iterate through all basic animations
		for (int j = 0; j < CCA_MAX; j++)
		{
			// register regular animations
			snprintf(szPath, MAX_D2PATH,
				"data\\global\\ui\\FrontEnd\\%s\\%s%s.dc6",
				szCharacterClassFolders[i], szCharacterClassShort[i], szAnimName[j]);
			snprintf(szHandle, 32, "%s%s", szCharacterClassShort[i], szAnimName[j]);

			CreateData[i].animTextureHandle[j] = engine->renderer->TextureFromAnimatedDC6(szPath, szHandle, PAL_FECHAR);
			CreateData[i].animAnimHandle[j] = 
				engine->renderer->RegisterDC6Animation(CreateData[i].animTextureHandle[j], szHandle, 0);

			engine->renderer->PollTexture(CreateData[i].animTextureHandle[j], nullptr, (DWORD*)&CreateData[i].nDrawBaselineY[j]);
			if (j != 0)
			{
				CreateData[i].nDrawBaselineY[j] -= CreateData[i].nDrawBaselineY[0];
			}

			if (CreateData[i].bSpecialAnimPresent[j])
			{
				// register special animations
				snprintf(szPath, MAX_D2PATH,
					"data\\global\\ui\\FrontEnd\\%s\\%s%s.dc6",
					szCharacterClassFolders[i], szCharacterClassShort[i], szAnimNameSpecial[j]);
				snprintf(szHandle, 32, "%s%s", szCharacterClassShort[i], szAnimNameSpecial[j]);

				CreateData[i].specialAnimTextureHandle[j] = engine->renderer->TextureFromAnimatedDC6(szPath, szHandle, PAL_FECHAR);
				CreateData[i].specialAnimAnimHandle[j] = 
					engine->renderer->RegisterDC6Animation(CreateData[i].specialAnimTextureHandle[j], szHandle, 0);

				// special animations are additively blended for the necro and sorc
				if (i == D2CLASS_SORCERESS || i == D2CLASS_NECROMANCER)
				{
					engine->renderer->SetTextureBlendMode(CreateData[i].specialAnimTextureHandle[j], BLEND_ADD);
				}
			}
			else
			{
				CreateData[i].specialAnimAnimHandle[j] = INVALID_HANDLE;
				CreateData[i].specialAnimTextureHandle[j] = INVALID_HANDLE;
			}
		}

		// get strings and positions
		switch (i)
		{
			case D2CLASS_AMAZON:
				CreateData[i].nDrawXPos = 100;
				CreateData[i].nDrawYPos = 140;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4011);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5128);
				break;
			case D2CLASS_SORCERESS:
				CreateData[i].nDrawXPos = 626;
				CreateData[i].nDrawYPos = 191;
				CreateData[i].nSpecialYOffset = 85;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4010);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5131);
				break;
			case D2CLASS_NECROMANCER:
				CreateData[i].nDrawXPos = 301;
				CreateData[i].nDrawYPos = 151;
				CreateData[i].nSpecialYOffset = -60;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4009);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5129);
				break;
			case D2CLASS_PALADIN:
				CreateData[i].nDrawXPos = 520;
				CreateData[i].nDrawYPos = 164;
				CreateData[i].nSpecialYOffset = 54;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4008);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5132);
				break;
			case D2CLASS_BARBARIAN:
				CreateData[i].nDrawXPos = 400;
				CreateData[i].nDrawYPos = 150;
				CreateData[i].nSpecialYOffset = 30;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4007);
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(5130);
				break;
			case D2CLASS_DRUID:
				CreateData[i].nDrawXPos = 720;
				CreateData[i].nDrawYPos = 170;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4012);	// in classic strings, wtf? :D
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(22518);
				break;
			case D2CLASS_ASSASSIN:
				CreateData[i].nDrawXPos = 232;
				CreateData[i].nDrawYPos = 178;
				CreateData[i].szCharClassName = engine->TBL_FindStringFromIndex(4013);	// in classic strings, wtf? :D
				CreateData[i].szCharClassDescription = engine->TBL_FindStringFromIndex(22519);
				break;
		}

		// set our state to be idle
		CreateData[i].status = CCA_IdleBack;

		// set our initial baseline to be 0
		CreateData[i].nDrawBaselineY[0] = 0;
	}

	// set us up to not have anything highlighted
	m_nHighlightedClass = D2CLASS_MAX;
	m_nSelectedClass = D2CLASS_MAX;

	// other data
	m_bFromCharSelect = bFromCharSelect;
#endif
}

/*
 *	Destroys the character creation menu
 */
D2Menu_CharCreate::~D2Menu_CharCreate()
{
#if 0
	// we can reasonably unregister any textures here, we probably won't see them again for a long time
	engine->renderer->DeregisterTexture(nullptr, backgroundTex);
	engine->renderer->DeregisterTexture("ccfire", INVALID_HANDLE);
	engine->renderer->DeregisterAnimation(fireAnim);

	for (int i = 0; i < D2CLASS_MAX; i++)
	{
		for (int j = 0; j < CCA_MAX; j++)
		{
			engine->renderer->DeregisterAnimation(CreateData[i].animAnimHandle[j]);
			engine->renderer->DeregisterTexture(nullptr, CreateData[i].animTextureHandle[j]);
			if (CreateData[i].bSpecialAnimPresent[j])
			{
				engine->renderer->DeregisterAnimation(CreateData[i].specialAnimAnimHandle[j]);
				engine->renderer->DeregisterTexture(nullptr, CreateData[i].specialAnimTextureHandle[j]);
			}
		}
	}

	// stop gobbling text events for now
	engine->In_StopTextEditing();

	delete pStaticPanel;
	delete pDynamicPanel;
#endif
}

/*
 *	Gets called whenever an animation finishes
 *	NOTE: static function
 */
void D2Menu_CharCreate::AnimationKeyframe(anim_handle anim, int nExtraInt)
{
#if 0
	D2Menu_CharCreate* pMenu = dynamic_cast<D2Menu_CharCreate*>(cl.pActiveMenu);
	
	if (pMenu->CreateData[nExtraInt].status == CCA_FrontToBack)
	{
		engine->renderer->SetAnimFrame(pMenu->CreateData[nExtraInt].animAnimHandle[CCA_IdleBack], 0);
		pMenu->CreateData[nExtraInt].status = CCA_IdleBack;
	}
	else if (pMenu->CreateData[nExtraInt].status == CCA_BackToFront)
	{
		engine->renderer->SetAnimFrame(pMenu->CreateData[nExtraInt].animAnimHandle[CCA_IdleFront], 0);
		pMenu->CreateData[nExtraInt].status = CCA_IdleFront;
	}
#endif
}

/*
 *	Draws the character creation menu
 */
void D2Menu_CharCreate::Draw()
{
#if 0
	int i = D2CLASS_BARBARIAN;

	// draw background
	engine->renderer->DrawTexture(backgroundTex, 0, 0, 800, 600, 0, 0);

	// draw choose class text
	engine->renderer->DrawText(cl.font30, szChooseClassStr, 0, 25, 800, 600, ALIGN_CENTER, ALIGN_TOP);

	// draw the characters in each of their position
	m_nHighlightedClass = D2CLASS_MAX;
	while(i != D2CLASS_MAX)
	{
		if (CreateData[i].status == CCA_IdleBack || CreateData[i].status == CCA_IdleBackSel)
		{
			// Idle in the back - we need to handle whether our mouse cursor is over top of it or not
			if (engine->renderer->PixelPerfectDetect(CreateData[i].animAnimHandle[CreateData[i].status],
				cl.dwMouseX, cl.dwMouseY, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos, true))
			{	// mouse is over this thing
				m_nHighlightedClass = i;
				if (CreateData[i].status == CCA_IdleBack)
				{
					// set new status frame to be the same as current
					engine->renderer->SetAnimFrame(CreateData[i].animAnimHandle[CCA_IdleBackSel],
						engine->renderer->GetAnimFrame(CreateData[i].animAnimHandle[CCA_IdleBack]));
				}
				CreateData[i].status = CCA_IdleBackSel;
			}
			else
			{
				if (CreateData[i].status == CCA_IdleBackSel)
				{
					// set new status frame to be the same as current
					engine->renderer->SetAnimFrame(CreateData[i].animAnimHandle[CCA_IdleBack],
						engine->renderer->GetAnimFrame(CreateData[i].animAnimHandle[CCA_IdleBackSel]));
				}
				CreateData[i].status = CCA_IdleBack;
			}
			engine->renderer->Animate(CreateData[i].animAnimHandle[CreateData[i].status],
				8, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos - CreateData[i].nDrawBaselineY[CreateData[i].status]);
		}
		else
		{
			engine->renderer->Animate(CreateData[i].animAnimHandle[CreateData[i].status],
				25, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos - CreateData[i].nDrawBaselineY[CreateData[i].status]);
			if (CreateData[i].bSpecialAnimPresent[CreateData[i].status] && i != m_nSelectedClass)
			{
				engine->renderer->Animate(CreateData[i].specialAnimAnimHandle[CreateData[i].status],
					25, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos + CreateData[i].nSpecialYOffset);
			}
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

	if (m_nSelectedClass != D2CLASS_MAX && m_nHighlightedClass == D2CLASS_MAX)
	{	// if we have selected a class and have none highlighted, use that one for its text
		m_nHighlightedClass = m_nSelectedClass;
	}

	if (m_nSelectedClass != D2CLASS_MAX)
	{
		// Always draw the special animation of the selected thing on top
		if (CreateData[m_nSelectedClass].bSpecialAnimPresent[CreateData[m_nSelectedClass].status])
		{
			engine->renderer->Animate(CreateData[m_nSelectedClass].specialAnimAnimHandle[CreateData[m_nSelectedClass].status],
				25, CreateData[m_nSelectedClass].nDrawXPos,
				CreateData[m_nSelectedClass].nDrawYPos + CreateData[m_nSelectedClass].nSpecialYOffset);
		}
	}

	// draw current class text
	if (m_nHighlightedClass != D2CLASS_MAX)
	{
		engine->renderer->DrawText(cl.font30, 
			CreateData[m_nHighlightedClass].szCharClassName, 0, 75, 800, 600, ALIGN_CENTER, ALIGN_TOP);
		engine->renderer->ColorModFont(cl.font16, 255, 255, 255);
		engine->renderer->DrawText(cl.font16,
			CreateData[m_nHighlightedClass].szCharClassDescription, 0, 105, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	}

	DrawAllPanels();

	// draw fire
	engine->renderer->Animate(fireAnim, 25, 380, 160);
#endif
}

/*
 *	Handle mouse down event on the character creation screen
 */
bool D2Menu_CharCreate::HandleMouseClicked(DWORD dwX, DWORD dwY)
{
#if 0
	anim_handle anim;

	// check to see if we clicked on any of the characters
	for (int i = 0; i < D2CLASS_MAX; i++)
	{
		if (m_nSelectedClass == i)
		{	// selected class ALWAYS uses alpha in per pixel collision
			if (engine->renderer->PixelPerfectDetect(CreateData[i].animAnimHandle[CreateData[i].status],
				dwX, dwY, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos, false))
			{
				// we clicked inside its bounds
				CreateData[m_nSelectedClass].status = CCA_FrontToBack;

				anim = CreateData[m_nSelectedClass].animAnimHandle[CCA_FrontToBack];
				engine->renderer->SetAnimFrame(anim, 0);
				engine->renderer->AddAnimKeyframe(anim, engine->renderer->GetAnimFrameCount(anim) - 1, AnimationKeyframe, i);
				if (CreateData[m_nSelectedClass].bSpecialAnimPresent[CCA_FrontToBack])
				{
					engine->renderer->SetAnimFrame(CreateData[m_nSelectedClass].specialAnimAnimHandle[CCA_FrontToBack], 0);
				}
				m_nSelectedClass = D2CLASS_MAX;

				HidePanel(pDynamicPanel);
				return true;
			}
		}
		else
		{
			if (engine->renderer->PixelPerfectDetect(CreateData[i].animAnimHandle[CreateData[i].status],
				dwX, dwY, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos, true))
			{	// we selected a class
				if (m_nSelectedClass != D2CLASS_MAX)
				{	// tell the other class to go to transition
					anim = CreateData[m_nSelectedClass].animAnimHandle[CCA_FrontToBack];
					CreateData[m_nSelectedClass].status = CCA_FrontToBack;
					engine->renderer->SetAnimFrame(anim, 0);
					engine->renderer->AddAnimKeyframe(anim, engine->renderer->GetAnimFrameCount(anim) - 1, AnimationKeyframe, m_nSelectedClass);
					if (CreateData[m_nSelectedClass].bSpecialAnimPresent[CCA_FrontToBack])
					{
						engine->renderer->SetAnimFrame(CreateData[m_nSelectedClass].specialAnimAnimHandle[CCA_FrontToBack], 0);
					}
				}
				CreateData[i].status = CCA_BackToFront;
				m_nSelectedClass = i;
				anim = CreateData[m_nSelectedClass].animAnimHandle[CCA_BackToFront];
				engine->renderer->SetAnimFrame(anim, 0);
				engine->renderer->AddAnimKeyframe(anim, engine->renderer->GetAnimFrameCount(anim) - 1, AnimationKeyframe, i);
				if (CreateData[m_nSelectedClass].bSpecialAnimPresent[CCA_BackToFront])
				{
					engine->renderer->SetAnimFrame(CreateData[m_nSelectedClass].specialAnimAnimHandle[CCA_BackToFront], 0);
				}
				ShowPanel(pDynamicPanel);
				return true;
			}
		}
	}
	return D2Menu::HandleMouseClicked(dwX, dwY);
#endif
	return false;
}

/*
 *	Handle a text input event
 */
void D2Menu_CharCreate::HandleTextInput(char* szText)
{
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
bool D2Menu_CharCreate::HandleKeyDown(DWORD dwKey)
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
bool D2Menu_CharCreate::TrySaveCharacter()
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