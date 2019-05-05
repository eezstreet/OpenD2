#pragma once
#include "D2Panel.hpp"
#include "D2Widget_CharSelectList.hpp"
#include "D2Widget_TextEntry.hpp"
#include "D2Widget_Button.hpp"

// The char select panel includes everything that is on the charselect screen.
class D2Panel_CharSelect : public D2Panel
{
private:
	D2Widget_Button*			createCharButton;
	D2Widget_Button*			deleteCharButton;
	D2Widget_Button*			convertExpansionButton;
	
	D2Widget_Button*			okButton;
	D2Widget_Button*			exitButton;
	D2Widget_CharSelectList*	charSelectList;
public:
	D2Panel_CharSelect();
	virtual ~D2Panel_CharSelect();

	void LoadSave(D2SaveHeader& pSaveHeader, char* path);
	void InvalidateSelection();
	void ValidateSelection();
	void SelectSave(int nNewSave);

	void AssignSelectedSave();

	virtual void Draw();
};

// This panel pops up to verify if we want to delete the character
class D2Panel_CharDeleteConfirm : public D2Panel
{
private:
	D2Widget_Button*			confirmYesButton;
	D2Widget_Button*			confirmNoButton;
	
	tex_handle					background;

public:
	D2Panel_CharDeleteConfirm();
	~D2Panel_CharDeleteConfirm();

	virtual void Draw();
};