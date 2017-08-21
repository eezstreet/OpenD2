#define WIN32_LEAN_AND_MEAN
#include "Diablo2.hpp"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* szCmdLine, int nShowCmd)
{
	return InitGame();
}