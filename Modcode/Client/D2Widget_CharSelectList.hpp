#pragma once
#include "D2Widget.hpp"
#include "D2Widget_Scrollbar.hpp"

class D2Widget_CharSelectList : public D2Widget
{
private:
	struct CharacterSaveData
	{
		D2SaveHeader header;
		char16_t name[16];	// The name gets converted into UTF-16 when we load it
		char path[MAX_D2PATH_ABSOLUTE];
		CharacterSaveData* pNext;

		ITokenReference* token; // The token
		IRenderObject* renderedToken; // The token as it is rendered
		IRenderObject* frame; // The frame that renders around the selection
		IRenderObject* title; // The title that gets rendered
		IRenderObject* charName;
		IRenderObject* classAndLevel;
		IRenderObject* expansionChar;
	};
	CharacterSaveData* pCharacterData;
	D2Widget_Scrollbar*		pScrollBar;
	class D2Widget_CharSelectSave* saves;
	int nCurrentScroll;
	int nCurrentSelection;
	int nNumberSaves;

	IGraphicsReference* greyFrameRef;
	IGraphicsReference* frameRef;

	const int nSlotWidth = 272;
	const int nSlotHeight = 93;

	void Clicked(DWORD dwX, DWORD dwY);
	void DrawSaveSlot(CharacterSaveData* pSaveData, int nSlot);

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