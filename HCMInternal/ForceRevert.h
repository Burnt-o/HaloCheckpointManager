#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MCCStateHook.h"
#include "MessagesGUI.h"
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
	gsl::not_null<std::shared_ptr<MCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<MessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceRevertFlag;

	// primary event callback
	void onForceRevert()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

		PLOG_DEBUG << "Force Revert called";
		try
		{
			byte enableFlag = 1;
			if (!forceRevertFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write Revert flag {}", MultilevelPointer::GetLastError()));
			messagesGUI->addMessage("Revert forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:
	ForceRevert(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl), 
		mForceRevertCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->forceRevertEvent, [this]() { onForceRevert(); }),
		mccStateHook(dicon.Resolve<MCCStateHook>()),
		messagesGUI(dicon.Resolve<MessagesGUI>()), 
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>();
		forceRevertFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>("forceRevertFlag", mGame);
	}

	std::string_view getName() override { return "Force Revert"; }

};