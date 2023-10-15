#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"


template< bool shouldRenderHotkey>
class GUISimpleButton : public IGUIElement {

private:
	std::string mButtonText;
	std::shared_ptr<ActionEvent> mEventToFire;
public:


	GUISimpleButton(GameState implGame, std::optional<HotkeysEnum> hotkey, std::string buttonText, std::shared_ptr<ActionEvent> eventToFire)
		: IGUIElement(implGame, hotkey), mButtonText(buttonText), mEventToFire(eventToFire)
	{
		if (mButtonText.empty()) throw HCMInitException("Cannot have empty button text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUISimplebutton, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		if constexpr (shouldRenderHotkey)
		{
			hotkeyRenderer.renderHotkey(mHotkey);
			ImGui::SameLine();
		}

		if (ImGui::Button(mButtonText.c_str()))
		{
			PLOG_VERBOSE << "GUISimplebutton (" << getName() << ") firing event";
			PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
			mEventToFire->operator()();
		}
	}

	std::string_view getName() override { return mButtonText; }

};