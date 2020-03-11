#pragma once
#include "D2Widget.hpp"

class D2Widget_Button : public D2Widget
{
private:
	bool bDisabled;
	bool bDown;
	bool bMovedText;
	bool bHasText;
	bool bHasClickSignal;
	bool bAlphaModulateDisable;

	char16_t buttonTextBuffer[64]{ 0 };
	MenuSignal clickSignal;
	
	tex_handle texture_up;
	tex_handle texture_down;
	tex_handle texture_disabled;

	IRenderObject* text;
	IGraphicsHandle* font;
	sfx_handle clickedSound;

	IRenderObject* backgroundObjectUp;
	IRenderObject* backgroundObjectDown;
	IRenderObject* backgroundObjectDisabled;
public:
	D2Widget_Button(int x, int y, char* szDC6Path, char* szButtonType,
		DWORD dwStartEnabled, DWORD dwEndEnabled,
		DWORD dwStartDown, DWORD dwEndDown, 
		DWORD dwStartDisabled, DWORD dwEndDisabled);
	virtual ~D2Widget_Button();

	void AttachText(char16_t* text);
	void DetachText();

	void Disable() { bDisabled = true; }
	void Enable() { bDisabled = false; }

	void AttachClickSignal(MenuSignal pClickSignal);
	void RemoveClickSignal() { bHasClickSignal = false; }

	void SetFont(IGraphicsHandle* newFont) { font = newFont; }

	virtual void Draw();
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);

	tex_handle texture;
};