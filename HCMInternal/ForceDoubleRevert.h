#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
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
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> settings;

	//data
	std::shared_ptr<MultilevelPointer> doubleRevertFlag;

	// primary event callback
	void onForceDoubleRevert()
	{
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame) == false) return;

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
			messagesGUI.lock()->addMessage("Checkpoint double reverted.");

			// fire revert event. Not our problem if it fails.
			settings.lock()->forceRevertEvent->operator()();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions.lock()->handleMessage(ex);
		}

	}




public:
	ForceDoubleRevert(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceDoubleRevertCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceDoubleRevertEvent, [this]() { onForceDoubleRevert(); }),
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		doubleRevertFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("doubleRevertFlag", mGame);
	}

	~ForceDoubleRevert()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	std::string_view getName() override { return nameof(ForceDoubleRevert); }


};