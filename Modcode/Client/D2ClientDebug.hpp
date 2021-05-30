#pragma once
#ifdef _DEBUG
namespace Debug
{
	void LoadWorld();
	void DrawWorld();

	bool HandleKeyInput(int keyNum);
}
#endif
