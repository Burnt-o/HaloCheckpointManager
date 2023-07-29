#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUISimpleButton : public GUIElementBase {

private:
	std::string mButtonText;
	eventpp::CallbackList<void()>& mEventToFire;

public:

	// TODO: how to check for failed services and uninclude that supported game?


	GUISimpleButton(std::set<GameState> supGames, std::string buttonText, eventpp::CallbackList<void()>& eventToFire)
		: mButtonText(buttonText), mEventToFire(eventToFire)
	{
		this->currentHeight = 20;
		this->supportedGames = supGames;
	}

	std::set<GameState>& getSupportedGames()
	{
		return this->supportedGames;
	}

	void render() override
	{
		if (ImGui::Button(mButtonText.c_str()))
		{
			mEventToFire();
		}
	}
};