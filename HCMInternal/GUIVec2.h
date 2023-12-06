#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template< bool shouldRenderVertically, bool showTypeLabelWhenVertical, int decimalPrecision>
class GUIVec2 : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<Setting<SimpleMath::Vector2>> mOptionVec2Weak;
	std::vector<std::thread> mUpdateSettingThreads;


	std::string mxLabel;
	std::string myLabel;
	std::string decimalPrecisionFormatter = std::format("%.{}f", decimalPrecision);
public:


	GUIVec2(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<Setting<SimpleMath::Vector2>> optionVec2, std::string xLabel = "x", std::string yLabel = "y")
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionVec2Weak(optionVec2), mxLabel(xLabel), myLabel(yLabel)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty vec2 label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIVec2, name: " << getName();
		PLOG_DEBUG << "mOptionVec2Weak.getOptionName: " << std::hex << mOptionVec2Weak.lock()->getOptionName();

		this->currentHeight = shouldRenderVertically ? (showTypeLabelWhenVertical ? GUIFrameHeightWithSpacing * 3 : GUIFrameHeightWithSpacing * 2) : GUIFrameHeightWithSpacing;

	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionVec2 = mOptionVec2Weak.lock();
		if (!mOptionVec2)
		{
			PLOG_ERROR << "bad mOptionVec2 weakptr when rendering " << getName();
			return;
		}

		if constexpr (shouldRenderVertically)
		{


			ImGui::BeginChild(mLabelText.c_str(), {0, currentHeight - GUISpacing});

			if constexpr (showTypeLabelWhenVertical)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(std::format("{}", mLabelText).c_str());
				renderTooltip();
			}

			// 2 float boxes each seperately bound to x, y of mOptionVec2. Each will render on a seperate horizontal line.
			ImGui::SetNextItemWidth(100);
			if (ImGui::InputFloat(std::format("{}##{}", mxLabel, mLabelText).c_str(), &mOptionVec2->GetValueDisplay().x, 0, 0, decimalPrecisionFormatter.c_str()))
			{
				PLOG_VERBOSE << "GUIVec2 (" << getName() << ") firing toggle event, new value: " << mOptionVec2->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec2]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();

			ImGui::SetNextItemWidth(100);
			if (ImGui::InputFloat(std::format("{}##{}", myLabel, mLabelText).c_str(), &mOptionVec2->GetValueDisplay().y, 0, 0, decimalPrecisionFormatter.c_str()))
			{
				PLOG_VERBOSE << "GUIVec2 (" << getName() << ") firing toggle event, new value: " << mOptionVec2->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec2]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();

			ImGui::EndChild();
		}
		else
		{
			ImGui::SetNextItemWidth(200);
			if (ImGui::InputFloat2(mLabelText.c_str(), &mOptionVec2->GetValueDisplay().x, decimalPrecisionFormatter.c_str())) // can bind InputFloat2 directly to Vec2.x for the whole thing
			{
				PLOG_VERBOSE << "GUIVec2 (" << getName() << ") firing toggle event, new value: " << mOptionVec2->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec2]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();
		}
		DEBUG_GUI_HEIGHT;

	}

	~GUIVec2()
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