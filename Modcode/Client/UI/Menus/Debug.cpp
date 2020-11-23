#include "Debug.hpp"

namespace D2Menus
{
	Debug::Debug() : D2Menu()
	{
		IGraphicsReference* background = engine->graphics->CreateReference(
			"data\\global\\ui\\FrontEnd\\gameselectscreenEXP.dc6",
			UsagePolicy_Permanent
		);

		backgroundObject = engine->renderer->AllocateObject(0);
		backgroundObject->AttachCompositeTextureResource(background, 0, -1);
		backgroundObject->SetDrawCoords(0, 0, 800, 600);
		backgroundObject->SetPalshift(0);

		pDebugPanel = new D2Panels::Debug();
		AddPanel(pDebugPanel);
	}

	Debug::~Debug()
	{
		delete pDebugPanel;
	}

	void Debug::Draw()
	{
		backgroundObject->Draw();

		DrawAllPanels();
	}
}