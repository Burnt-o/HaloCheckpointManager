#pragma once
#include "pch.h"
#include "HaloEnums.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"
#include "GameStateHook.h"

class ForceDoubleRevert : public CheatBase
{
private:

	eventpp::CallbackList<void()>::Handle mForceDoubleRevertCallbackHandle = {};

	void onForceDoubleRevert()
	{
		if (GameStateHook::getCurrentGameState() != mGame) return;

		PLOG_DEBUG << "Force DoubleRevert called";
		try
		{
			// read the flag
			byte currentFlag = 1;
			if (!doubleRevertFlag.get()->readData(&currentFlag)) throw HCMRuntimeException(std::format("Failed to read doubleRevertFlag {}", MultilevelPointer::GetLastError()));

			// flip the value;
			currentFlag == 1 ? currentFlag = 0 : currentFlag = 1;
			
			// write the flag
			if (!doubleRevertFlag.get()->writeData(&currentFlag)) throw HCMRuntimeException(std::format("Failed to write DoubleRevert flag {}", MultilevelPointer::GetLastError()));
			MessagesGUI::addMessage("Checkpoint double reverted.");

			// fire revert event. Not our problem if it fails.
			OptionsState::forceRevertEvent();
		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}

	//data
	std::shared_ptr<MultilevelPointer> doubleRevertFlag;

public:

	ForceDoubleRevert(GameState gameImpl) : CheatBase(gameImpl)
	{

	}

	~ForceDoubleRevert()
	{
		// unsubscribe 
		if (mForceDoubleRevertCallbackHandle)
			OptionsState::forceDoubleRevertEvent.remove(mForceDoubleRevertCallbackHandle);
	}

	void initialize() override
	{
		// exceptions thrown up to creator
		doubleRevertFlag = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("doubleRevertFlag", mGame);

		// subscribe to forceDoubleRevert event
		mForceDoubleRevertCallbackHandle = OptionsState::forceDoubleRevertEvent.append([this]() { this->onForceDoubleRevert(); });

	}

	std::string_view getName() override { return "Force DoubleRevert"; }


};