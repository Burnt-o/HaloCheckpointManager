#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"
#include "GameStateHook.h"

class ForceCoreSave : public CheatBase
{
private:

	eventpp::CallbackList<void()>::Handle mForceCoreSaveCallbackHandle = {};

	//data
	std::shared_ptr<MultilevelPointer> forceCoreSaveFlag;

	void onForceCoreSave()
	{
		if (GameStateHook::getCurrentGameState() != mGame) return;

		PLOG_DEBUG << "Force core save called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreSaveFlag.get()->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write core save flag {}", MultilevelPointer::GetLastError()));
			MessagesGUI::addMessage("Core Save forced.");
		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}



public:

	ForceCoreSave(GameState gameImpl) : CheatBase(gameImpl)
	{

	}

	~ForceCoreSave()
	{
		// unsubscribe 
		if (mForceCoreSaveCallbackHandle)
			OptionsState::forceCoreSaveEvent.get()->remove(mForceCoreSaveCallbackHandle);
	}

	void initialize() override
	{
		// exceptions thrown up to creator
		forceCoreSaveFlag = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("forceCoreSaveFlag", mGame);

		// subscribe to forceCoreSave event
		mForceCoreSaveCallbackHandle = OptionsState::forceCoreSaveEvent.get()->append([this]() { this->onForceCoreSave(); });
	}

	std::string_view getName() override { return "Force Core Save"; }


};