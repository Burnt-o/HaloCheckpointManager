#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MCCStateHook.h"
#include "MessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
class ForceCoreSave : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mForceCoreSaveCallbackHandle;

	// injected services
	gsl::not_null<std::shared_ptr<MCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<MessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceCoreSaveFlag;

	// primary event callback
	void onForceCoreSave()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force CoreSave called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreSaveFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreSave flag {}", MultilevelPointer::GetLastError()));
			messagesGUI->addMessage("CoreSave forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


public:

	ForceCoreSave(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceCoreSaveCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->forceCoreSaveEvent, [this]() {onForceCoreSave(); }),
		mccStateHook(dicon.Resolve<MCCStateHook>()),
		messagesGUI(dicon.Resolve<MessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>();
		forceCoreSaveFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("forceCoreSaveFlag", mGame);
	}

	virtual std::string_view getName() override { return nameof(ForceCoreSave); }

};