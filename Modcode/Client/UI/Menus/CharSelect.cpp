#include "CharSelect.hpp"

static char* gszTokenNames[D2CLASS_MAX] = {
	"AM", "SO", "NE", "PA", "BA", "DZ", "AI",
};

namespace D2Menus
{
	/*
	 *	Creates the CharSelect menu.
	 */
	CharSelect::CharSelect(char** pszSavePaths, int nNumFiles)
	{
		bool bPreloadedSave = (pszSavePaths != nullptr);
		D2SaveHeader header{ 0 };
		fs_handle f;

		// Create the background.
		backgroundTexture = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\characterselectscreenEXP.dc6", UsagePolicy_SingleUse);
		backgroundObject = engine->renderer->AllocateObject(0);
		backgroundObject->AttachCompositeTextureResource(backgroundTexture, 0, -1);
		backgroundObject->SetDrawCoords(0, 0, 800, 600);

		// Create the panels and add them to the menu panel list
		m_charSelectPanel = new D2Panels::CharSelect();
		m_charDeletePanel = new D2Panels::CharDeleteConfirm();

		AddPanel(m_charSelectPanel, true);
		AddPanel(m_charDeletePanel, false);

		// We have to load the saves since we did not get them from the previous call
		if (!bPreloadedSave)
		{
			pszSavePaths = engine->FS_ListFilesInDirectory("Save", "*.d2s", &nNumFiles);
		}

		// Iterate through the save files. We tell the select panel to add character data to the display.
		for (int i = 0; i < nNumFiles; i++)
		{
			// Read only the header file of each one. It contains all the information we need.
			engine->FS_Open(pszSavePaths[i], &f, FS_READ, true);
			if (f == INVALID_HANDLE)
			{
				continue;
			}
			engine->FS_Read(f, &header, sizeof(header), 1);
			engine->FS_CloseFile(f);

			// Pass the header to the panel
			m_charSelectPanel->LoadSave(header, pszSavePaths[i]);
		}

		// Don't forget to free the file list!
		if (!bPreloadedSave)
		{
			engine->FS_FreeFileList(pszSavePaths, nNumFiles);
		}

		// Select the first save in the list
		m_charSelectPanel->SelectSave(0);
	}

	/*
	 *	Frees up information related to the CharSelect menu.
	 */
	CharSelect::~CharSelect()
	{
		// Delete the background texture.
		engine->graphics->DeleteReference(backgroundTexture);
		engine->renderer->Remove(backgroundObject);

		// Delete the panels.
		delete m_charSelectPanel;
		delete m_charDeletePanel;
	}

	/*
	 *	Draws the CharSelect menu
	 */
	void CharSelect::Draw()
	{
		backgroundObject->Draw();

		DrawAllPanels();
	}

	/*
	 *	We were asked to delete a character. Go ahead and pop up the deletion confirmation.
	 */
	void CharSelect::AskForDeletionConfirmation()
	{
		ShowPanel(m_charDeletePanel);
	}

	/*
	 *	We clicked "YES" on the dialogue asking us if we wanted to delete the character.
	 */
	void CharSelect::DeleteConfirmed()
	{
		HidePanel(m_charDeletePanel);
	}

	/*
	 *	We clicked "NO" on the dialogue asking us if we wanted to delete the character.
	 */
	void CharSelect::DeleteCanceled()
	{
		HidePanel(m_charDeletePanel);
	}

	/*
	 *	Returns true if we are allowed to choose this character to play (ie, not dead Hardcore player)
	 */
	bool CharSelect::CharacterChosen()
	{
		// in the future we will want to bring up a popup that states that the hardcore character is dead and cannot be played.
		// however for now, just assume everything is OK

		return true;
	}
}