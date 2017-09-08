#pragma once
#include "../Shared/D2Common_Shared.hpp"
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
	tex_handle animTextureHandle[CCA_MAX];
	anim_handle animAnimHandle[CCA_MAX];
	bool bSpecialAnimPresent[CCA_MAX];
	tex_handle specialAnimTextureHandle[CCA_MAX];
	anim_handle specialAnimAnimHandle[CCA_MAX];
	char16_t* szCharClassName;
	char16_t* szCharClassDescription;
	CharCreateAnims status;
	int nDrawXPos;
	int nDrawYPos;
};

class D2Menu_CharCreate : public D2Menu
{
private:
	tex_handle backgroundTex;
	anim_handle fireAnim;

	int m_nHighlightedClass;
	int m_nSelectedClass;

	D2Panel_CharCreate_Static* pStaticPanel;
	D2Panel_CharCreate_Dynamic* pDynamicPanel;

	CharCreateData CreateData[D2CLASS_MAX];
	char16_t *szChooseClassStr;

public:
	D2Menu_CharCreate();
	~D2Menu_CharCreate();

	virtual void Draw();
	virtual bool HandleMouseClicked(DWORD dwX, DWORD dwY);
};