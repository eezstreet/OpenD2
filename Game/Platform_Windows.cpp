#define WIN32_LEAN_AND_MEAN
#include "Diablo2.hpp"
#include <Windows.h>
#include <stdlib.h>

#define D2REGISTRY_BETA_KEY	"SOFTWARE\\Blizzard Entertainment\\Diablo II Beta"
#define D2REGISTRY_KEY		"SOFTWARE\\Blizzard Entertainment\\Diablo II"

#define REGISTRY_KEY_SIZE	16384

/*
 *	Global variables
 */

static const char* gszInterfaces[D2I_MAX] = {
	nullptr,
	"D2Client.dll",
	"D2Server.dll",
	"D2Multi.dll",
	"D2Launch.dll",
};

run_t gpfModules[D2I_MAX] = { nullptr, nullptr, nullptr, nullptr, nullptr };

/*
 *	Copy registry keys (and delete them) from the Diablo II beta to the retail game.
 *	This step is done before anything else.
 *	Never really gets used anymore, but since we're in the business of replicating base game behavior...
 */
void Sys_CopyBetaRegistryKeys()
{
	HKEY betakey;
	HKEY key;
	int i;
	DWORD type;
	BYTE data;
	DWORD cbdata;
	LSTATUS status;
	char keybuffer[REGISTRY_KEY_SIZE]{ 0 };
	DWORD keybufferSize = REGISTRY_KEY_SIZE;

	if (!RegOpenKeyA(HKEY_LOCAL_MACHINE, D2REGISTRY_BETA_KEY, &betakey))
	{	// We had the Diablo II beta installed. Copy the keys and delete the old ones.
		RegCreateKeyA(HKEY_LOCAL_MACHINE, D2REGISTRY_KEY, &key);

		do
		{
			status = RegEnumValueA(betakey, i, keybuffer, &keybufferSize, 0, &type, &data, &cbdata);
		} while (status == 0 && RegSetValueExA(key, keybuffer, 0, type, &data, cbdata));

		RegCloseKey(betakey);
		RegCloseKey(key);
		RegDeleteKeyA(HKEY_LOCAL_MACHINE, D2REGISTRY_BETA_KEY);
	}
}

/*
 *	Copy registry keys from the retail game and make them available in D2.ini
 *	This step has to be done after we've initialized the file system
 */
void Sys_CopySettings()
{

}

/*
 *	Load all of the modules
 *	@author Necrolis (modified by eezstreet)
 */
void Sys_InitModules()
{
	for (int i = 1; i < 5; i++)
	{
		HMODULE hLib = LoadLibrary(gszInterfaces[i]);
		if (hLib)
		{
			interface_t pfInterface = (interface_t)GetProcAddress(hLib, "QueryInterface");
			if (pfInterface != nullptr)
			{
				gpfModules[i] = *pfInterface();
			}
		}
	}
}

/*
 *	The main entrypoint of the program (on Windows)
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* szCmdLine, int nShowCmd)
{
	Sys_CopyBetaRegistryKeys();

	return InitGame(__argc, __argv, (DWORD)hInstance);
}