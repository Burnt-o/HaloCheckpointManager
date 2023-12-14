#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"


template< bool shouldRenderHotkey>
class GUISimpleButton : public IGUIElement {

private:
	std::string mButtonText;
	std::weak_ptr<ActionEvent> mEventToFireWeak;
	std::vector<std::thread> mFireEventThreads;
public:


	GUISimpleButton(GameState implGame, ToolTipCollection tooltip, std::optional<RebindableHotkeyEnum> hotkey, std::string buttonText, std::shared_ptr<ActionEvent> eventToFire)
		: IGUIElement(implGame, hotkey, tooltip), mButtonText(buttonText), mEventToFireWeak(eventToFire)
	{
		if (mButtonText.empty()) throw HCMInitException("Cannot have empty button text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUISimplebutton, name: " << getName();
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


		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}

		if (ImGui::Button(mButtonText.c_str()))
		{
			PLOG_VERBOSE << "GUISimplebutton (" << getName() << ") firing event";
			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire]() {mEvent->operator()(); }));
			newThread.detach();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;
	}

	~GUISimpleButton()
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

	std::string_view getName() override { return mButtonText; }

};