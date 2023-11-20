#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

class GUIColourPickerAlpha : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<Setting<SimpleMath::Vector4>> mOptionVec4Weak;
	std::vector<std::thread> mUpdateSettingThreads;

public:


	GUIColourPickerAlpha(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<Setting<SimpleMath::Vector4>> optionVec4)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionVec4Weak(optionVec4)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty vec4 label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIColourPickerAlpha, name: " << getName();
		PLOG_DEBUG << "mOptionVec4Weak.getOptionName: " << std::hex << mOptionVec4Weak.lock()->getOptionName();

		this->currentHeight = GUIFrameHeightWithSpacing;




	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionVec4 = mOptionVec4Weak.lock();
		if (!mOptionVec4)
		{
			PLOG_ERROR << "bad mOptionVec4 weakptr when rendering " << getName();
			return;
		}


			if (ImGui::ColorEdit4(mLabelText.c_str(), &mOptionVec4->GetValueDisplay().x, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoDragDrop))
			{
				PLOG_VERBOSE << "GUIColourPickerAlpha (" << getName() << ") firing toggle event, new value: " << mOptionVec4->GetValueDisplay();
				auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec4]() { optionToggle->UpdateValueWithInput(); }));
				newThread.detach();
			}
			renderTooltip();
		
		DEBUG_GUI_HEIGHT;

	}

	~GUIColourPickerAlpha()
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