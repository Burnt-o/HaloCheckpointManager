#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template<bool showInputs>
class GUIColourPicker : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<BinarySetting<SimpleMath::Vector3>> mOptionVec3Weak;
	std::vector<std::thread> mUpdateSettingThreads;
	const ImGuiColorEditFlags editFlags = showInputs ?
		(ImGuiColorEditFlags_Float  | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoDragDrop) :
		(ImGuiColorEditFlags_Float  | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoInputs);
public:


	GUIColourPicker(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<BinarySetting<SimpleMath::Vector3>> optionVec3)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionVec3Weak(optionVec3)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty Vec3 label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIColourPicker, name: " << getName();
		PLOG_DEBUG << "mOptionVec3Weak.getOptionName: " << std::hex << mOptionVec3Weak.lock()->getOptionName();

		this->currentHeight = GUIFrameHeightWithSpacing;




	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionVec3 = mOptionVec3Weak.lock();
		if (!mOptionVec3)
		{
			PLOG_ERROR << "bad mOptionVec3 weakptr when rendering " << getName();
			return;
		}


		if (ImGui::ColorEdit3(mLabelText.c_str(), &mOptionVec3->GetValueDisplay().x, editFlags))
		{
			PLOG_VERBOSE << "GUIColourPicker (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}
		renderTooltip();

		DEBUG_GUI_HEIGHT;

	}

	~GUIColourPicker()
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