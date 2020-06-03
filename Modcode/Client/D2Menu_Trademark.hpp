#pragma once
#include "D2Menu.hpp"

class D2Menu_Trademark : public D2Menu
{
private:
	tex_handle backgroundTexture;
	anim_handle flameLeftAnim;
	anim_handle flameRightAnim;
	anim_handle blackLeftAnim;
	anim_handle blackRightAnim;

	char16_t* szCopyrightText;
	char16_t* szAllRightsReservedText;
	DWORD dwStartTicks;

public:
	D2Menu_Trademark();
	~D2Menu_Trademark() override;

	bool HandleMouseClicked(DWORD dwX, DWORD dwY) override;
	bool WaitingSignal() override;
	static void TrademarkSignal(D2Panel* pCallingPanel, D2Widget* pCallingWidget);

	void Draw() override;
};
