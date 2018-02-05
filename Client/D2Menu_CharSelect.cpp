#include "D2Menu_CharSelect.hpp"

static char* gszTokenNames[D2CLASS_MAX] = {
	"AM", "SO", "NE", "PA", "BA", "DZ", "AI",
};

/*
 *	Creates the CharSelect menu.
 */
D2Menu_CharSelect::D2Menu_CharSelect(char** pszSavePaths, int nNumFiles)
{
	bool bPreloadedSave = (pszSavePaths != nullptr);
	D2SaveHeader header{ 0 };
	fs_handle f;

	// Should match the one in the main menu
	backgroundTexture =
		trap->R_RegisterDC6Texture("data\\global\\ui\\CharSelect\\characterselectscreenEXP.dc6", "charselect", 0, 11, PAL_UNITS);

	// Create the panels and add them to the menu panel list
	m_charSelectPanel = new D2Panel_CharSelect();
	m_charDeletePanel = new D2Panel_CharDeleteConfirm();

	AddPanel(m_charSelectPanel, true);
	AddPanel(m_charDeletePanel, false);

	// We have to load the saves since we did not get them from the previous call
	if (!bPreloadedSave)
	{
		pszSavePaths = trap->FS_ListFilesInDirectory("Save", "*.d2s", &nNumFiles);
	}

	// Iterate through the save files. We tell the select panel to add character data to the display.
	for (int i = 0; i < nNumFiles; i++)
	{
		// Read only the header file of each one. It contains all the information we need.
		trap->FS_Open(pszSavePaths[i], &f, FS_READ, true);
		if (f == INVALID_HANDLE)
		{
			continue;
		}
		trap->FS_Read(f, &header, sizeof(header), 1);
		trap->FS_CloseFile(f);

		// Pass the header to the panel
		m_charSelectPanel->LoadSave(header, pszSavePaths[i]);
	}

	// Don't forget to free the file list!
	if (!bPreloadedSave)
	{
		trap->FS_FreeFileList(pszSavePaths, nNumFiles);
	}

	// Select the first save in the list
	m_charSelectPanel->SelectSave(0);
}

/*
 *	Frees up information related to the CharSelect menu.
 */
D2Menu_CharSelect::~D2Menu_CharSelect()
{
	if (bJoiningGame)
	{
		// If we are creating/joining a game, we should free up the background texture.
		trap->R_DeregisterTexture(nullptr, backgroundTexture);
	}

	delete m_charSelectPanel;
	delete m_charDeletePanel;
}

/*
 *	Draws the CharSelect menu
 */
void D2Menu_CharSelect::Draw()
{
	// Draw the background
	trap->R_DrawTexture(backgroundTexture, 0, 0, 800, 600, 0, 0);

	// And draw all the panels too
	DrawAllPanels();
}

/*
 *	We were asked to delete a character. Go ahead and pop up the deletion confirmation.
 */
void D2Menu_CharSelect::AskForDeletionConfirmation()
{
	ShowPanel(m_charDeletePanel);
}

/*
 *	We clicked "YES" on the dialogue asking us if we wanted to delete the character.
 */
void D2Menu_CharSelect::DeleteConfirmed()
{
	HidePanel(m_charDeletePanel);
}

/*
 *	We clicked "NO" on the dialogue asking us if we wanted to delete the character.
 */
void D2Menu_CharSelect::DeleteCanceled()
{
	HidePanel(m_charDeletePanel);
}

/*
 *	Returns true if we are allowed to choose this character to play (ie, not dead Hardcore player)
 */
bool D2Menu_CharSelect::CharacterChosen()
{
	// don't check this for now
	return true;
}