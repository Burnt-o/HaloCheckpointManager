#pragma once
#include "IMCCStateHook.h"
class MockMCCStateHook : public IMCCStateHook
{
	MCCState currentMCCState{GameState::Value::NoGame, PlayState::MainMenu, LevelID::no_map_loaded };
	std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedEvent = std::make_shared<eventpp::CallbackList<void(const MCCState&)>>();
	public:
		virtual const MCCState& getCurrentMCCState() override { return currentMCCState; }
		virtual bool isGameCurrentlyPlaying(GameState gameToCheck) override { return false; }

		// main event we fire when a new game/level loads
		virtual std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> getMCCStateChangedEvent() { return MCCStateChangedEvent ; }
};
