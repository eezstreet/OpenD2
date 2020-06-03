#pragma once
#include "../Shared/D2Shared.hpp"

// INI.cpp
namespace INI
{
	void WriteConfig(fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig);
	void ReadConfig(const fs_handle* f, D2GameConfigStrc* pGameConfig, OpenD2ConfigStrc* pOpenConfig);
}
