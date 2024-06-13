#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template<typename dataT>
class GUILabel : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<UnarySetting<dataT>> mDynamicDataWeak;
public:


	GUILabel(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<UnarySetting<dataT>> dynamicData)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mDynamicDataWeak(dynamicData)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUILabel, name: " << getName();
		PLOG_DEBUG << "GUILabel.getOptionName: " << std::hex << mDynamicDataWeak.lock()->getOptionName();

		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{

		auto mDynamicData = mDynamicDataWeak.lock();
		if (!mDynamicData)
		{
			PLOG_ERROR << "bad mDynamicData weakptr when rendering " << getName();
			return;
		}

		ImGui::Text(std::vformat(mLabelText, std::make_format_args(mDynamicData->GetValue())).c_str());

		renderTooltip();
		DEBUG_GUI_HEIGHT;


	}


	std::string_view getName() override { return mLabelText; }

};