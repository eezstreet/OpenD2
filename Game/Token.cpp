#include "Diablo2.hpp"

////////////////////////////////////////////////////////////
//
//	Token Loading

// Maps a token type to a string
static char* gszTokenType[TOKEN_MAX] =
{
	"chars", "monsters", "objects"
};

// Maps a plrmode to a string
static char* gszPlrMode[PLRMODE_MAX] =
{
	"dt", "nu", "wl", "rn", 
	"gh", "tn", "tw", "a1", 
	"a2", "bl", "sc", "th", 
	"kk", "s1", "s2", "s3", 
	"s4", "dd", "sq", "gh",
};

// Maps a monmode to a string
static char* gszMonMode[MONMODE_MAX] =
{
	"dt", "nu", "wl", "gh",
	"a1", "a2", "bl", "sc",
	"s1", "s2", "s3", "s4",
	"dd", "gh", "xx", "rn",
};

// Maps a objmode to a string
static char* gszObjMode[OBJMODE_MAX] =
{
	"nu", "op", "on", "s1",
	"s2", "s3", "s4", "s5",
};

#define MAX_TOKEN_HASH	2048

struct TokenHash
{
	bool	bRegistered;
	char	tokenName[8];		// Used when hashing the token
	char	baseTokenName[4];	// Used for the base token name, eg, "FA" for Fallen
	char	weaponClass[4];		// Used for the weapon class, eg, "hth" for Hand to Hand
	AnimToken token;
};

static TokenHash gTokenTable[MAX_TOKEN_HASH]{ 0 };
static int gnNumTokensRegistered = 0;

/*
 *	Registers a token.
 *	@author	eezstreet
 */
token_handle TOK_RegisterToken(D2TokenType type, char* tokenName, char* szWeaponClass)
{
	char registerName[8]{ 0 };
	token_handle out;
	int i;

	Log_ErrorAssert(gnNumTokensRegistered < MAX_TOKEN_HASH, INVALID_HANDLE);

	snprintf(registerName, 8, "%s%s", tokenName, szWeaponClass);
	out = D2_strhash(registerName, 0, MAX_TOKEN_HASH);

	while (gTokenTable[out].bRegistered)
	{
		if (!D2_stricmp(registerName, gTokenTable[out].tokenName))
		{
			// Already registered this token/weapon class combination
			return out;
		}
		out++;
		out %= MAX_TOKEN_HASH;
	}

	// Register all of the COFs for this token
	gTokenTable[out].token.tokenType = type;
	switch (type)
	{
		case TOKEN_CHAR:
			for (i = 0; i < PLRMODE_MAX; i++)
			{
				gTokenTable[out].token.plrCof[i] = 
					COF_Register(gszTokenType[type], tokenName, gszPlrMode[i], szWeaponClass);
				if (gTokenTable[out].token.plrCof[i] != INVALID_HANDLE)
				{
					gTokenTable[out].token.dwCOFsPresent |= (1 << i);
				}
			}
			break;
		case TOKEN_MONSTER:
			for (i = 0; i < MONMODE_MAX; i++)
			{
				gTokenTable[out].token.monCof[i] =
					COF_Register(gszTokenType[type], tokenName, gszMonMode[i], szWeaponClass);
				if (gTokenTable[out].token.monCof[i] != INVALID_HANDLE)
				{
					gTokenTable[out].token.dwCOFsPresent |= (1 << i);
				}
			}
			break;
		case TOKEN_OBJECT:
			for (i = 0; i < OBJMODE_MAX; i++)
			{
				gTokenTable[out].token.objCof[i] =
					COF_Register(gszTokenType[type], tokenName, gszObjMode[i], szWeaponClass);
				if (gTokenTable[out].token.objCof[i] != INVALID_HANDLE)
				{
					gTokenTable[out].token.dwCOFsPresent |= (1 << i);
				}
			}
			break;
	}

	D2_strncpyz(gTokenTable[out].tokenName, registerName, 8);
	D2_strncpyz(gTokenTable[out].baseTokenName, tokenName, 4);
	D2_strncpyz(gTokenTable[out].weaponClass, szWeaponClass, 4);
	gnNumTokensRegistered++;
	gTokenTable[out].bRegistered = true;
	return out;
}

/*
 *	Deregisters a specific token.
 *	@author	eezstreet
 */
