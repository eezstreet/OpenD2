#pragma once
#include "D2Panel.hpp"

class D2Widget
{
protected:
	// relative to panel
	D2Panel* m_pOwner{};
	int x{}, y{};
	int w{}, h{};

	bool m_bVisible{};

	char identifier[64]{};
	bool bHasIdentifier{};

private:
	D2Widget* m_pNextWidget{};
	D2Widget* m_pNextVisibleWidget{};

public:
	D2Widget(int x, int y, int w, int h);
	D2Widget();
	virtual ~D2Widget();

	bool IsVisible() const { return m_bVisible; }
	void Show() { m_bVisible = true; }
	void Hide() { m_bVisible = false; }

	D2Widget* GetNextWidget() { return m_pNextWidget; }
	D2Widget* GetNextVisible() { return m_pNextVisibleWidget; }

	void AttachIdentifier(const char *szIdentifier);
	void RemoveIdentifier();
	char* GetIdentifier();

	virtual void Draw() = 0;
	virtual void OnWidgetAdded();
	virtual bool HandleTextInput(char* szText);
	virtual bool HandleTextEditing(char* szText, int nStart, int nLength);
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY) = 0;
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY) = 0;
	virtual bool HandleKeyDown(DWORD dwKey);
	virtual bool HandleKeyUp(DWORD dwKey);

	friend class D2Panel;
};
