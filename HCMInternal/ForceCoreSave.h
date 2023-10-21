#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
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
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceCoreSaveFlag;

	// primary event callback
	void onForceCoreSave()
	{
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force CoreSave called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreSaveFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreSave flag {}", MultilevelPointer::GetLastError()));
			messagesGUI.lock()->addMessage("CoreSave forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions.lock()->handleMessage(ex);
		}

	}


public:

	ForceCoreSave(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceCoreSaveCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceCoreSaveEvent, [this]() {onForceCoreSave(); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		forceCoreSaveFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("forceCoreSaveFlag", mGame);
	}

	~ForceCoreSave()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(ForceCoreSave); }

};