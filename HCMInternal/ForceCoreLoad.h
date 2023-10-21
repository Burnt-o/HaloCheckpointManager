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
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceCoreLoadFlag;

	// primary event callback
	void onForceCoreLoad()
	{
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force CoreLoad called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreLoadFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreLoad flag {}", MultilevelPointer::GetLastError()));
			messagesGUI.lock()->addMessage("CoreLoad forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions.lock()->handleMessage(ex);
		}

	}


public:

	ForceCoreLoad(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceCoreLoadCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceCoreLoadEvent, [this]() {onForceCoreLoad(); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		forceCoreLoadFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("forceCoreLoadFlag", mGame);
	}

	~ForceCoreLoad()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(ForceCoreLoad); }

};