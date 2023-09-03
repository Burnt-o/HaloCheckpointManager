#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUISpeedhack : public GUIElementBase {

private:
	std::shared_ptr<ActionEvent> mEventToFire;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	virtual std::string_view getHotkeyButtonLabelId() override { return mHotkeyButtonLabelId; }
public:

	GUISpeedhack(GameState implGame, std::vector<std::map<GameState, std::shared_ptr<CheatBase>>> requiredServices, std::optional<std::shared_ptr<Hotkey>> hotkey)
		: GUIElementBase(implGame, requiredServices, hotkey)
	{

		PLOG_VERBOSE << "Constructing GUISpeedhack, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render() override
	{
		renderHotkey();
		ImGui::SameLine();
		if (ImGui::Checkbox("Speedhack", &OptionsState::speedhackToggle.get()->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUISpeedhack being toggled to " << OptionsState::speedhackToggle.get()->GetValueDisplay();
			OptionsState::speedhackToggle.get()->UpdateValueWithInput();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputDouble("##settingForSpeedhack", &OptionsState::speedhackSetting.get()->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUISpeedhack speedsetting being updated to " << OptionsState::speedhackSetting.get()->GetValueDisplay();
			OptionsState::speedhackSetting.get()->UpdateValueWithInput();

		}
	}

	std::string_view getName() override { return "Speedhack"; }

};