#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MCCStateHook.h"
#include "MessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"

class ForceDoubleRevert : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mForceDoubleRevertCallbackHandle;

	// injected services
	gsl::not_null<std::shared_ptr<MCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<MessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;
	gsl::not_null<std::shared_ptr<SettingsStateAndEvents>> settings;

	//data
	std::shared_ptr<MultilevelPointer> doubleRevertFlag;

	// primary event callback
	void onForceDoubleRevert()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force DoubleRevert called";
		try
		{
			// read the flag
			byte currentFlag = 1;
			if (!doubleRevertFlag->readData(&currentFlag)) throw HCMRuntimeException(std::format("Failed to read doubleRevertFlag {}", MultilevelPointer::GetLastError()));

			// flip the value;
			currentFlag == 1 ? currentFlag = 0 : currentFlag = 1;
			
			// write the flag
			if (!doubleRevertFlag->writeData(&currentFlag)) throw HCMRuntimeException(std::format("Failed to write DoubleRevert flag {}", MultilevelPointer::GetLastError()));
			messagesGUI->addMessage("Checkpoint double reverted.");

			// fire revert event. Not our problem if it fails.
			settings->forceRevertEvent->operator()();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}




public:
	ForceDoubleRevert(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceDoubleRevertCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->forceDoubleRevertEvent, [this]() { onForceDoubleRevert(); }),
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHook(dicon.Resolve<MCCStateHook>()),
		messagesGUI(dicon.Resolve<MessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>();
		doubleRevertFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("doubleRevertFlag", mGame);
	}



	std::string_view getName() override { return nameof(ForceDoubleRevert); }


};