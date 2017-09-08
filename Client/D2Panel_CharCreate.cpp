#include "D2Panel_CharCreate.hpp"
#include "D2Menu_Main.hpp"

#define TBLTEXT_EXIT			5101
#define TBLTEXT_OK				5102
#define SMALL_BUTTON_DC6		"data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6"

/*
 *
 *	COMMON CODE
 *
 */
static void PanelSignal(D2Panel* pCallerPanel, D2Widget* pCallerWidget)
{
	if (!D2_stricmp(pCallerWidget->GetIdentifier(), "cc_cancel"))
	{
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menu_Main();
		return;
	}
	else if (!D2_stricmp(pCallerWidget->GetIdentifier(), "cc_ok"))
	{
		return;
	}
}

/*
 *
 *	STATIC PANEL
 *
 */

D2Panel_CharCreate_Static::D2Panel_CharCreate_Static() : D2Panel()
{
	pCancelButton = new D2Widget_Button(35, 535, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);
	pOKButton = new D2Widget_Button(630, 535, SMALL_BUTTON_DC6, "medium", 0, 0, 1, 1, 0, 0);

	AddWidget(pCancelButton);
	AddWidget(pOKButton);

	pCancelButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_EXIT));
	pOKButton->AttachText(trap->TBL_FindStringFromIndex(TBLTEXT_OK));

	pCancelButton->AttachClickSignal(PanelSignal);
	pOKButton->AttachClickSignal(PanelSignal);

	pCancelButton->AttachIdentifier("cc_cancel");
	pOKButton->AttachIdentifier("cc_ok");

	pOKButton->Disable();
}

D2Panel_CharCreate_Static::~D2Panel_CharCreate_Static()
{
	delete pCancelButton;
	delete pOKButton;
}

void D2Panel_CharCreate_Static::Draw()
{
	DrawAllWidgets();
}

/*
 *
 *	DYNAMIC PANEL
 *
 */

D2Panel_CharCreate_Dynamic::D2Panel_CharCreate_Dynamic() : D2Panel()
{

}

D2Panel_CharCreate_Dynamic::~D2Panel_CharCreate_Dynamic()
{

}

void D2Panel_CharCreate_Dynamic::Draw()
{
	DrawAllWidgets();
}