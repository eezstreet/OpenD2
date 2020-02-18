#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	Tokens.
 *	Tokens are a collection of COFs.
 *	@author	eezstreet
 */
struct AnimToken
{
	D2TokenType	tokenType;
	DWORD		dwCOFsPresent;

	union
	{
		cof_handle	plrCof[PLRMODE_MAX];
		cof_handle	monCof[MONMODE_MAX];
		cof_handle	objCof[OBJMODE_MAX];
	};
};

/*
 *	Token instances.
 *	When you want a monster, object or character to be shown on the screen, you should use a token instance.
 *	A token instance is a single rendering of one of those objects and is animated separately.
 *	(Missiles and overlays do not use token instances. Instead, they are rendered as raw DCCs.)
 *	@author	eezstreet
 */
struct AnimTokenInstance
{
	token_handle	currentHandle;
	int				tokenType;
	int				currentMode;
	int				currentFrame;
	int				currentDirection;
	char			components[COMP_MAX][4];
	bool			bInUse;
	bool			bActive;

	anim_handle		componentAnims[XXXMODE_MAX][COMP_MAX];
};

// Token.cpp - Should maybe move this to gamecode?
namespace Token
{
	token_handle RegisterToken(D2TokenType type, char* tokenName, char* szWeaponClass);
	void DeregisterToken(token_handle token);
	AnimToken* GetAnimData(token_handle token);
	cof_handle GetCOFData(token_handle token, int mode);
}

namespace TokenInstance
{
	anim_handle CreateTokenAnimInstance(token_handle token);
	void SwapTokenAnimToken(anim_handle handle, token_handle newhandle);
	void DestroyTokenInstance(anim_handle handle);
	void SetTokenInstanceComponent(anim_handle handle, int componentNum, char* componentName);
	char* GetTokenInstanceComponent(anim_handle handle, int component);
	void SetTokenInstanceFrame(anim_handle handle, int frameNum);
	int GetTokenInstanceFrame(anim_handle handle);
	char* GetTokenInstanceWeaponClass(anim_handle handle);
	void SetInstanceActive(anim_handle handle, bool bNewActive);
	AnimTokenInstance* GetTokenInstanceData(anim_handle handle);
	void SetTokenInstanceMode(anim_handle handle, int modeNum);
	void SetTokenInstanceDirection(anim_handle handle, int dirNum);
}