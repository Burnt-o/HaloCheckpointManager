#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
class GUISpeedhack : public IGUIElement {

private:
	std::shared_ptr<ActionEvent> mEventToFire;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	gsl::not_null<std::shared_ptr<SettingsStateAndEvents>> mSettings;
public:

	GUISpeedhack(GameState implGame, std::optional<HotkeysEnum> hotkey, std::shared_ptr<SettingsStateAndEvents> settings)
		: IGUIElement(implGame, hotkey), mSettings(settings)
	{

		PLOG_VERBOSE << "Constructing GUISpeedhack, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		hotkeyRenderer.renderHotkey(mHotkey);
		ImGui::SameLine();
		if (ImGui::Checkbox("Speedhack", &mSettings->speedhackToggle->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUISpeedhack being toggled to " << mSettings->speedhackToggle->GetValueDisplay();
			mSettings->speedhackToggle->UpdateValueWithInput();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputDouble("##settingForSpeedhack", &mSettings->speedhackSetting->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUISpeedhack speedsetting being updated to " << mSettings->speedhackSetting->GetValueDisplay();
			mSettings->speedhackSetting->UpdateValueWithInput();

		}
	}

	std::string_view getName() override { return "Speedhack"; }

};