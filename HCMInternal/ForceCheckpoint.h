#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"
#include "GameStateHook.h"
class ForceCheckpoint : public CheatBase
{
private:

	eventpp::CallbackList<void()>::Handle mForceCheckpointCallbackHandle = {};

	void onForceCheckpoint()
	{
		if (GameStateHook::getCurrentGameState() != mGame) return;

		PLOG_DEBUG << "Force checkpoint called";
		try
		{
			byte enableFlag = 1;
			if (!forceCheckpointFlag.get()->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write checkpoint flag {}", MultilevelPointer::GetLastError()));
			MessagesGUI::addMessage("Checkpoint forced.");
		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}

	//data
	std::shared_ptr<MultilevelPointer> forceCheckpointFlag;

public:

	ForceCheckpoint(GameState gameImpl) : CheatBase(gameImpl)
	{

	}

	~ForceCheckpoint()
	{
		// unsubscribe 
		if (mForceCheckpointCallbackHandle)
			OptionsState::forceCheckpointEvent.get()->remove(mForceCheckpointCallbackHandle);
	}

	void initialize() override
	{
		// exceptions thrown up to creator
		forceCheckpointFlag = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("forceCheckpointFlag", mGame);

		// subscribe to forcecheckpoint event
		mForceCheckpointCallbackHandle = OptionsState::forceCheckpointEvent.get()->append([this]() { this->onForceCheckpoint(); });
	}

	std::string_view getName() override { return "Force Checkpoint"; }
	

};