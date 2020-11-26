#ifdef _DEBUG
#include "../Common/D2Common.hpp"
namespace Debug
{
	int currentLevel = 1; // rogue encampment

	void UnloadCurrentWorld()
	{

	}

	void LoadWorld()
	{
		UnloadCurrentWorld();
	}
}
#endif