/////////////////////////////////////////////////////
//
//	There are two panels as part of the character creation menu:
//	The Static panel is always present and includes the OK and BACK buttons.
//	The Dynamic panel is only present when a character class is selected.

#pragma once
#include "../D2Panel.hpp"
#include "../Widgets/Checkbox.hpp"
#include "../Widgets/TextEntry.hpp"
#include "../Widgets/Button.hpp"

namespace D2Panels
{
	class CharCreate_Static : public D2Panel
	{
	private:
		D2Widgets::Button* pCancelButton;
		D2Widgets::Button* pOKButton;
	public:
		virtual void Draw();
		void EnableOKButton() { pOKButton->Enable(); }
		void DisableOKButton() { pOKButton->Disable(); }

		CharCreate_Static();
		virtual ~CharCreate_Static();
	};

	class CharCreate_Dynamic : public D2Panel
	{
	private:
		D2Widgets::TextEntry* pNameEntry;
		D2Widgets::Checkbox* pExpansionCheckbox;
		D2Widgets::Checkbox* pHardcoreCheckbox;

	public:
		virtual void Draw();
		size_t GetNameLength() { return pNameEntry->GetTextLength(); }
		char16_t* GetName() { return pNameEntry->GetText(); }

		bool HardcoreChecked() { return pHardcoreCheckbox->IsChecked(); }
		bool ExpansionChecked() { return pExpansionCheckbox->IsChecked(); }

		CharCreate_Dynamic();
		~CharCreate_Dynamic();
	};
}