#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"


class ForceRevert : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback <ActionEvent>mForceRevertCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceRevertFlag;
	std::shared_ptr<MultilevelPointer> revertQueuedFlag; // only used/non-null for Halo1

	// primary event callback
	void onForceRevert()
	{
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force Revert called";
		try
		{
			byte enableFlag = 1;
			if (!forceRevertFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write Revert flag {}", MultilevelPointer::GetLastError()));

			if (mGame.operator GameState::Value() == GameState::Value::Halo1)
			{
				byte clearQueueFlag = 0;
				if (!revertQueuedFlag->writeData(&clearQueueFlag)) PLOG_ERROR << "Failed to clear revertQueuedFlag, error: " << MultilevelPointer::GetLastError();
			}

			messagesGUI.lock()->addMessage("Revert forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions.lock()->handleMessage(ex);
		}

	}



public:
	ForceRevert(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceRevertCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceRevertEvent, [this]() { onForceRevert(); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		forceRevertFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(forceRevertFlag), mGame);

		if (gameImpl.operator GameState::Value() == GameState::Value::Halo1)
		{
			revertQueuedFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(revertQueuedFlag), mGame);
		}
	}

	~ForceRevert()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	std::string_view getName() override { return "Force Revert"; }

};