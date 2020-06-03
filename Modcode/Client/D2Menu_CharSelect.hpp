#pragma once
#include "D2Menu.hpp"
#include "D2Panel_CharSelect.hpp"

class D2Menu_CharSelect : public D2Menu
{
private:
	tex_handle	backgroundTexture;

	// if bJoiningGame is true, then the background texture will be deregistered when this menu is destroyed.
	bool		bJoiningGame;

	D2Panel_CharSelect* m_charSelectPanel;
	D2Panel_CharDeleteConfirm* m_charDeletePanel;
public:
	explicit D2Menu_CharSelect(char** pszSavePaths = nullptr, int nNumFiles = 0);
	~D2Menu_CharSelect() override;

	void AskForDeletionConfirmation();
	void DeleteConfirmed();
	void DeleteCanceled();

        bool CharacterChosen();
	void Draw() override;
};
