#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"


class InjectCheckpointImplBase {
public:
	virtual void onInject() = 0;
};

class InjectCheckpoint : public CheatBase
{


private:

	eventpp::CallbackList<void()>::Handle mInjectCallbackHandle = {};

	//impl
	std::unique_ptr<InjectCheckpointImplBase> impl;



public:

	InjectCheckpoint(GameState game) : CheatBase(game) {}
	~InjectCheckpoint()
	{
		// unsubscribe 
		if (mInjectCallbackHandle)
			OptionsState::injectCheckpointEvent.get()->remove(mInjectCallbackHandle);


		// kill impl
		impl.reset();
	}

	void initialize() override;



	std::string_view getName() override { return "Inject Checkpoint"; }


};