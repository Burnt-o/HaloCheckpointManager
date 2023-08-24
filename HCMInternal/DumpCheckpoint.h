#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"


class DumpCheckpointImplBase {
public:
	virtual void onDump() = 0;
};

class DumpCheckpoint : public CheatBase
{


private:

	eventpp::CallbackList<void()>::Handle mDumpCallbackHandle = {};

	//impl
	std::unique_ptr<DumpCheckpointImplBase> impl;



public:

	DumpCheckpoint(GameState game) : CheatBase(game) {}
	~DumpCheckpoint()
	{
		if (mDumpCallbackHandle)
			OptionsState::dumpCheckpointEvent.get()->remove(mDumpCallbackHandle);

		// kill impl
		impl.reset();
	}

	void initialize() override;



	std::string_view getName() override { return "Dump Checkpoint"; }


};