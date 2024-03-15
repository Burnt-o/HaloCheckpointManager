#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
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
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceCoreLoadFlag;
	std::shared_ptr<MultilevelPointer> revertQueuedFlag; // only used/non-null for Halo1

	// primary event callback
	void onForceCoreLoad()
	{

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
			PLOG_DEBUG << "Force CoreLoad called";


			byte enableFlag = 1;
			if (!forceCoreLoadFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreLoad flag {}", MultilevelPointer::GetLastError()));
			
			if (mGame.operator GameState::Value() == GameState::Value::Halo1)
			{
				byte clearQueueFlag = 0;
				if (!revertQueuedFlag->writeData(&clearQueueFlag)) PLOG_ERROR << "Failed to clear revertQueuedFlag, error: " << MultilevelPointer::GetLastError();
			}
			
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
		mForceCoreLoadCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceCoreLoadEvent, [this]() {onForceCoreLoad(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		forceCoreLoadFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("forceCoreLoadFlag", mGame);

		if (gameImpl.operator GameState::Value() == GameState::Value::Halo1)
		{
			revertQueuedFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(revertQueuedFlag), mGame);
		}
	}

	~ForceCoreLoad()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(ForceCoreLoad); }

};