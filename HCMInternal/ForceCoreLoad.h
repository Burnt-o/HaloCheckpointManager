#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MCCStateHook.h"
#include "MessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
class ForceCoreLoad : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mForceCoreLoadCallbackHandle;

	// injected services
	gsl::not_null<std::shared_ptr<MCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<MessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceCoreLoadFlag;

	// primary event callback
	void onForceCoreLoad()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force CoreLoad called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreLoadFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreLoad flag {}", MultilevelPointer::GetLastError()));
			messagesGUI->addMessage("CoreLoad forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


public:

	ForceCoreLoad(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceCoreLoadCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->forceCoreLoadEvent, [this]() {onForceCoreLoad(); }),
		mccStateHook(dicon.Resolve<MCCStateHook>()),
		messagesGUI(dicon.Resolve<MessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>();
		forceCoreLoadFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("forceCoreLoadFlag", mGame);
	}

	virtual std::string_view getName() override { return nameof(ForceCoreLoad); }

};