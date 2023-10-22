#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MultilevelPointer.h"
#include "ControlServiceContainer.h"
#include "ModuleHook.h"
#include "IMCCStateHook.h"

class AdvanceTicksImpl;
class AdvanceTicks : public IOptionalCheat
{
private:
	GameState mGame;

	// event callbacks
	ScopedCallback <ActionEvent> mAdvanceTicksCallbackHandle;
	std::weak_ptr<IMCCStateHook> mccStateHook;

	static std::unique_ptr<AdvanceTicksImpl> pimpl; // one shared instance of impl

	// hooks 
	std::shared_ptr<ModuleMidHook> tickIncrementHook; // seperate hook for each game, each gets bound to impls tickIncrementHookFunction

	// event callback ( the main stuff happens in a different callback in impl, this just turns on the hook)
	void onAdvanceTicksEvent()
	{
		// Starts unattached, gets turned on the first time advanceTicks is called,
		// juuuust in case there's a bug/crash with the hook
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame))
			tickIncrementHook->setWantsToBeAttached(true); 
	}

public:
	AdvanceTicks(GameState gameImpl, IDIContainer& dicon);

	~AdvanceTicks();

	std::string_view getName() override { return nameof(AdvanceTicks); }



};