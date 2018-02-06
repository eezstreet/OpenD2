/////////////////////////////////////////////////////
//
//	There are two panels as part of the character creation menu:
//	The Static panel is always present and includes the OK and BACK buttons.
//	The Dynamic panel is only present when a character class is selected.

#pragma once
#include "D2Panel.hpp"
#include "D2Widget_Button.hpp"
#include "D2Widget_Checkbox.hpp"
#include "D2Widget_TextEntry.hpp"

class D2Panel_CharCreate_Static : public D2Panel
{
private:
	D2Widget_Button* pCancelButton;
	D2Widget_Button* pOKButton;
public:
	virtual void Draw();
	void EnableOKButton() { pOKButton->Enable(); }
	void DisableOKButton() { pOKButton->Disable(); }

	D2Panel_CharCreate_Static();
	virtual ~D2Panel_CharCreate_Static();
};

class D2Panel_CharCreate_Dynamic : public D2Panel
{
private:
	D2Widget_TextEntry* pNameEntry;
	D2Widget_Checkbox* pExpansionCheckbox;
	D2Widget_Checkbox* pHardcoreCheckbox;

public:
	virtual void Draw();
	size_t GetNameLength() { return pNameEntry->GetTextLength(); }
	char16_t* GetName() { return pNameEntry->GetText(); }

	bool HardcoreChecked() { return pHardcoreCheckbox->IsChecked(); }
	bool ExpansionChecked() { return pExpansionCheckbox->IsChecked(); }

	D2Panel_CharCreate_Dynamic();
	~D2Panel_CharCreate_Dynamic();
};