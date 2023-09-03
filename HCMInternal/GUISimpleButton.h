#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUISimpleButton : public GUIElementBase {

private:
	std::string mButtonText;
	std::shared_ptr<ActionEvent> mEventToFire;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	virtual std::string_view getHotkeyButtonLabelId() override { return mHotkeyButtonLabelId; }
public:


	GUISimpleButton(GameState implGame, std::vector<std::map<GameState, std::shared_ptr<CheatBase>>> requiredServices, std::optional<std::shared_ptr<Hotkey>> hotkey, std::string buttonText, std::shared_ptr<ActionEvent> eventToFire)
		: GUIElementBase(implGame, requiredServices, hotkey), mButtonText(buttonText), mEventToFire(eventToFire)
	{
		if (mButtonText.empty()) throw HCMInitException("Cannot have empty button text (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUISimplebutton, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render() override
	{
		renderHotkey();
		ImGui::SameLine();
		if (ImGui::Button(mButtonText.c_str()))
		{
			PLOG_VERBOSE << "GUISimplebutton (" << getName() << ") firing event";
			PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
			mEventToFire.get()->operator()();
		}
	}

	std::string_view getName() override { return mButtonText; }

};