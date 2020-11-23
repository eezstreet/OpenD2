#include "CharCreate.hpp"
#include "../Menus/Main.hpp"
#include "../Menus/Loading.hpp"
#include "../Menus/CharCreate.hpp"
#include "../Menus/CharSelect.hpp"

#define TBLTEXT_EXIT			5101
#define TBLTEXT_OK				5102

namespace D2Panels
{
	/*
	 *
	 *	STATIC PANEL
	 *
	 */
	CharCreate_Static::CharCreate_Static() : D2Panel()
	{
		pCancelButton = new D2Widgets::Button(35, 535, SMALL_BUTTON_FETCHAR_DC6, "medium", 0, 0, 1, 1, 0, 0);
		pOKButton = new D2Widgets::Button(630, 535, SMALL_BUTTON_FETCHAR_DC6, "medium", 0, 0, 1, 1, 0, 0);

		AddWidget(pCancelButton);
		AddWidget(pOKButton);

		pCancelButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_EXIT));
		pOKButton->AttachText(engine->TBL_FindStringFromIndex(TBLTEXT_OK));

		pCancelButton->AttachIdentifier("cc_cancel");
		pOKButton->AttachIdentifier("cc_ok");

		pCancelButton->AddEventListener(Clicked, [] {
			delete cl.pActiveMenu;
			cl.pActiveMenu = new D2Menus::CharSelect();
			});

		pOKButton->Disable();
	}

	CharCreate_Static::~CharCreate_Static()
	{
		delete pCancelButton;
		delete pOKButton;
	}

	void CharCreate_Static::Draw()
	{
		DrawAllWidgets();
	}

	/*
	 *
	 *	DYNAMIC PANEL
	 *
	 */

	CharCreate_Dynamic::CharCreate_Dynamic() : D2Panel()
	{
		x = 320;
		y = 490;

		pNameEntry = new D2Widgets::TextEntry(0, 0, true, true, true, false);
		pExpansionCheckbox = new D2Widgets::Checkbox(0, 35, true);
		pHardcoreCheckbox = new D2Widgets::Checkbox(0, 55, false);

		AddWidget(pNameEntry);
		AddWidget(pExpansionCheckbox);
		AddWidget(pHardcoreCheckbox);

		pNameEntry->AttachLabel(engine->TBL_FindStringFromIndex(5125));
		pExpansionCheckbox->AttachLabel(engine->TBL_FindStringFromIndex(22731));
		pHardcoreCheckbox->AttachLabel(engine->TBL_FindStringFromIndex(5126));
	}

	CharCreate_Dynamic::~CharCreate_Dynamic()
	{
		delete pNameEntry;
		delete pExpansionCheckbox;
		delete pHardcoreCheckbox;
	}

	void CharCreate_Dynamic::Draw()
	{
		DrawAllWidgets();
	}
}