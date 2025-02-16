#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
#include "SliderParam.h"


class GUIInputSubpixel : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<BinarySetting<SubpixelID>> mOptionIntWeak;
	std::vector<std::thread> mUpdateSettingThreads;
public:


	GUIInputSubpixel(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<BinarySetting<SubpixelID>> optionInt)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionIntWeak(optionInt)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIInputSubpixel, name: " << getName();
		PLOG_DEBUG << "GUIInputSubpixel.getOptionName: " << std::hex << mOptionIntWeak.lock()->getOptionName();

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

		bool updateRequired = ImGui::InputInt(mLabelText.c_str(), reinterpret_cast<int*>(&mOptionInt->GetValueDisplay()) );


		if (updateRequired)
		{
			PLOG_VERBOSE << "GUIInputSubpixel (" << getName() << ") firing toggle event, new value: " << mOptionInt->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionInt]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;


	}

	~GUIInputSubpixel()
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