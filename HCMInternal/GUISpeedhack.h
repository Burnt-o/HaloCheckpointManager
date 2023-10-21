#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
class GUISpeedhack : public IGUIElement {

private:
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
	std::vector<std::thread> mUpdateSettingThreads;
public:

	GUISpeedhack(GameState implGame, std::optional<HotkeysEnum> hotkey, std::shared_ptr<SettingsStateAndEvents> settings)
		: IGUIElement(implGame, hotkey), mSettingsWeak(settings)
	{

		PLOG_VERBOSE << "Constructing GUISpeedhack, name: " << getName();
		this->currentHeight = 20;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{

		auto mSettings = mSettingsWeak.lock();
		if (!mSettings)
		{
			PLOG_ERROR << "bad mSettings weakptr when rendering " << getName();
			return;
		}


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
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mSettings->speedhackSetting]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();

		}
	}

	~GUISpeedhack()
	{
		for (auto& thread : mUpdateSettingThreads)
		{
			if (thread.joinable())
			{
				PLOG_DEBUG << getName() << " joining mUpdateSettingThread";
				thread.join();
				PLOG_DEBUG << getName() << " mUpdateSettingThread finished";
			}
		}
	}

	std::string_view getName() override { return "Speedhack"; }

};