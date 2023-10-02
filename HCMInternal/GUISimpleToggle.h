#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
class GUISimpleToggle : public IGUIElement {

private:
	std::string mToggleText;
	std::shared_ptr<Setting<bool>> mOptionToggle;
public:


	GUISimpleToggle(GameState implGame,  std::optional<HotkeysEnum> hotkey, std::string toggleText, std::shared_ptr<Setting<bool>> optionToggle)
		: IGUIElement(implGame, hotkey), mToggleText(toggleText), mOptionToggle(optionToggle)
	{
		if (mToggleText.empty()) throw HCMInitException("Cannot have empty toggle text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUISimpleToggle, name: " << getName();
		PLOG_DEBUG << "mOptionToggle.getOptionName: " << std::hex << mOptionToggle->getOptionName();
		this->currentHeight = 20;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		hotkeyRenderer.renderHotkey(mHotkey);
		ImGui::SameLine();
		if (ImGui::Checkbox(mToggleText.c_str(), &mOptionToggle->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIToggleButton (" << getName() << ") firing toggle event, new value: " << mOptionToggle->GetValueDisplay();
			mOptionToggle->UpdateValueWithInput();
		}
	}

	std::string_view getName() override { return mToggleText; }

};