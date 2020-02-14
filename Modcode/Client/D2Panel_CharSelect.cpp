#include "D2Panel_CharSelect.hpp"
#include "D2Menu_CharSelect.hpp"
#include "D2Menu_Main.hpp"
#include "D2Menu_Loading.hpp"
#include "D2Menu_CharCreate.hpp"

#define TBLTEXT_EXIT			5101	// "Exit"
#define TBLTEXT_OK				5102	// "OK"
#define TBLTEXT_CANCEL			5103	// "Cancel"
#define TBLTEXT_YES				5166	// "YES"
#define TBLTEXT_NO				5167	// "NO"
#define TBLTEXT_CONVERTCHAR		22742	// "CONVERT TO EXPANSION"
#define TBLTEXT_CREATECHAR		22743	// "CREATE NEW CHARACTER"
#define TBLTEXT_DELETECHAR		22744	// "DELETE CHARACTER"
//#define TBLTEXT_EXPANSIONCHAR	22731	// "EXPANSION CHARACTER"

/////////////////////////////////////////////////////////////////
//
//	THE MAIN CHARACTER SELECTION PANEL

/*
 *	Handles a signal from a button that was pressed.
 */
static void PanelSignal(D2Panel* pCallerPanel, D2Widget* pCallerWidget)
{
	D2Menu_CharSelect* pCharMenu = dynamic_cast<D2Menu_CharSelect*>(cl.pActiveMenu);
	D2Panel_CharSelect* pCharPanel = dynamic_cast<D2Panel_CharSelect*>(pCallerPanel);

	if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "cs_exit"))
	{
		// Exit button pressed
		delete cl.pActiveMenu;
		D2Client_GoToContextMenu();
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "cs_ok"))
	{
		// OK button pressed
		if (pCharMenu->CharacterChosen())
		{
			pCharPanel->AssignSelectedSave();
			delete cl.pActiveMenu;
			cl.pActiveMenu = nullptr;
			cl.pLoadingMenu = new D2Menu_Loading();
			cl.gamestate = GS_LOADING;
			D2Client_SetupServerConnection();
			return;
		}
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "cs_delete"))
	{
		// Delete character button pressed
		pCharMenu->AskForDeletionConfirmation();
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "cs_convert"))
	{
		// Convert character button pressed
		// Currently doesn't do anything.
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "cs_new"))
	{
		// New character button pressed
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menu_CharCreate(true);
		return;
	}
}

/*
 *	Creates the CharSelect panel
 */
D2Panel_CharSelect::D2Panel_CharSelect() : D2Panel()
{
	createCharButton = new D2Widget_Button(34, 467, "data\\global\\ui\\CharSelect\\TallButtonBlank.dc6", "tallButton", 0, 0, 1, 1, 1, 1);
	deleteCharButton = new D2Widget_Button(432, 467, "data\\global\\ui\\CharSelect\\TallButtonBlank.dc6", "tallbutton", 0, 0, 1, 1, 1, 1);
	convertExpansionButton = new D2Widget_Button(232, 467, "data\\global\\ui\\CharSelect\\TallButtonBlank.dc6", "tallbutton", 0, 0, 1, 1, 1, 1);

	okButton = new D2Widget_Button(628, 538, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
	exitButton = new D2Widget_Button(34, 538, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
	charSelectList = new D2Widget_CharSelectList(37, 86, 548, 370);

	AddWidget(createCharButton);
	AddWidget(deleteCharButton);
	AddWidget(convertExpansionButton);
	AddWidget(okButton);
	AddWidget(exitButton);
	AddWidget(charSelectList);

	createCharButton->AttachClickSignal(PanelSignal);
	deleteCharButton->AttachClickSignal(PanelSignal);
	convertExpansionButton->AttachClickSignal(PanelSignal);
	okButton->AttachClickSignal(PanelSignal);
	exitButton->AttachClickSignal(PanelSignal);

	createCharButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CREATECHAR));
	deleteCharButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_DELETECHAR));
	convertExpansionButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CONVERTCHAR));
	okButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_OK));
	exitButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_EXIT));

	createCharButton->AttachIdentifier("cs_new");
	deleteCharButton->AttachIdentifier("cs_delete");
	convertExpansionButton->AttachIdentifier("cs_convert");
	okButton->AttachIdentifier("cs_ok");
	exitButton->AttachIdentifier("cs_exit");
}

/*
 *	Destroys the CharSelect panel 
 */
D2Panel_CharSelect::~D2Panel_CharSelect()
{
	delete createCharButton;
	delete deleteCharButton;
	delete convertExpansionButton;
	delete okButton;
	delete exitButton;
	delete charSelectList;
}

