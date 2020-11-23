#pragma once
#include "../D2Menu.hpp"
#include "../Panels/CharSelect.hpp"

namespace D2Menus
{
	class CharSelect : public D2Menu
	{
	private:
		IGraphicsReference* backgroundTexture;
		IRenderObject* backgroundObject;

		// if bJoiningGame is true, then the background texture will be deregistered when this menu is destroyed.
		bool		bJoiningGame;

		D2Panels::CharSelect* m_charSelectPanel;
		D2Panels::CharDeleteConfirm* m_charDeletePanel;
	public:
		CharSelect(char** pszSavePaths = nullptr, int nNumFiles = 0);
		virtual ~CharSelect();

		void AskForDeletionConfirmation();
		void DeleteConfirmed();
		void DeleteCanceled();

		bool CharacterChosen();
		virtual void Draw();
	};
}