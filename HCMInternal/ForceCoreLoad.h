#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"
#include "GameStateHook.h"
class ForceCoreLoad : public CheatBase
{
private:

	eventpp::CallbackList<void()>::Handle mForceCoreLoadCallbackHandle = {};

	void onForceCoreLoad()
	{
		if (GameStateHook::getCurrentGameState() != mGame) return;

		PLOG_DEBUG << "Force CoreLoad called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreLoadFlag.get()->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreLoad flag {}", MultilevelPointer::GetLastError()));
			MessagesGUI::addMessage("CoreLoad forced.");
		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}

	//data
	std::shared_ptr<MultilevelPointer> forceCoreLoadFlag;

public:

	ForceCoreLoad(GameState gameImpl) : CheatBase(gameImpl)
	{

	}

	~ForceCoreLoad()
	{
		// unsubscribe 
		if (mForceCoreLoadCallbackHandle)
			OptionsState::forceCoreLoadEvent.get()->remove(mForceCoreLoadCallbackHandle);
	}

	void initialize() override
	{
		// exceptions thrown up to creator
		forceCoreLoadFlag = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("forceCoreLoadFlag", mGame);

		// subscribe to forceCoreLoad event
		mForceCoreLoadCallbackHandle = OptionsState::forceCoreLoadEvent.get()->append([this]() { this->onForceCoreLoad(); });

	}

	std::string_view getName() override { return "Force Core Load"; }

};