/*
 *	Draws the CharSelect panel
 */
void D2Panel_CharSelect::Draw()
{
	// Draw the character name
	char16_t* szCharName = charSelectList->GetSelectedCharacterName();

	if (szCharName && szCharName[0])
	{
		engine->renderer->DrawText(cl.font42, szCharName, 34, 16, 564, 51, ALIGN_CENTER, ALIGN_CENTER);
	}

	// Draw the widgets
	DrawAllWidgets();
}

/*
 *	We just received a save from the owning menu. Add it to the CharSelectList.
 */
void D2Panel_CharSelect::LoadSave(D2SaveHeader& save, char* path)
{
	if (charSelectList != nullptr)
	{
		charSelectList->AddSave(save, path);
	}
}

/*
 *	We just got told to select a save in the save list widget.
 *	@author	eezstreet
 */
void D2Panel_CharSelect::SelectSave(int nSaveNumber)
{
	if (charSelectList != nullptr)
	{
		charSelectList->Selected(nSaveNumber);
	}
}

/*
 *	We got told to load up the save.
 *	@author	eezstreet
 */
void D2Panel_CharSelect::AssignSelectedSave()
{
	// Propagate this to the char select list
	if (charSelectList != nullptr)
	{
		charSelectList->LoadSave();
	}
}

/*
 *	We just received word that we selected an invalid character.
 *	@author	eezstreet
 */
void D2Panel_CharSelect::InvalidateSelection()
{
	deleteCharButton->Disable();
	convertExpansionButton->Disable();
	okButton->Disable();
}

/*
 *	We just received word that we selected a valid character.
 *	@author	eezstreet
 */
void D2Panel_CharSelect::ValidateSelection()
{
	deleteCharButton->Enable();
	convertExpansionButton->Enable();
	okButton->Enable();
}

/////////////////////////////////////////////////////////////////
//
//	CONFIRM DELETION PANEL

/*
 *	Signals for when a button is pressed
 */
static void PanelSignal_DeleteConfirm(D2Panel* pCallerPanel, D2Widget* pCallerWidget)
{
	D2Menu_CharSelect* pCharMenu = dynamic_cast<D2Menu_CharSelect*>(cl.pActiveMenu);

	if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "csc_ok"))
	{
		// Delete the selected character
		pCharMenu->DeleteConfirmed();
		return;
	}
	else if (!D2Lib::stricmp(pCallerWidget->GetIdentifier(), "csc_cancel"))
	{
		// I changed my mind, go back!
		pCharMenu->DeleteCanceled();
		return;
	}
}

/*
 *	Initializes the delete confirmation panel
 */
D2Panel_CharDeleteConfirm::D2Panel_CharDeleteConfirm() : D2Panel()
{
	confirmYesButton = new D2Widget_Button(420, 340, "data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6", "tiny", 0, 0, 1, 1, 0, 0);
	confirmNoButton = new D2Widget_Button(280, 340, "data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6", "tiny", 0, 0, 1, 1, 0, 0);

	background = engine->renderer->TextureFromStitchedDC6("data\\global\\ui\\FrontEnd\\PopUpOkCancel2.dc6", "PopUpOkCancel2", 0, 1, PAL_UNITS);

	AddWidget(confirmYesButton);
	AddWidget(confirmNoButton);

	confirmYesButton->AttachClickSignal(PanelSignal_DeleteConfirm);
	confirmNoButton->AttachClickSignal(PanelSignal_DeleteConfirm);

	confirmYesButton->AttachIdentifier("csc_ok");
	confirmNoButton->AttachIdentifier("csc_cancel");

	confirmYesButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_YES));
	confirmNoButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_NO));
}

/*
 *	Clean up the delete confirmation panel
 */
D2Panel_CharDeleteConfirm::~D2Panel_CharDeleteConfirm()
{
	// Delete the widgets and deregister the popup texture (this is literally the only place it's used)
	delete confirmYesButton;
	delete confirmNoButton;

	engine->renderer->DeregisterTexture("PopUpOkCancel2", INVALID_HANDLE);
}

/*
 *	Draws the confirmation panel for deleting a character.
 *	FIXME: in the original game, the delete confirmation darkens out the rest of the screen and prevents clicking.
 */
void D2Panel_CharDeleteConfirm::Draw()
{
	// Draw the background
	engine->renderer->DrawTexture(background, 268, 212, 264, 176, 0, 0);

	// And the widgets too
	DrawAllWidgets();
}