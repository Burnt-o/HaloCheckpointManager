#pragma once
#include "MCCState.h"

class IMCCStateHook {
public:
	virtual const MCCState& getCurrentMCCState() = 0;
	virtual bool isGameCurrentlyPlaying(GameState gameToCheck) = 0;

	// main event we fire when a new game/level loads
	virtual std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> getMCCStateChangedEvent() = 0;
	virtual ~IMCCStateHook() = default;
};
