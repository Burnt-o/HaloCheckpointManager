#pragma once
#include "pch.h"
#include "HaloEnums.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"


class CheckpointInjectDumpImpl {
public:
	virtual void onInject() = 0;
	virtual void onDump() = 0;
};

class CheckpointInjectDump : public CheatBase
{


private:

	eventpp::CallbackList<void()>::Handle mInjectCallbackHandle = {};
	eventpp::CallbackList<void()>::Handle mDumpCallbackHandle = {};

	//impl
	std::unique_ptr<CheckpointInjectDumpImpl> impl;

	

public:

	CheckpointInjectDump(GameState game) : CheatBase(game) {}
	~CheckpointInjectDump()
	{
		// unsubscribe 
		if (mInjectCallbackHandle)
			OptionsState::injectCheckpointEvent.remove(mInjectCallbackHandle);

		if (mDumpCallbackHandle)
			OptionsState::dumpCheckpointEvent.remove(mDumpCallbackHandle);

		// kill impl
		impl.reset();
	}

	void initialize() override;



	std::string_view getName() override { return "Inject/Dump Checkpoint"; }
	std::set<GameState> getSupportedGames() override { return std::set<GameState>{GameState::Halo1}; }


};