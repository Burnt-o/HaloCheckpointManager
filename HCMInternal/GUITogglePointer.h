#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
#include "BitBool.h"
template< bool shouldRenderHotkey, bool isBitOffset>
class GUITogglePointer : public IGUIElement {

private:
	std::string mToggleText;
	std::weak_ptr<PointerSetting<bool>> mOptionTogglePointerWeak;

public:


	GUITogglePointer(GameState implGame, ToolTipCollection tooltip, std::optional<RebindableHotkeyEnum> hotkey, std::string toggleText, std::shared_ptr<PointerSetting<bool>> optionTogglePointer)
		: IGUIElement(implGame, hotkey, tooltip), mToggleText(toggleText), mOptionTogglePointerWeak(optionTogglePointer)
	{
		if (mOptionTogglePointerWeak.lock()->successfullyConstructed(implGame) == false)
			throw HCMInitException(std::format("Could not construct {}: {}", getName(), mOptionTogglePointerWeak.lock()->getLastError(implGame)));
		if (mToggleText.empty()) 
			throw HCMInitException("Cannot have empty toggle text (needs label for imgui ID system, use ## for invisible labels)");

		PLOG_VERBOSE << "Constructing GUITogglePointer, name: " << getName();
		PLOG_DEBUG << "mOptionTogglePointer.getOptionName: " << std::hex << mOptionTogglePointerWeak.lock()->getOptionName();
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionTogglePointer = mOptionTogglePointerWeak.lock();
		if (!mOptionTogglePointer)
		{
			PLOG_ERROR << "bad mOptionToggle weakptr when rendering " << getName();
			return;
		}

		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}

		if constexpr (isBitOffset)
		{
			auto bitBool = mOptionTogglePointer->getBitBool(mImplGame);
			if (bitBool == std::nullopt)
			{
				ImGui::Text(std::format("{} error: {}", mToggleText, mOptionTogglePointer->getLastError(mImplGame)).c_str());
			}
			else
			{
				bool tempValue = bitBool.value().operator const bool();
				if (ImGui::Checkbox(mToggleText.c_str(), &tempValue))
				{
					bitBool.value().set(tempValue);
				}

			}

		}
		else
		{
			auto resolvedDataPointer = mOptionTogglePointer->getRef(mImplGame);
			if (resolvedDataPointer == std::nullopt)
			{
				ImGui::Text(std::format("{} error: {}", mToggleText, mOptionTogglePointer->getLastError(mImplGame)).c_str());
			}
			else
			{
				ImGui::Checkbox(mToggleText.c_str(), resolvedDataPointer.value());
			}
		}



		renderTooltip();
		DEBUG_GUI_HEIGHT;
	}

	~GUITogglePointer()
	{

	}

	std::string_view getName() override { return mToggleText; }

};