void TOK_DeregisterToken(token_handle token)
{
	if (!gTokenTable[token].bRegistered)
	{
		return;
	}

	// Deregister all of the COFs
	for (int i = 0; i < 32; i++)
	{
		if (gTokenTable[token].token.dwCOFsPresent & (1 << i))
		{
			switch (gTokenTable[token].token.tokenType)
			{
				case TOKEN_CHAR:
					COF_Deregister(gTokenTable[token].token.plrCof[i]);
					break;
				case TOKEN_MONSTER:
					COF_Deregister(gTokenTable[token].token.monCof[i]);
					break;
				case TOKEN_OBJECT:
					COF_Deregister(gTokenTable[token].token.objCof[i]);
					break;
			}
		}
	}
}

/*
 *	Retrieve a pointer to the AnimToken data from the given token handle
 *	@author	eezstreet
 */
AnimToken* TOK_GetAnimData(token_handle token)
{
	if (token == INVALID_HANDLE || token >= MAX_TOKEN_HASH)
	{
		return nullptr;
	}

	if (gTokenTable[token].bRegistered)
	{
		return &gTokenTable[token].token;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////
//
//	Token Instances

#define MAX_TOKEN_INSTANCES	2048

static AnimTokenInstance gTokenInstances[MAX_TOKEN_INSTANCES]{ 0 };
static int gnNumInstancesUsed = 0;

/*
 *	Create a token instance. THESE SHOULD BE MANUALLY CLEANED UP WHEN NOT IN USE!
 *	@author	eezstreet
 */
anim_handle TOK_CreateTokenAnimInstance(token_handle token)
{
	if (gnNumInstancesUsed >= MAX_TOKEN_INSTANCES)
	{
		Log_Print(PRIORITY_MESSAGE, "Token instances exceeded");
	}

	// Just pick one at random
	anim_handle handle = rand() * MAX_TOKEN_INSTANCES;

	while (gTokenInstances[handle].bInUse)
	{
		handle++;
		handle %= MAX_TOKEN_INSTANCES;
	}

	memset(&gTokenInstances[handle], 0, sizeof(AnimTokenInstance));
	gTokenInstances[handle].bInUse = true;
	gTokenInstances[handle].currentHandle = token;
	return handle;
}

/*
 *	Swap a token instance's token with another token.
 *	An example of this would be switching into the Werebear token when that skill is activated.
 *	Or, more frequently, when your weapon is changed, since each weapon class uses a new token_handle
 *	@author	eezstreet
 */
void TOK_SwapTokenAnimToken(anim_handle handle, token_handle newhandle)
{
	if (handle == INVALID_HANDLE || newhandle == INVALID_HANDLE)
	{
		return;
	}

	// The components might not be altered when we switch tokens but the frame *will*
	gTokenInstances[handle].currentHandle = newhandle;
	gTokenInstances[handle].currentFrame = 0;

	// FIXME: if our token class changes (ie, we change from monster to player or vice versa), our mode will be wrong!
	// This is because each token type has its own mappings for these modes!
}

/*
 *	Kill off a token instance.
 *	@author	eezstreet
 */
void TOK_DestroyTokenInstance(anim_handle handle)
{
	if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
	{
		return;
	}

	gTokenInstances[handle].bInUse = false;
}

/*
 *	Set a token instance's component
 *	@author	eezstreet
 */
void TOK_SetTokenInstanceComponent(anim_handle handle, int componentNum, char* componentName)
{
	if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
	{
		return;
	}

	if (!componentName)
	{
		return;
	}

	D2_strncpyz(gTokenInstances[handle].components[componentNum], componentName, 4);
}

/*
 *	Get a token instance's component at a specific component number
 *	@author	eezstreet
 */
char* TOK_GetTokenInstanceComponent(anim_handle handle, int component)
{
	if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
	{
		return nullptr;
	}

	return gTokenInstances[handle].components[component];
}

/*
 *	Set a token instance's frame
 *	@author	eezstreet
 */
void TOK_SetTokenInstanceFrame(anim_handle handle, int frameNum)
{
	if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
	{
		return;
	}

	gTokenInstances[handle].currentFrame = frameNum;
}

/*
 *	Get a token instance's frame
 *	@author	eezstreet
 */
int TOK_GetTokenInstanceFrame(anim_handle handle)
{
	if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
	{
		return 0;
	}

	return gTokenInstances[handle].currentFrame;
}

/*
 *	Get a token instance's weapon class
 *	@author	eezstreet
 */
char* TOK_GetTokenInstanceWeaponClass(anim_handle handle)
{
	if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
	{
		// The token instance is invalid
		return nullptr;
	}

	if (gTokenInstances[handle].currentHandle == INVALID_HANDLE ||
		gTokenInstances[handle].currentHandle >= MAX_TOKEN_HASH)
	{
		// The token that this instance uses is wrong
		return nullptr;
	}

	return gTokenTable[gTokenInstances[handle].currentHandle].weaponClass;
}