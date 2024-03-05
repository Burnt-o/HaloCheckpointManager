#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

class GUIInputInt : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<BinarySetting<int>> mOptionIntWeak;
	std::vector<std::thread> mUpdateSettingThreads;
public:


	GUIInputInt(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<BinarySetting<int>> optionInt)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionIntWeak(optionInt)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIInputInt, name: " << getName();
		PLOG_DEBUG << "GUIInputInt.getOptionName: " << std::hex << mOptionIntWeak.lock()->getOptionName();

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

		ImGui::SetNextItemWidth(150);
		if (ImGui::InputInt(mLabelText.c_str(), &mOptionInt->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIInputInt (" << getName() << ") firing toggle event, new value: " << mOptionInt->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionInt]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;


	}

	~GUIInputInt()
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

	std::string_view getName() override { return mLabelText; }

};