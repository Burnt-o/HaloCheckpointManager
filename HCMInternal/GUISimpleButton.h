#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUISimpleButton : public GUIElementBase {

private:
	std::string mButtonText;
	eventpp::CallbackList<void()>* mEventToFire;

public:

	// TODO: how to check for failed services and uninclude that supported game?


	GUISimpleButton(GameState implGame, std::map<GameState, std::shared_ptr<CheatBase>>& requiredService, std::string buttonText, eventpp::CallbackList<void()>* eventToFire)
		: GUIElementBase(implGame, requiredService), mButtonText(buttonText), mEventToFire(eventToFire)
	{
		PLOG_VERBOSE << "Constructing GUISimplebutton, name: " << this->mButtonText;
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render() override
	{
		if (ImGui::Button(mButtonText.c_str()))
		{
			PLOG_VERBOSE << "GUISimplebutton (" << this->mButtonText << ") firing event";
			PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
			mEventToFire->operator()();
		}
	}
};