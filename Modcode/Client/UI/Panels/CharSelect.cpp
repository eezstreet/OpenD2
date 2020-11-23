#include "CharSelect.hpp"
#include "../Menus/CharSelect.hpp"
#include "../Menus/Main.hpp"
#include "../Menus/Loading.hpp"
#include "../Menus/CharCreate.hpp"

#define TBLTEXT_EXIT			5101	// "Exit"
#define TBLTEXT_OK				5102	// "OK"
#define TBLTEXT_CANCEL			5103	// "Cancel"
#define TBLTEXT_YES				5166	// "YES"
#define TBLTEXT_NO				5167	// "NO"
#define TBLTEXT_CONVERTCHAR		22742	// "CONVERT TO EXPANSION"
#define TBLTEXT_CREATECHAR		22743	// "CREATE NEW CHARACTER"
#define TBLTEXT_DELETECHAR		22744	// "DELETE CHARACTER"
//#define TBLTEXT_EXPANSIONCHAR	22731	// "EXPANSION CHARACTER"

namespace D2Panels
{
	/*
	 *	Creates the CharSelect panel
	 */
	CharSelect::CharSelect() : D2Panel()
	{
		characterDisplayName = engine->renderer->AllocateObject(1);

		engine->renderer->SetGlobalPalette(PAL_SKY);
		createCharButton = new D2Widgets::Button(34, 467, "data\\global\\ui\\CharSelect\\TallButtonBlank.dc6", "tallButton", 0, 0, 1, 1, 1, 1);
		deleteCharButton = new D2Widgets::Button(432, 467, "data\\global\\ui\\CharSelect\\TallButtonBlank.dc6", "tallbutton", 0, 0, 1, 1, 1, 1);
		convertExpansionButton = new D2Widgets::Button(232, 467, "data\\global\\ui\\CharSelect\\TallButtonBlank.dc6", "tallbutton", 0, 0, 1, 1, 1, 1);

		okButton = new D2Widgets::Button(628, 538, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
		exitButton = new D2Widgets::Button(34, 538, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
		charSelectList = new D2Widgets::CharSelectList(37, 86, 548, 370, characterDisplayName);

		//AddWidget(createCharButton);
		//AddWidget(deleteCharButton);
		//AddWidget(convertExpansionButton);
		AddWidget(okButton);
		AddWidget(exitButton);
		AddWidget(charSelectList);

		//createCharButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CREATECHAR));
		//deleteCharButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_DELETECHAR));
		//convertExpansionButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_CONVERTCHAR));
		okButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_OK));
		exitButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_EXIT));


		characterDisplayName->AttachFontResource(cl.font42);
	}

	/*
	 *	Destroys the CharSelect panel
	 */
	CharSelect::~CharSelect()
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
	void CharSelect::Draw()
	{
		characterDisplayName->Draw();

		// Draw the widgets
		DrawAllWidgets();
	}

	/*
	 *	We just received a save from the owning menu. Add it to the CharSelectList.
	 */
	void CharSelect::LoadSave(D2SaveHeader& save, char* path)
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
	void CharSelect::SelectSave(int nSaveNumber)
	{
		if (charSelectList != nullptr)
		{
			charSelectList->Selected(nSaveNumber);

			// Update the name displayed up top, and center it.
			int32_t width, height;
			characterDisplayName->SetText(charSelectList->GetSelectedCharacterName());
			characterDisplayName->GetDrawCoords(nullptr, nullptr, &width, &height);
			characterDisplayName->SetDrawCoords(316 - (width / 2), 41 - (height / 2), 0, 0);
		}
	}

	/*
	 *	We got told to load up the save.
	 *	@author	eezstreet
	 */
	void CharSelect::AssignSelectedSave()
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
	void CharSelect::InvalidateSelection()
	{
		deleteCharButton->Disable();
		convertExpansionButton->Disable();
		okButton->Disable();
	}

	/*
	 *	We just received word that we selected a valid character.
	 *	@author	eezstreet
	 */
	void CharSelect::ValidateSelection()
	{
		deleteCharButton->Enable();
		convertExpansionButton->Enable();
		okButton->Enable();
	}

	/////////////////////////////////////////////////////////////////
	//
	//	CONFIRM DELETION PANEL

	/*
	 *	Initializes the delete confirmation panel
	 */
	CharDeleteConfirm::CharDeleteConfirm() : D2Panel()
	{
		confirmYesButton = new D2Widgets::Button(420, 340, "data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6", "tiny", 0, 0, 1, 1, 0, 0);
		confirmNoButton = new D2Widgets::Button(280, 340, "data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6", "tiny", 0, 0, 1, 1, 0, 0);

		renderObject = engine->renderer->AllocateObject(0);

		IGraphicsReference* backgroundHandle = engine->graphics->CreateReference("data\\global\\ui\\FrontEnd\\PopUpOkCancel2.dc6", UsagePolicy_SingleUse);
		renderObject->AttachCompositeTextureResource(backgroundHandle, 0, 1);
		renderObject->SetDrawCoords(268, 212, 264, 176);
		engine->graphics->DeleteReference(backgroundHandle);

		AddWidget(confirmYesButton);
		AddWidget(confirmNoButton);

		confirmYesButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_YES));
		confirmNoButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_NO));
	}

	/*
	 *	Clean up the delete confirmation panel
	 */
	CharDeleteConfirm::~CharDeleteConfirm()
	{
		// Delete the widgets and deregister the popup texture (this is literally the only place it's used)
		delete confirmYesButton;
		delete confirmNoButton;

		engine->renderer->Remove(renderObject);
	}

	/*
	 *	Draws the confirmation panel for deleting a character.
	 *	FIXME: in the original game, the delete confirmation darkens out the rest of the screen and prevents clicking.
	 */
	void CharDeleteConfirm::Draw()
	{
		// Draw the background
		renderObject->Draw();

		// And the widgets too
		DrawAllWidgets();
	}
}