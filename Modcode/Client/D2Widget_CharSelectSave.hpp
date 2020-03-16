#pragma once
#include "D2Widget.hpp"

/**
 *	D2Widget_CharSelectSave is contained within a D2Widget_CharSelectList.
 *	It's a linked list of elements to draw.
 */
class D2Widget_CharSelectSave : public D2Widget
{
private:
	D2Widget_CharSelectSave* nextInChain;
	char16_t charName[32];
	bool bIsSelected, bHasTitle, bIsDeadHardcore, bIsExpansion, bIsHardcore;

	const int nSlotWidth = 272;
	const int nSlotHeight = 93;

	void OnSelected();

	IRenderObject* selectionFrame;
	IRenderObject* characterPreview;
	IRenderObject* characterTitle;
	IRenderObject* characterName;
	IRenderObject* characterLevelAndClass;
	IRenderObject* expansionText;

public:
	D2Widget_CharSelectSave(const char* characterSave, D2SaveHeader& header);
	virtual ~D2Widget_CharSelectSave();

	void SetDrawPosition(uint32_t x, uint32_t y);

	virtual void Draw();
	
	bool CheckMouseDownInChain(DWORD dwX, DWORD dwY, int& counter);
	bool CheckMouseClickInChain(DWORD dwX, DWORD dwY, int& counter);

	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY) {}
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY) {}
	
	void SetNextInChain(D2Widget_CharSelectSave* next);
	void DrawLink(int counter, bool bDrawLeft);
	void Select(int counter);
	void DeselectAllInChain();

	char16_t* GetSelectedCharacterName();
};