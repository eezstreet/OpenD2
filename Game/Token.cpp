#include "Diablo2.hpp"

////////////////////////////////////////////////////////////
//
//	Token Loading

// Maps a token type to a string
static char* gszTokenType[TOKEN_MAX] =
{
	"chars", "monsters", "objects"
};

// Maps COF layers to strings
extern const char* COFLayerNames[COMP_MAX];

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

namespace Token
{

	/*
	 *	Registers a token.
	 *	@author	eezstreet
	 */
	token_handle RegisterToken(D2TokenType type, char* tokenName, char* szWeaponClass)
	{
		char registerName[8]{ 0 };
		token_handle out;
		int i;

		Log_ErrorAssert(gnNumTokensRegistered < MAX_TOKEN_HASH, INVALID_HANDLE);

		snprintf(registerName, 8, "%s%s", tokenName, szWeaponClass);
		out = D2Lib::strhash(registerName, 0, MAX_TOKEN_HASH);

		while (gTokenTable[out].bRegistered)
		{
			if (!D2Lib::stricmp(registerName, gTokenTable[out].tokenName))
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
					COF::Register(gszTokenType[type], tokenName, gszPlrMode[i], szWeaponClass);
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
					COF::Register(gszTokenType[type], tokenName, gszMonMode[i], szWeaponClass);
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
					COF::Register(gszTokenType[type], tokenName, gszObjMode[i], szWeaponClass);
				if (gTokenTable[out].token.objCof[i] != INVALID_HANDLE)
				{
					gTokenTable[out].token.dwCOFsPresent |= (1 << i);
				}
			}
			break;
		}

		D2Lib::strncpyz(gTokenTable[out].tokenName, registerName, 8);
		D2Lib::strncpyz(gTokenTable[out].baseTokenName, tokenName, 4);
		D2Lib::strncpyz(gTokenTable[out].weaponClass, szWeaponClass, 4);
		gnNumTokensRegistered++;
		gTokenTable[out].bRegistered = true;
		return out;
	}

	/*
	 *	Deregisters a specific token.
	 *	@author	eezstreet
	 */
	void DeregisterToken(token_handle token)
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
					COF::Deregister(gTokenTable[token].token.plrCof[i]);
					break;
				case TOKEN_MONSTER:
					COF::Deregister(gTokenTable[token].token.monCof[i]);
					break;
				case TOKEN_OBJECT:
					COF::Deregister(gTokenTable[token].token.objCof[i]);
					break;
				}
			}
		}
	}

	/*
	 *	Retrieve a pointer to the AnimToken data from the given token handle
	 *	@author	eezstreet
	 */
	AnimToken* GetAnimData(token_handle token)
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

	/*
	 *	Retrieve a COF handle by the provided token handle and mode index.
	 *	@author	eezstreet
	 */
	cof_handle GetCOFData(token_handle token, int mode)
	{
		TokenHash* pHash;

		if (token == INVALID_HANDLE)
		{	// invalid handle passed, invalid handle is what you get back
			return INVALID_HANDLE;
		}

		pHash = &gTokenTable[token];
		if (!pHash->bRegistered)
		{	// the handle we passed was valid, but the thing it pointed to was not
			return INVALID_HANDLE;
		}

		switch (pHash->token.tokenType)
		{
			default:
			case TOKEN_CHAR:
				return pHash->token.plrCof[mode];
			case TOKEN_MONSTER:
				return pHash->token.monCof[mode];
			case TOKEN_OBJECT:
				return pHash->token.objCof[mode];
		}
	}
}

////////////////////////////////////////////////////////////
//
//	Token Instances

#define MAX_TOKEN_INSTANCES	2048

static AnimTokenInstance gTokenInstances[MAX_TOKEN_INSTANCES]{ 0 };
static int gnNumInstancesUsed = 0;

namespace TokenInstance
{

