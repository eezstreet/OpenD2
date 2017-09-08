#pragma once
#include "D2Panel.hpp"

class D2Widget
{
protected:
	// relative to panel
	D2Panel* m_pOwner;
	int x, y;
	int w, h;

	bool m_bVisible;

	char identifier[64];
	bool bHasIdentifier;

private:
	D2Widget* m_pNextWidget;
	D2Widget* m_pNextVisibleWidget;

public:
	D2Widget(int x, int y, int w, int h);
	~D2Widget();
	D2Widget();

	bool IsVisible() { return m_bVisible; }
	void Show() { m_bVisible = true; }
	void Hide() { m_bVisible = false; }

	D2Widget* GetNextWidget() { return m_pNextWidget; }
	D2Widget* GetNextVisible() { return m_pNextVisibleWidget; }

	void AttachIdentifier(char* szIdentifier);
	void RemoveIdentifier();
	char* GetIdentifier();

	virtual void Draw() = 0;
	virtual bool HandleMouseDown(DWORD dwX, DWORD dwY) = 0;
	virtual bool HandleMouseClick(DWORD dwX, DWORD dwY) = 0;

	friend class D2Panel;
};
