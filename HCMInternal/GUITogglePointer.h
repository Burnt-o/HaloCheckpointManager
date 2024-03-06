#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template< bool shouldRenderHotkey>
class GUITogglePointer : public IGUIElement {

private:
	std::string mToggleText;
	std::weak_ptr<PointerSetting<bool>> mOptionTogglePointerWeak;

public:


	GUITogglePointer(GameState implGame, ToolTipCollection tooltip, std::optional<RebindableHotkeyEnum> hotkey, std::string toggleText, std::shared_ptr<PointerSetting<bool>> optionTogglePointer)
		: IGUIElement(implGame, hotkey, tooltip), mToggleText(toggleText), mOptionTogglePointerWeak(optionTogglePointer)
	{
		if (mToggleText.empty()) throw HCMInitException("Cannot have empty toggle text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUITogglePointer, name: " << getName();
		PLOG_DEBUG << "mOptionTogglePointer.getOptionName: " << std::hex << mOptionToggleWeak.lock()->getOptionName();
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

		auto resolvedDataPointer = mOptionTogglePointer->getRef(mImplGame);
		if (resolvedDataPointer = std::nullopt)
		{
			ImGui::Text(std::format("{} error: {}", mToggleText, mOptionTogglePointer->getLastError()));
		}
		else
		{
			ImGui::Checkbox(mToggleText.c_str(), resolvedDataPointer.value());
		}

		renderTooltip();
		DEBUG_GUI_HEIGHT;
	}

	~GUITogglePointer()
	{

	}

	std::string_view getName() override { return mToggleText; }

};