#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template< bool shouldRenderHotkey>
class GUISimpleToggle : public IGUIElement {

private:
	std::string mToggleText;
	std::shared_ptr<Setting<bool>> mOptionToggle;
	std::vector<std::thread> mUpdateSettingThreads;
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
		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}

		if (ImGui::Checkbox(mToggleText.c_str(), &mOptionToggle->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIToggleButton (" << getName() << ") firing toggle event, new value: " << mOptionToggle->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionToggle]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}
	}

	~GUISimpleToggle()
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

	std::string_view getName() override { return mToggleText; }

};