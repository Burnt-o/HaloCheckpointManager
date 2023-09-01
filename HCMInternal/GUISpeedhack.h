#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUISpeedhack : public GUIElementBase {

private:
	std::shared_ptr<ActionEvent> mEventToFire;

public:

	GUISpeedhack(GameState implGame, std::vector<std::map<GameState, std::shared_ptr<CheatBase>>> requiredServices)
		: GUIElementBase(implGame, requiredServices)
	{

		PLOG_VERBOSE << "Constructing GUISpeedhack, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render() override
	{
		
		if (ImGui::Checkbox("Speedhack", &OptionsState::speedhackToggle.get()->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUISpeedhack being toggled to " << OptionsState::speedhackToggle.get()->GetValueDisplay();
			OptionsState::speedhackToggle.get()->UpdateValueWithInput();
		}
		ImGui::SameLine();
		if (ImGui::InputDouble("##settingForSpeedhack", &OptionsState::speedhackSetting.get()->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUISpeedhack speedsetting being updated to " << OptionsState::speedhackSetting.get()->GetValueDisplay();
			OptionsState::speedhackSetting.get()->UpdateValueWithInput();

		}
	}

	std::string_view getName() override { return "Speedhack"; }

};