	/*
	 *	Create a token instance. THESE SHOULD BE MANUALLY CLEANED UP WHEN NOT IN USE!
	 *	@author	eezstreet
	 */
	anim_handle CreateTokenAnimInstance(token_handle token)
	{
		if (gnNumInstancesUsed >= MAX_TOKEN_INSTANCES)
		{
			Log::Print(PRIORITY_MESSAGE, "Token instances exceeded");
		}

		// Just pick one at random
		anim_handle handle = rand() * MAX_TOKEN_INSTANCES;
		handle %= MAX_TOKEN_INSTANCES;

		while (gTokenInstances[handle].bInUse)
		{
			handle++;
			handle %= MAX_TOKEN_INSTANCES;
		}

		memset(&gTokenInstances[handle], 0, sizeof(AnimTokenInstance));
		gTokenInstances[handle].bInUse = true;
		gTokenInstances[handle].currentHandle = token;
		gTokenInstances[handle].tokenType = gTokenTable[token].token.tokenType;

		for (int i = 0; i < COMP_MAX; i++)
		{
			for (int j = 0; j < XXXMODE_MAX; j++)
			{
				gTokenInstances[handle].componentAnims[j][i] = INVALID_HANDLE;
			}
			D2Lib::strncpyz(gTokenInstances[handle].components[i], "xxx", 4);
		}
		return handle;
	}

	/*
	 *	Swap a token instance's token with another token.
	 *	An example of this would be switching into the Werebear token when that skill is activated.
	 *	Or, more frequently, when your weapon is changed, since each weapon class uses a new token_handle
	 *	@author	eezstreet
	 */
	void SwapTokenAnimToken(anim_handle handle, token_handle newhandle)
	{
		if (handle == INVALID_HANDLE || newhandle == INVALID_HANDLE)
		{
			return;
		}

		// The components might not be altered when we switch tokens but the frame *will*
		gTokenInstances[handle].currentHandle = newhandle;
		gTokenInstances[handle].currentFrame = 0;
		gTokenInstances[handle].tokenType = gTokenTable[newhandle].token.tokenType;

		// FIXME: if our token class changes (ie, we change from monster to player or vice versa), our mode will be wrong!
		// This is because each token type has its own mappings for these modes!
	}

	/*
	 *	Kill off a token instance.
	 *	@author	eezstreet
	 */
	void DestroyTokenInstance(anim_handle handle)
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
	void SetTokenInstanceComponent(anim_handle handle, int componentNum, char* componentName)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
		{
			return;
		}

		if (!componentName)
		{
			return;
		}

