#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"



class DumpCore : public CheatBase
{
private:

	eventpp::CallbackList<void()>::Handle mDumpCallbackHandle = {};

	void onDump();

public:

	DumpCore(GameState game) : CheatBase(game) {}
	~DumpCore()
	{
		// unsubscribe 

		if (mDumpCallbackHandle)
			OptionsState::dumpCoreEvent.get()->remove(mDumpCallbackHandle);
	}

	void initialize() override
	{
		mDumpCallbackHandle = OptionsState::dumpCoreEvent.get()->append([this]() {
			onDump();
			});
	}



	std::string_view getName() override { return "Dump Core"; }


};