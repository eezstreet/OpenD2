#include "D2Menu_CharCreate.hpp"

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
D2Menu_CharCreate::D2Menu_CharCreate()
{
	tex_handle fireTex = trap->R_RegisterAnimatedDC6("data\\global\\ui\\FrontEnd\\fire.DC6", "ccfire", PAL_FECHAR);

	backgroundTex = trap->R_RegisterDC6Texture("data\\global\\ui\\FrontEnd\\characterCreationScreenEXP.dc6", 
		"ccback", 0, 11, PAL_FECHAR);
	fireAnim = trap->R_RegisterAnimation(fireTex, "ccfire", 0);

	pStaticPanel = new D2Panel_CharCreate_Static();
	pDynamicPanel = new D2Panel_CharCreate_Dynamic();

	AddPanel(pStaticPanel, true);
	AddPanel(pDynamicPanel, false);

	// Load all of the information needed by the char create menu
	szChooseClassStr = trap->TBL_FindStringFromIndex(5127);

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
			snprintf(szPath, MAX_D2PATH,
				"data\\global\\ui\\FrontEnd\\%s\\%s%s.dc6",
				szCharacterClassFolders[i], szCharacterClassShort[i], szAnimName[j]);
			snprintf(szHandle, 32, "%s%s", szCharacterClassShort[i], szAnimName[j]);

			CreateData[i].animTextureHandle[j] = trap->R_RegisterAnimatedDC6(szPath, szHandle, PAL_FECHAR);
			CreateData[i].animAnimHandle[j] = 
				trap->R_RegisterAnimation(CreateData[i].animTextureHandle[j], szHandle, 0);

			if (CreateData[i].bSpecialAnimPresent[j])
			{
				snprintf(szPath, MAX_D2PATH,
					"data\\global\\ui\\FrontEnd\\%s\\%s%s.dc6",
					szCharacterClassFolders[i], szCharacterClassShort[i], szAnimNameSpecial[j]);
				snprintf(szHandle, 32, "%s%s", szCharacterClassShort[i], szAnimNameSpecial[j]);

				CreateData[i].specialAnimTextureHandle[j] = trap->R_RegisterAnimatedDC6(szPath, szHandle, PAL_FECHAR);
				CreateData[i].specialAnimAnimHandle[j] = 
					trap->R_RegisterAnimation(CreateData[i].specialAnimAnimHandle[j], szHandle, 0);
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
				CreateData[i].nDrawYPos = 160;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4011);
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(5128);
				break;
			case D2CLASS_SORCERESS:
				CreateData[i].nDrawXPos = 620;
				CreateData[i].nDrawYPos = 200;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4010);
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(5131);
				break;
			case D2CLASS_NECROMANCER:
				CreateData[i].nDrawXPos = 290;
				CreateData[i].nDrawYPos = 160;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4009);
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(5129);
				break;
			case D2CLASS_PALADIN:
				CreateData[i].nDrawXPos = 505;
				CreateData[i].nDrawYPos = 170;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4008);
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(5132);
				break;
			case D2CLASS_BARBARIAN:
				CreateData[i].nDrawXPos = 390;
				CreateData[i].nDrawYPos = 150;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4007);
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(5130);
				break;
			case D2CLASS_DRUID:
				CreateData[i].nDrawXPos = 720;
				CreateData[i].nDrawYPos = 180;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4012);	// in classic strings, wtf? :D
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(22518);
				break;
			case D2CLASS_ASSASSIN:
				CreateData[i].nDrawXPos = 225;
				CreateData[i].nDrawYPos = 185;
				CreateData[i].szCharClassName = trap->TBL_FindStringFromIndex(4013);	// in classic strings, wtf? :D
				CreateData[i].szCharClassDescription = trap->TBL_FindStringFromIndex(22519);
				break;
		}

		// set our state to be idle
		CreateData[i].status = CCA_IdleBack;
	}

	// set us up to not have anything highlighted
	m_nHighlightedClass = D2CLASS_MAX;
}

/*
 *	Destroys the character creation menu
 */
D2Menu_CharCreate::~D2Menu_CharCreate()
{
	// we can reasonably unregister any textures here, we probably won't see them again for a long time
	trap->R_DeregisterTexture(nullptr, backgroundTex);
	trap->R_DeregisterTexture("ccfire", INVALID_HANDLE);
	trap->R_DeregisterAnimation(fireAnim);

	for (int i = 0; i < D2CLASS_MAX; i++)
	{
		for (int j = 0; j < CCA_MAX; j++)
		{
			trap->R_DeregisterAnimation(CreateData[i].animAnimHandle[j]);
			trap->R_DeregisterTexture(nullptr, CreateData[i].animTextureHandle[j]);
			if (CreateData[i].bSpecialAnimPresent[j])
			{
				trap->R_DeregisterAnimation(CreateData[i].specialAnimAnimHandle[j]);
				trap->R_DeregisterTexture(nullptr, CreateData[i].specialAnimTextureHandle[j]);
			}
		}
	}

	delete pStaticPanel;
	delete pDynamicPanel;
}

/*
 *	Draws the character creation menu
 */
void D2Menu_CharCreate::Draw()
{
	// draw background
	trap->R_DrawTexture(backgroundTex, 0, 0, 800, 600, 0, 0);

	// draw choose class text
	trap->R_DrawText(cl.font30, szChooseClassStr, 0, 25, 800, 600, ALIGN_CENTER, ALIGN_TOP);

	// draw the characters in each of their position
	for (int i = 0; i < D2CLASS_MAX; i++)
	{
		if (trap->R_PixelPerfectDetect(CreateData[i].animAnimHandle[CreateData[i].status],
			cl.dwMouseX, cl.dwMouseY, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos, true))
		{	// mouse is over this thing
			m_nHighlightedClass = i;
			CreateData[i].status = CCA_IdleBackSel;
		}
		else
		{
			CreateData[i].status = CCA_IdleBack;
		}

		trap->R_Animate(CreateData[i].animAnimHandle[CreateData[i].status], 
			8, CreateData[i].nDrawXPos, CreateData[i].nDrawYPos);
	}

	// draw current class text
	if (m_nHighlightedClass != D2CLASS_MAX)
	{
		trap->R_DrawText(cl.font30, 
			CreateData[m_nHighlightedClass].szCharClassName, 0, 75, 800, 600, ALIGN_CENTER, ALIGN_TOP);
		trap->R_ColorModFont(cl.font16, 255, 255, 255);
		trap->R_DrawText(cl.font16,
			CreateData[m_nHighlightedClass].szCharClassDescription, 0, 105, 800, 600, ALIGN_CENTER, ALIGN_TOP);
	}

	DrawAllPanels();

	// draw fire
	trap->R_Animate(fireAnim, 25, 380, 150);
}