		D2Lib::strncpyz(gTokenInstances[handle].components[componentNum], componentName, 4);
	}

	/*
	 *	Get a token instance's component at a specific component number
	 *	@author	eezstreet
	 */
	char* GetTokenInstanceComponent(anim_handle handle, int component)
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
	void SetTokenInstanceFrame(anim_handle handle, int frameNum)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
		{
			return;
		}

		gTokenInstances[handle].currentFrame = frameNum;
	}

	/*
	 *	Set a token instance's mode
	 *	@author	eezstreet
	 */
	void SetTokenInstanceMode(anim_handle handle, int modeNum)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
		{
			return;
		}

		gTokenInstances[handle].currentMode = modeNum;
	}

	/*
	 *	Set a token instance's direction
	 *	@author	eezstreet
	 */
	void SetTokenInstanceDirection(anim_handle handle, int dirNum)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
		{
			return;
		}

		gTokenInstances[handle].currentDirection = dirNum;
	}

	/*
	 *	Get a token instance's frame
	 *	@author	eezstreet
	 */
	int GetTokenInstanceFrame(anim_handle handle)
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
	char* GetTokenInstanceWeaponClass(anim_handle handle)
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

	/*
	 *	Get the AnimTokenInstance data associated with a token handle
	 *	@author	eezstreet
	 */
	AnimTokenInstance* GetTokenInstanceData(anim_handle handle)
	{
		if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
		{
			return nullptr;
		}

		return &gTokenInstances[handle];
	}

	/*
	 *	When we are ready to draw a token, we should set it as being active.
	 *	The DCC becomes registered and loaded if it is not already.
	 *	@author	eezstreet
	 */
	void SetInstanceActive(anim_handle handle, bool bNewActive)
	{
		AnimTokenInstance* pInstance;
		int i, j;
		D2TokenType tokenType;
		char path[MAX_D2PATH];
		char name[MAX_D2PATH];
		char* tokenName;
		char* tokenClass;
		char** modeNames;
		cof_handle* pCOFs;
		int max = 0;

#ifdef PROFILE_TOKENS
		DWORD dwStartTicks = SDL_GetTicks();
		DWORD dwTicks;
#endif

		if (handle == INVALID_HANDLE || handle >= MAX_TOKEN_INSTANCES)
		{	// not valid, don't do anything
			return;
		}

		pInstance = &gTokenInstances[handle];
		if (!pInstance->bInUse)
		{	// instance is not loaded
			return;
		}

		if (pInstance->currentHandle == INVALID_HANDLE)
		{
			// we need data in the token table and our token instance is using an invalid handle
			return;
		}

		tokenType = gTokenTable[pInstance->currentHandle].token.tokenType;
		tokenName = gTokenTable[pInstance->currentHandle].baseTokenName;
		tokenClass = gTokenTable[pInstance->currentHandle].weaponClass;

		if (bNewActive)
		{
			// Register component DCCs
			switch (tokenType)
			{
			case TOKEN_CHAR:
				max = PLRMODE_MAX;
				modeNames = gszPlrMode;
				pCOFs = gTokenTable[pInstance->currentHandle].token.plrCof;
				break;
			case TOKEN_MONSTER:
				max = MONMODE_MAX;
				modeNames = gszMonMode;
				pCOFs = gTokenTable[pInstance->currentHandle].token.monCof;
				break;
			case TOKEN_OBJECT:
				max = OBJMODE_MAX;
				modeNames = gszObjMode;
				pCOFs = gTokenTable[pInstance->currentHandle].token.objCof;
				break;
			}

			// Load and increment use count of each component's DCC
			// There is a DCC file for each component type (CAP, GHM, LIT, MED, HVY, ...)
			// ...on each component layer (HD, TR, LG, LH, ...)
			// ...for every mode (WL, RN, NU, ...)
			// Example: data/global/CHARS/AM/HD/AMHDCRNA21HS.dcc
			// The COF file on the token acts like glue to hold everything together.
			for (i = 0; i < max; i++)
			{
				for (j = 0; j < COMP_MAX; j++)
				{
					if (!COF::LayerPresent(pCOFs[i], j))
					{	// layer not present in the COF, so it is not drawn
						pInstance->componentAnims[i][j] = INVALID_HANDLE;
						continue;
					}

					snprintf(name, MAX_D2PATH, "%s%s%s%s%s",
						tokenName, COFLayerNames[j], pInstance->components[j], modeNames[i], tokenClass);
					snprintf(path, MAX_D2PATH, "data\\global\\%s\\%s\\%s\\%s.dcc",
						gszTokenType[tokenType], tokenName, COFLayerNames[j], name);
					pInstance->componentAnims[i][j] = DCC::Load(path, name);

					if (pInstance->componentAnims[i][j] != INVALID_HANDLE)
					{
						DCC::IncrementUseCount(pInstance->componentAnims[i][j], 1);
					}
				}
			}
		}
		else
		{
			// Decrement use count of each component's DCC
			// Don't purge it if it goes totally unused though. We only do that in special circumstances.
			for (i = 0; i < max; i++)
			{
				for (j = 0; j < COMP_MAX; j++)
				{
					if (pInstance->componentAnims[i][j] != INVALID_HANDLE)
					{
						DCC::IncrementUseCount(pInstance->componentAnims[i][j], -1);
					}
				}
			}
		}

		pInstance->bActive = bNewActive;

#ifdef PROFILE_TOKENS
		dwTicks = SDL_GetTicks();
		Log_Print(PRIORITY_DEBUG, "Loaded token instance %s (%d) in %d milliseconds\n",
			gTokenTable[pInstance->currentHandle].baseTokenName, pInstance->currentHandle, dwTicks - dwStartTicks);
#endif
	}
}