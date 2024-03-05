#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"


template< bool shouldRenderHotkey>
class GUIAdvanceTicks : public IGUIElement {

private:
	std::weak_ptr<ActionEvent> mEventToFireWeak;
	std::weak_ptr<BinarySetting<int>> mBoundIntWeak;
	std::vector<std::thread> mFireEventThreads;
public:


	GUIAdvanceTicks(GameState implGame, ToolTipCollection tooltip, std::optional<RebindableHotkeyEnum> hotkey, std::weak_ptr<ActionEvent> eventToFire, std::weak_ptr<BinarySetting<int>> boundInt)
		: IGUIElement(implGame, hotkey, tooltip),  mEventToFireWeak(eventToFire), mBoundIntWeak(boundInt)
	{
		PLOG_VERBOSE << "Constructing GUIAdvanceTicks";
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		

		auto mEventToFire = mEventToFireWeak.lock();
		if (!mEventToFire)
		{
			PLOG_ERROR << "bad mEventToFire weakptr when rendering " << getName();
			return;
		}

		auto mBoundInt = mBoundIntWeak.lock();
		if (!mBoundInt)
		{
			PLOG_ERROR << "bad mBoundInt weakptr when rendering " << getName();
			return;
		}

		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}

		if (ImGui::Button("Advance"))
		{
			PLOG_VERBOSE << "GUISimplebutton (" << getName() << ") firing event";
			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire]() {mEvent->operator()(); }));
			newThread.detach();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(75.f);
		if (ImGui::InputInt("##advanceTicksInt", &mBoundInt->GetValueDisplay(), 1, 5))
		{
			mBoundInt->UpdateValueWithInput(); // nothing actually subscribes to this value changed event so doesn't matter that it's on ui thread
		}
		renderTooltip();
		ImGui::SameLine();
		ImGui::Text(mBoundInt->GetValue() == 1 ? "tick" : "ticks");
		renderTooltip();


	}

	~GUIAdvanceTicks()
	{
		for (auto& thread : mFireEventThreads)
		{
			if (thread.joinable())
			{
				PLOG_DEBUG << getName() << "joining mFireEventThread";
				thread.join();
				PLOG_DEBUG << getName() << "FireEventThread finished";
			}

		}
	}

	std::string_view getName() override { return nameof(GUIAdvanceTicks); }

};