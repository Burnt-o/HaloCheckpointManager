#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
#include <bitset>

// used by SwitchBSP
// hotkey fires button event. subscribing services use int as arg however they like
template< bool shouldRenderHotkey>
class GUIButtonAndBinaryInt : public IGUIElement {

private:
	std::string mButtonText;
	std::string mInputLabel;

	std::weak_ptr<ActionEvent> mEventToFireWeak;
	std::weak_ptr<BinarySetting<int>> mOptionBinaryIntWeak;
	std::vector<std::thread> mUpdateSettingThreads;
public:

	GUIButtonAndBinaryInt(GameState implGame, ToolTipCollection tooltip, std::optional<RebindableHotkeyEnum> hotkey, std::string buttonText, std::string inputLabel, std::shared_ptr<BinarySetting<int>> optionInt, std::shared_ptr<ActionEvent> eventToFire)
		: IGUIElement(implGame, hotkey, tooltip), mOptionBinaryIntWeak(optionInt), mEventToFireWeak(eventToFire), mInputLabel(inputLabel), mButtonText(buttonText)
	{
		PLOG_VERBOSE << "Constructing GUIButtonAndBinaryInt, name: " << getName();
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionBinaryInt = mOptionBinaryIntWeak.lock();
		if (!mOptionBinaryInt)
		{
			PLOG_ERROR << "bad mOptionBinaryInt weakptr when rendering " << getName();
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
			PLOG_VERBOSE << "GUIButtonAndBinaryInt (" << getName() << ") firing button event";
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([mEvent = mEventToFire]() {mEvent->operator()(); }));
			newThread.detach();
		}
		renderTooltip();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);

		std::stringstream ss;
		ss << std::bitset<32>(mOptionBinaryInt->GetValueDisplay());
		std::string binaryString = ss.str() ;

		binaryString.erase(0, binaryString.find_first_not_of('0')); // eliminate leading zeros
		

		if (ImGui::InputText(mInputLabel.c_str(), &binaryString))
		{
			// check that new string only contains 0's or 1's
			if (binaryString.find_first_not_of("01") != std::string::npos)
			{
				PLOG_DEBUG << "bad binary string input: " << binaryString;
				binaryString = ss.str();
			}
			else
			{
				// convert string back to number value and fire update changed event
				try
				{
					auto newNum = std::stoi(binaryString, nullptr, 2);
					PLOG_VERBOSE << "GUIButtonAndBinaryInt (" << getName() << ") firing int update event, new value: " << mOptionBinaryInt->GetValueDisplay();
					mOptionBinaryInt->GetValueDisplay() = newNum;
					auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionBinaryInt]() { optionToggle->UpdateValueWithInput(); }));
					newThread.detach();
				}
				catch (std::invalid_argument ex)
				{
					PLOG_DEBUG << "bad binary string input conversion (invalid_argument): " << binaryString << ", ex: " << ex.what();
					binaryString = ss.str();
				}
				catch (std::out_of_range ex)
				{
					PLOG_DEBUG << "bad binary string input conversion (out_of_range): " << binaryString << ", ex: " << ex.what();
					binaryString = ss.str();
				}
			}
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;



	}

	~GUIButtonAndBinaryInt()
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