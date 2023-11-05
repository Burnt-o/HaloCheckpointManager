#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"



class GUIHotkeyOnly : public IGUIElement {

private:
	std::string mText;
public:


	GUIHotkeyOnly(GameState implGame, ToolTipCollection tooltip, std::optional<HotkeysEnum> hotkey, std::string buttonText)
		: IGUIElement(implGame, hotkey, tooltip), mText(buttonText)
	{
		if (mText.empty()) throw HCMInitException("Cannot have empty button text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIHotkeyOnly, name: " << getName();
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		
		hotkeyRenderer.renderHotkey(mHotkey);
		DEBUG_GUI_HEIGHT;
		ImGui::SameLine();
		ImGui::Text(mText.c_str());
		renderTooltip();
	}

	std::string_view getName() override { return mText; }

};