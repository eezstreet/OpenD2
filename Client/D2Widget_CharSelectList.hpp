#pragma once
#include "D2Widget.hpp"
#include "D2Widget_Scrollbar.hpp"

class D2Widget_CharSelectList : public D2Widget
{
private:
	D2Widget_Scrollbar*		pScrollBar;

	struct CharacterSaveData
	{
		D2SaveHeader header;
		char16_t name[16];	// The name gets converted into UTF-16 when we load it
		char path[MAX_D2PATH_ABSOLUTE];
		CharacterSaveData* pNext;
	};
	CharacterSaveData* pCharacterData;
	int nCurrentScroll;
	int nCurrentSelection;
	int nNumberSaves;

	void DrawSaveSlot(CharacterSaveData* pCharacterData, int nSlot);
public:
	D2Widget_CharSelectList(int x, int y, int w, int h);
	~D2Widget_CharSelectList();

	void AddSave(D2SaveHeader& header, char* path);

	virtual void Draw();
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);
};