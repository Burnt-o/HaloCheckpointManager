#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"


// used by SwitchBSP
// hotkey fires button event. subscribing services use int as arg however they like
template< bool shouldRenderHotkey>
class GUIButtonAndInt : public IGUIElement {

private:
	std::string mButtonText;
	std::string mInputLabel;

	std::weak_ptr<ActionEvent> mEventToFireWeak;
	std::weak_ptr<Setting<int>> mOptionIntWeak;
	std::vector<std::thread> mUpdateSettingThreads;
public:

	GUIButtonAndInt(GameState implGame, ToolTipCollection tooltip, std::optional<RebindableHotkeyEnum> hotkey, std::string buttonText, std::string inputLabel, std::shared_ptr<Setting<int>> optionInt, std::shared_ptr<ActionEvent> eventToFire)
		: IGUIElement(implGame, hotkey, tooltip), mOptionIntWeak(optionInt), mEventToFireWeak(eventToFire), mInputLabel(inputLabel), mButtonText(buttonText)
	{
		PLOG_VERBOSE << "Constructing GUIButtonAndInt, name: " << getName();
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionInt = mOptionIntWeak.lock();
		if (!mOptionInt)
		{
			PLOG_ERROR << "bad mOptionInt weakptr when rendering " << getName();
			return;
		}

		auto mEventToFire = mEventToFireWeak.lock();
		if (!mEventToFire)
		{
			PLOG_ERROR << "bad mEventToFire weakptr when rendering " << getName();
			return;
		}


		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}

		if (ImGui::Button(mButtonText.c_str()))
		{
			PLOG_VERBOSE << "GUIButtonAndInt (" << getName() << ") firing button event";
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([mEvent = mEventToFire]() {mEvent->operator()(); }));
			newThread.detach();
		}
		renderTooltip();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt(std::format("{}##{}", mInputLabel, mButtonText).c_str(), &mOptionInt->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIButtonAndInt (" << getName() << ") firing int update event, new value: " << mOptionInt->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionInt]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;



	}

	~GUIButtonAndInt()
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

	std::string_view getName() override {
		return mButtonText;
	}

};