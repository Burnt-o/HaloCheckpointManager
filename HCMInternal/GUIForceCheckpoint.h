#pragma once
#include "GUIElementBase.h"
#include "OptionsState.h"
class GUIForceCheckpoint : public GUIElementBase {


public:

	 GUIForceCheckpoint()
	 {
		 this->currentHeight = 20;
		 this->supportedGames = { GameState::Halo1, };
	 }

	 std::set<GameState>& getSupportedGames()
	 {
		 static std::set<GameState> mSupportedGames = { GameState::Halo1 };
		 return mSupportedGames;
	 }

	 void render() override
	 {
		 if (ImGui::Button("Force Checkpoint"))
		 {
			 OptionsState::forceCheckpointEvent();
		 }
	 }
};