#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUISimpleToggle : public GUIElementBase {

private:
	std::string mToggleText;
	std::shared_ptr<Option<bool>> mOptionToggle;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	virtual std::string_view getHotkeyButtonLabelId() override { return mHotkeyButtonLabelId; }
public:


	GUISimpleToggle(GameState implGame, std::vector<std::map<GameState, std::shared_ptr<CheatBase>>> requiredServices, std::optional<std::shared_ptr<Hotkey>> hotkey, std::string toggleText, std::shared_ptr<Option<bool>> optionToggle)
		: GUIElementBase(implGame, requiredServices, hotkey), mToggleText(toggleText), mOptionToggle(optionToggle)
	{
		if (mToggleText.empty()) throw HCMInitException("Cannot have empty toggle text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUISimpleToggle, name: " << getName();
		PLOG_DEBUG << "mOptionToggle.getOptionName: " << std::hex << mOptionToggle.get()->getOptionName();
		this->currentHeight = 20;
	}

	void render() override
	{
		renderHotkey();
		ImGui::SameLine();
		if (ImGui::Checkbox(mToggleText.c_str(), &mOptionToggle.get()->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIToggleButton (" << getName() << ") firing toggle event, new value: " << mOptionToggle.get()->GetValueDisplay();
			mOptionToggle.get()->UpdateValueWithInput();
		}
	}

	std::string_view getName() override { return mToggleText; }

};