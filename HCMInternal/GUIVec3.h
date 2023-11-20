#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template< bool shouldRenderVertically, bool AbsoluteOrRelative, int decimalPrecision>
class GUIVec3 : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<Setting<SimpleMath::Vector3>> mOptionVec3Weak;
	std::vector<std::thread> mUpdateSettingThreads;

	std::string xLabel;
	std::string yLabel;
	std::string zLabel;
	std::string type;

	std::string decimalPrecisionFormatter = std::format("%.{}f", decimalPrecision);
public:


	GUIVec3(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<Setting<SimpleMath::Vector3>> optionVec3)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionVec3Weak(optionVec3)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty vec3 label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIVec3, name: " << getName();
		PLOG_DEBUG << "mOptionVec3Weak.getOptionName: " << std::hex << mOptionVec3Weak.lock()->getOptionName();

		this->currentHeight = shouldRenderVertically ? GUIFrameHeightWithSpacing * 4 : GUIFrameHeightWithSpacing;



		if constexpr (AbsoluteOrRelative)
		{
			xLabel = "x";
			yLabel = "y";
			zLabel = "z";
			type = "Absolute";
		}
		else
		{
			xLabel = "Forward";
			yLabel = "Right";
			zLabel = "Up";
			type = "Relative";
		}

	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		

		auto mOptionVec3 = mOptionVec3Weak.lock();
		if (!mOptionVec3)
		{
			PLOG_ERROR << "bad mOptionVec3 weakptr when rendering " << getName();
			return;
		}

		if constexpr (shouldRenderVertically)
		{


			ImGui::BeginChild(std::format("##", mLabelText).c_str(), { 0, currentHeight - GUISpacing });
			ImGui::AlignTextToFramePadding();
			ImGui::Text(std::format("{} {}", type, mLabelText).c_str());
			renderTooltip();
			// 3 float boxes each seperately bound to x, y, z of mOptionVec3. Each will render on a seperate horizontal line.
			ImGui::SetNextItemWidth(100);
			if (ImGui::InputFloat(std::format("{}##{}", xLabel, mLabelText).c_str(), &mOptionVec3->GetValueDisplay().x, 0, 0, decimalPrecisionFormatter.c_str()))
			{
				PLOG_VERBOSE << "GUIVec3 (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();

			ImGui::SetNextItemWidth(100);
			if (ImGui::InputFloat(std::format("{}##{}", yLabel, mLabelText).c_str(), &mOptionVec3->GetValueDisplay().y, 0, 0, decimalPrecisionFormatter.c_str()))
			{
				PLOG_VERBOSE << "GUIVec3 (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();

			ImGui::SetNextItemWidth(100);
			if (ImGui::InputFloat(std::format("{}##{}", zLabel, mLabelText).c_str(), &mOptionVec3->GetValueDisplay().z, 0, 0, decimalPrecisionFormatter.c_str()))
			{
				PLOG_VERBOSE << "GUIVec3 (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();
			ImGui::EndChild();
		}
		else
		{
			if (ImGui::InputFloat3(mLabelText.c_str(), &mOptionVec3->GetValueDisplay().x), decimalPrecisionFormatter.c_str()) // can bind InputFloat3 directly to Vec3.x for the whole thing
			{
				PLOG_VERBOSE << "GUIVec3 (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();
		}
		DEBUG_GUI_HEIGHT;

	}

	~GUIVec3()
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