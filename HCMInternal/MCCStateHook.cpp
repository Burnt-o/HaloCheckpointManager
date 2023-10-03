#include "pch.h"
#include "MCCStateHook.h"


void MCCStateHook::updateMCCState()
{
	PLOG_DEBUG << "updating MCC State";
	try
	{
		byte currentGameUnparsed = -1;
		if (!instance->gameIndicator->readData(&currentGameUnparsed))
		{
			throw  HCMRuntimeException(std::format("Failed to read gameIndicator: {}", MultilevelPointer::GetLastError()));
		}
		PLOG_DEBUG << "read current game: " << currentGameUnparsed;

		if (!magic_enum::enum_contains<GameState::Value>(currentGameUnparsed)) throw HCMRuntimeException(std::format("Invalid currentGame value!: {}", currentGameUnparsed));

		GameState currentGame{ currentGameUnparsed };
		PLOG_DEBUG << "read current game (parsed): " << currentGame.toString();

		bool loadingFlag;
		if (!instance->loadIndicator->readData(&loadingFlag))
		{
			throw  HCMRuntimeException(std::format("Failed to read loadIndicator: {}", MultilevelPointer::GetLastError()));
		}
		PLOG_DEBUG << "read loadingFlag: " << loadingFlag;

		bool menuFlag;
		if (!instance->menuIndicator->readData(&menuFlag))
		{
			throw  HCMRuntimeException(std::format("Failed to read menuIndicator: {}", MultilevelPointer::GetLastError()));
		}
		PLOG_DEBUG << "read menuFlag: " << menuFlag;

		// load flag takes priority over mainmenu flag
		PlayState currentPlayState = loadingFlag ? PlayState::Loading : (menuFlag ? PlayState::MainMenu : PlayState::Ingame);
		PLOG_DEBUG << "read playstate: " << magic_enum::enum_name(currentPlayState);
		
		byte currentLevelIDUnparsed;
		if (!instance->levelIndicator->readData(&currentLevelIDUnparsed))
		{
			throw  HCMRuntimeException(std::format("Failed to read levelIndicator: {}", MultilevelPointer::GetLastError()));
		}
		PLOG_DEBUG << "read level ID: " << currentLevelIDUnparsed;
		if (!magic_enum::enum_contains<LevelID>(currentLevelIDUnparsed)) throw HCMRuntimeException(std::format("Invalid currentLevelID value!: {}", currentLevelIDUnparsed));

		LevelID currentLevelID{ currentLevelIDUnparsed };
		PLOG_DEBUG << "read level ID (parsed): " << magic_enum::enum_name(currentLevelID);

		// update current state
		currentMCCState = { currentGame, currentPlayState, currentLevelID };
		PLOG_DEBUG << "state updated";

		// fire event
		MCCStateChangedEvent->operator()(currentMCCState);
		PLOG_DEBUG << "event fired";
	}
	catch(HCMRuntimeException ex)
	{
		ex.prepend("An error occured while evaluating new MCC state: \n");
		runtimeExceptions->handleMessage(ex);
	}
}


