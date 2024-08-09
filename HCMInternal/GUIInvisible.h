#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

// Does nothing.
// Used if you just need to link a GUIElementEnum to Required service 
class GUIInvisible : public IGUIElement {

private:
	std::string mName;
public:


	GUIInvisible(GameState implGame, ToolTipCollection tooltip, std::string name)
		: IGUIElement(implGame, std::nullopt, tooltip), mName(name)
	{
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
	}



	std::string_view getName() override { return mName; }

};