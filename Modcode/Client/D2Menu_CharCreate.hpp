#pragma once
#include "../../Shared/D2Common_Shared.hpp"
#include "D2Menu.hpp"
#include "D2Panel_CharCreate.hpp"

// Each class has these animations. Some have special extra animations which are drawn as an overlay.
enum CharCreateAnims
{
	CCA_IdleBack,		// Idle in back, not being hovered over
	CCA_IdleBackSel,	// Idle in back, being hovered over
	CCA_IdleFront,		// Idle in front
	CCA_BackToFront,	// Character is moving from back to front
	CCA_FrontToBack,	// Character is moving from front to back
	CCA_MAX,
};

// Each class has a CharCreateData. As we mess around in the Character Create screen, this becomes filled up.
struct CharCreateData
{
	IRenderObject* displayObject;
	IRenderObject* specialAnimationObject;
	IGraphicsReference* animationHandle[CCA_MAX];
	IGraphicsReference* specialAnimationHandle[CCA_MAX];

	bool bSpecialAnimPresent[CCA_MAX];

	char16_t* szCharClassName;
	char16_t* szCharClassDescription;
	CharCreateAnims status;
	int nDrawXPos;
	int nDrawYPos;
	int nDrawBaselineY[CCA_MAX];
	int nSpecialYOffset;

	// These are all callbacks
	static void OnCharacterSteppedBack(class IRenderObject* caller, void* extraData);
	static void OnCharacterCameForward(class IRenderObject* caller, void* extraData);
};

class D2Menu_CharCreate : public D2Menu
{
private:
	IRenderObject* backgroundTexture;
	IRenderObject* fireAnimation;
	IRenderObject* chooseClassTitle;
	IRenderObject* chosenClassName;
	IRenderObject* chosenClassDescription;

	IGraphicsReference* fireGraphic;

	int m_nHighlightedClass;
	int m_nSelectedClass;

	D2Panel_CharCreate_Static* pStaticPanel;
	D2Panel_CharCreate_Dynamic* pDynamicPanel;

	CharCreateData CreateData[D2CLASS_MAX];
	char16_t *szChooseClassStr;

public:
	bool	m_bFromCharSelect;

	D2Menu_CharCreate(bool bFromCharSelect = false);
	virtual ~D2Menu_CharCreate();

	virtual void Draw();
	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
	virtual void HandleTextInput(char* szText);
	virtual bool HandleKeyDown(DWORD dwKey);

	// Character class needs to walk away from the fire
	void DeselectCharacterClass(int classNum);
	void SelectCharacterClass(int classNum);

	bool TrySaveCharacter();
};