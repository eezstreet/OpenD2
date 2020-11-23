#pragma once
#include "../D2Panel.hpp"
#include "../Widgets/CharSelectList.hpp"
#include "../Widgets/TextEntry.hpp"
#include "../Widgets/Button.hpp"

namespace D2Panels
{
	// The char select panel includes everything that is on the charselect screen.
	class CharSelect : public D2Panel
	{
	private:
		D2Widgets::Button* createCharButton;
		D2Widgets::Button* deleteCharButton;
		D2Widgets::Button* convertExpansionButton;

		D2Widgets::Button* okButton;
		D2Widgets::Button* exitButton;
		D2Widgets::CharSelectList* charSelectList;

		IRenderObject* characterDisplayName;
	public:
		CharSelect();
		virtual ~CharSelect();

		void LoadSave(D2SaveHeader& pSaveHeader, char* path);
		void InvalidateSelection();
		void ValidateSelection();
		void SelectSave(int nNewSave);

		void AssignSelectedSave();

		virtual void Draw();
	};

	// This panel pops up to verify if we want to delete the character
	class CharDeleteConfirm : public D2Panel
	{
	private:
		D2Widgets::Button* confirmYesButton;
		D2Widgets::Button* confirmNoButton;

		IRenderObject* renderObject;
		tex_handle					background;

	public:
		CharDeleteConfirm();
		~CharDeleteConfirm();

		virtual void Draw();
	};
}