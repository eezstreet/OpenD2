#pragma once
#include "D2Widget.hpp"
#include "D2Widget_Scrollbar.hpp"

class D2Widget_CharSelectList : public D2Widget
{
private:
	D2Widget_Scrollbar*		pScrollBar;
	class D2Widget_CharSelectSave* saves;
	int nCurrentScroll;
	int nCurrentSelection;
	int nNumberSaves;

	void Clicked(DWORD dwX, DWORD dwY);
public:
	D2Widget_CharSelectList(int x, int y, int w, int h);
	virtual ~D2Widget_CharSelectList();

	void AddSave(D2SaveHeader& header, char* path);
	char16_t* GetSelectedCharacterName();
	void LoadSave();

	virtual void OnWidgetAdded();
	virtual void Draw();
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);

	void Selected(int nNewSelection);
};