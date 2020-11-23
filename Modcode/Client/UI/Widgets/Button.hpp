#pragma once
#include "../D2Widget.hpp"

enum ButtonWidgetClickEvent {
	Clicked,
	Down,
	Up,
};

namespace D2Widgets
{
	class Button : public D2Widget
	{
	private:
		bool bDisabled;
		bool bDown;
		bool bMovedText;
		bool bHasText;
		bool bHasClickSignal;
		bool bAlphaModulateDisable;

		char16_t buttonTextBuffer[64]{ 0 };

		tex_handle texture_up;
		tex_handle texture_down;
		tex_handle texture_disabled;

		IRenderObject* text;
		IGraphicsReference* font;
		sfx_handle clickedSound;

		IRenderObject* backgroundObjectUp;
		IRenderObject* backgroundObjectDown;
		IRenderObject* backgroundObjectDisabled;

		UnorderedDelegateList<> OnClicked;
		UnorderedDelegateList<> OnDown;
		UnorderedDelegateList<> OnUp;
	public:
		Button(int x, int y, char* szDC6Path, char* szButtonType,
			DWORD dwStartEnabled, DWORD dwEndEnabled,
			DWORD dwStartDown, DWORD dwEndDown,
			DWORD dwStartDisabled, DWORD dwEndDisabled);
		virtual ~Button();

		void AttachText(char16_t* text);
		void DetachText();

		void Disable() { bDisabled = true; }
		void Enable() { bDisabled = false; }

		handle AddEventListener(ButtonWidgetClickEvent eventType, void (*function)());
		void RemoveEventListener(ButtonWidgetClickEvent eventType, handle eventHandle);

		void SetFont(IGraphicsReference* newFont) { font = newFont; }

		virtual void Draw();
		virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
		virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);

		tex_handle texture;
	};
}