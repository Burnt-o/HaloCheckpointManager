#pragma once
#include "imgui.h"
#include "IMCCStateHook.h"
#include "ScopedCallback.h"
class GUIMCCState
{
private:

	std::string headerText;
	void OnStateChange(const MCCState& newState)
	{


		headerText = std::format("Current Game: {0} ({1}), Level: {2}",
			newState.currentGameState.toString(),
			magic_enum::enum_name(newState.currentPlayState).data(),
			magic_enum::enum_name(newState.currentLevelID).data()
		);
	}

	ScopedCallback<eventpp::CallbackList<void(const MCCState&)>> MCCStateChangeCallback;

public:


	GUIMCCState(std::shared_ptr<IMCCStateHook> mccStateHook) :
		MCCStateChangeCallback(mccStateHook->getMCCStateChangedEvent(), [this](const MCCState& n) {OnStateChange(n); })
	{
		// init strings
		OnStateChange(mccStateHook->getCurrentMCCState());
		// subscribe to mccStateChange so we can update strings on change
	}

	void render() 
	{
		ImGui::Text(headerText.c_str());
	}

};