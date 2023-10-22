#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"


template< bool shouldRenderHotkey>
class GUIAdvanceTicks : public IGUIElement {

private:
	std::weak_ptr<ActionEvent> mEventToFireWeak;
	std::weak_ptr<Setting<int>> mBoundIntWeak;
	std::vector<std::thread> mFireEventThreads;
public:


	GUIAdvanceTicks(GameState implGame, std::optional<HotkeysEnum> hotkey, std::weak_ptr<ActionEvent> eventToFire, std::weak_ptr<Setting<int>> boundInt)
		: IGUIElement(implGame, hotkey),  mEventToFireWeak(eventToFire), mBoundIntWeak(boundInt)
	{
		PLOG_VERBOSE << "Constructing GUIAdvanceTicks";
		this->currentHeight = 20;
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
		ImGui::SameLine();
		ImGui::SetNextItemWidth(75.f);
		if (ImGui::InputInt("##advanceTicksInt", &mBoundInt->GetValueDisplay(), 1, 5))
		{
			mBoundInt->UpdateValueWithInput(); // nothing actually subscribes to this value changed event so doesn't matter that it's on ui thread
		}
		ImGui::SameLine();
		ImGui::Text(mBoundInt->GetValue() == 1 ? "tick" : "ticks");


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