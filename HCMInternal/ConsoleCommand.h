#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"



class ConsoleCommand : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mConsoleCommandEventCallbackHandle;

	// injected services
	gsl::not_null<std::shared_ptr<IMCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<IMessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;
	gsl::not_null<std::shared_ptr<SettingsStateAndEvents>> mSettings;


	//data
	std::shared_ptr<MultilevelPointer> gameEnginePointer;
	std::shared_ptr<MultilevelPointer> sendCommandPointer;

	// primary event callback
	void onSendCommand()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

		try
		{
			PLOG_DEBUG << "onSendCommand called for game: " << mGame.toString();

			uintptr_t pCommand;
			if (!sendCommandPointer->resolve(&pCommand)) throw HCMRuntimeException("Could not resolve pointer to sendCommand function");

			uintptr_t pEngine;
			if (!gameEnginePointer->resolve(&pEngine)) throw HCMRuntimeException("Could not resolve pointer to gameEngine");


			typedef int64_t(*EngineCommand_t)(void* engine, const char* commandString);
			EngineCommand_t engine_command_vptr;
			engine_command_vptr = static_cast<EngineCommand_t>((void*)pCommand);


			std::string command = "HS: " + mSettings->consoleCommandString->GetValue();
			

			messagesGUI->addMessage(std::format("Sending command: {}", mSettings->consoleCommandString->GetValue()));
			auto commandResult = engine_command_vptr((void*)pEngine, command.c_str());
			messagesGUI->addMessage(std::format("Result: 0x{:X}", commandResult));

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:

	ConsoleCommand(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mConsoleCommandEventCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->consoleCommandEvent, [this]() {onSendCommand(); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mSettings(dicon.Resolve<SettingsStateAndEvents>())

	{
		PLOG_VERBOSE << "constructing ConsoleCommand OptionalCheat for game: " << mGame.toString();
		auto ptr = dicon.Resolve<PointerManager>();
		gameEnginePointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEnginePointer));
		sendCommandPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(sendCommandPointer), mGame);
	}

	virtual std::string_view getName() override {
		return nameof(ConsoleCommand);
	}

	~ConsoleCommand()
	{
			PLOG_VERBOSE << "~" << getName();
	}

};
