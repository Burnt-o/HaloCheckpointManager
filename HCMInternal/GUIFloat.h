#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

class GUIFloat : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<Setting<float>> mOptionFloatWeak;
	std::vector<std::thread> mUpdateSettingThreads;
public:


	GUIFloat(GameState implGame, ToolTipCollection tooltip,  std::string labelText, std::shared_ptr<Setting<float>> optionFloat)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionFloatWeak(optionFloat)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIFloat, name: " << getName();
		PLOG_DEBUG << "GUIFloat.getOptionName: " << std::hex << mOptionFloatWeak.lock()->getOptionName();

		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		

		auto mOptionFloat = mOptionFloatWeak.lock();
		if (!mOptionFloat)
		{
			PLOG_ERROR << "bad mOptionFloat weakptr when rendering " << getName();
			return;
		}


		if (ImGui::InputFloat(mLabelText.c_str(), &mOptionFloat->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIFloat (" << getName() << ") firing toggle event, new value: " << mOptionFloat->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionFloat]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		

	}

	~GUIFloat()
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