#ifdef _DEBUG
#include "../Common/D2Common.hpp"
#include "D2Client.hpp"
namespace Debug
{
	int currentLevel = 1; // rogue encampment
	IRenderObject* text = nullptr;

	void UnloadCurrentWorld()
	{

	}

	void LoadWorld()
	{
		char levelName[128];
		char16_t unicodeLevelName[128];
		int seed = rand();
		UnloadCurrentWorld();

		if (currentLevel >= sgptDataTables->nLevelsTxtRecordCount)
		{
			currentLevel = 1;
		}

		D2Common_ConstructSingleLevel(currentLevel, seed);

		if (text == nullptr)
		{
			text = engine->renderer->AllocateObject(0);
			text->AttachFontResource(cl.font16);
			text->SetTextColor(TextColor_Gold);
			text->SetDrawCoords(10, 10, 0, 0);
		}
		
		D2Lib::strncpyz(levelName, sgptDataTables->pLevelsTxt[currentLevel].szLevelName, 128);
		D2Lib::qmbtowc(unicodeLevelName, 128, levelName);
		text->SetText(unicodeLevelName);
	}

	void DrawWorld()
	{
		static float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Blank out the background
		engine->renderer->Clear();
		engine->renderer->DrawRectangle(0, 0, 800, 600, 0, nullptr, black);

		if (text != nullptr)
		{
			text->Draw();
		}
	}

	bool HandleKeyInput(int keyNum)
	{
		if (keyNum == B_UPARROW)
		{
			if (currentLevel <= 1)
			{
				return true;
			}
			currentLevel--;
			LoadWorld();
			return true;
		}
		else if (keyNum == B_DOWNARROW)
		{
			if (currentLevel >= sgptDataTables->nLevelsTxtRecordCount - 1)
			{
				return true;
			}
			currentLevel++;
			LoadWorld();
			return true;
		}
		return false;
	}
}
#endif