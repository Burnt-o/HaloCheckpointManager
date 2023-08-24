#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"



class InjectCore : public CheatBase
{
private:

	eventpp::CallbackList<void()>::Handle mInjectCallbackHandle = {};

	void onInject();

public:

	InjectCore(GameState game) : CheatBase(game) {}
	~InjectCore()
	{
		// unsubscribe 
		if (mInjectCallbackHandle)
			OptionsState::injectCoreEvent.get()->remove(mInjectCallbackHandle);


	}

	void initialize() override
	{
		mInjectCallbackHandle = OptionsState::injectCoreEvent.get()->append([this]() {
			onInject();
			});
	}



	std::string_view getName() override { return "Inject Core"; }


};