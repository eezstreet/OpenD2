#pragma once
#include "../D2Widget.hpp"

namespace D2Widgets
{
	class Checkbox : public D2Widget
	{
	private:
		IRenderObject* checkboxRendered;
		IRenderObject* label;

		// FIXME: ...? this should be an atlas
		IGraphicsReference* checkboxTexture;
		IGraphicsReference* checkboxTextureChecked;

		char16_t szLabel[32];
		bool m_bHasLabel;

		bool m_bChecked;
	public:
		Checkbox(int x, int y, bool bStartChecked);
		virtual ~Checkbox();

		bool IsChecked() { return m_bChecked; }

		void AttachLabel(char16_t* szText);
		void RemoveLabel();

		virtual bool HandleMouseDown(DWORD dwX, DWORD dwY);
		virtual bool HandleMouseClick(DWORD dwX, DWORD dwY);
		virtual void Draw();
	};
}