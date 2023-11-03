#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

enum class GUIToggleWithChildrenParameters
{
	AlwaysShowChildren,
	ShowWhenTrue,
	ShowWhenFalse
};


template< GUIToggleWithChildrenParameters childShowCondition, bool shouldRenderHotkey>
class GUIToggleWithChildren : public IGUIElement {

private:
	std::string mToggleText;
	std::weak_ptr<Setting<bool>> mOptionToggleWeak;
	std::vector<std::thread> mUpdateSettingThreads;
	std::vector<std::shared_ptr<IGUIElement>> mChildElements;
public:


	GUIToggleWithChildren(GameState implGame, std::optional<HotkeysEnum> hotkey, std::string toggleText, std::shared_ptr<Setting<bool>> optionToggle, std::vector<std::optional<std::shared_ptr<IGUIElement>>> childElements)
		: IGUIElement(implGame, hotkey), mToggleText(toggleText), mOptionToggleWeak(optionToggle)
	{
		if (mToggleText.empty()) throw HCMInitException("Cannot have empty toggle text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIToggleWithChildrenParameters, name: " << getName();
		PLOG_DEBUG << "mOptionToggle.getOptionName: " << std::hex << mOptionToggleWeak.lock()->getOptionName();


		for (auto maybeElement : childElements)
		{
			if (maybeElement.has_value()) mChildElements.push_back(maybeElement.value());
		}

		this->currentHeight = GUIFrameHeightWithSpacing + GUISpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionToggle = mOptionToggleWeak.lock();
		if (!mOptionToggle)
		{
			PLOG_ERROR << "bad mOptionToggle weakptr when rendering " << getName();
			return;
		}

		ImGui::BeginChild(std::format("##children{}", mToggleText).c_str(), { 450, currentHeight - GUISpacing});

		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}


		if (ImGui::Checkbox(mToggleText.c_str(), &mOptionToggle->GetValueDisplay()))
		{
			PLOG_VERBOSE << "GUIToggleWithChildrenParameters (" << getName() << ") firing toggle event, new value: " << mOptionToggle->GetValueDisplay();
			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionToggle]() { optionToggle->UpdateValueWithInput(); }));
			newThread.detach();
		}

		DEBUG_GUI_HEIGHT;




		bool shouldShowChildrenThisFrame = true;
		if constexpr (childShowCondition == GUIToggleWithChildrenParameters::ShowWhenTrue)
		{
			shouldShowChildrenThisFrame = mOptionToggle->GetValueDisplay();
		}
		else if constexpr (childShowCondition == GUIToggleWithChildrenParameters::ShowWhenFalse)
		{
			shouldShowChildrenThisFrame = !mOptionToggle->GetValueDisplay();
		}

		this->currentHeight = GUIFrameHeightWithSpacing + GUISpacing;
		if (shouldShowChildrenThisFrame)
		{
			
			for (auto& element : mChildElements)
			{
				auto thisElementHeight = element->getCurrentHeight();
				currentHeight += thisElementHeight;

				ImGui::Dummy({ 20, thisElementHeight < 3 ? 0 : thisElementHeight - GUISpacing }); // left margin
				ImGui::SameLine();
				element->render(hotkeyRenderer);
			}

		}

		ImGui::EndChild();

	}

	~GUIToggleWithChildren()
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

	std::string_view getName() override { return mToggleText; }

};