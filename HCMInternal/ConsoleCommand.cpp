#pragma once
#include "pch.h"
#include "ConsoleCommand.h"
#include "IMCCStateHook.h"

#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "winnt.h"

template <GameState::Value gameT>
class ConsoleCommandImpl : public IConsoleCommand
{
private:

	// event callbacks
	ScopedCallback<ActionEvent> mConsoleCommandEventCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;


	//data
	std::shared_ptr<MultilevelPointer> gameEnginePointer;
	std::shared_ptr<MultilevelPointer> sendCommandPointer;

	// primary event callback
	void onSendCommand()
	{

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(mSettingsWeak, mSettings);
			lockOrThrow(messagesGUIWeak, messagesGUI);

			if (mccStateHook->isGameCurrentlyPlaying((GameState)gameT) == false) return;

			std::string command = mSettings->consoleCommandString->GetValue();



			messagesGUI->addMessage(std::format("Sending command: {}", mSettings->consoleCommandString->GetValue()));

			if (command.contains("gamespeed"))
				return;

			sendCommand(command);

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:

	ConsoleCommandImpl(GameState game, IDIContainer& dicon)
		: 
		mConsoleCommandEventCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->consoleCommandEvent, [this]() {onSendCommand(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mSettingsWeak(dicon.Resolve<SettingsStateAndEvents>())

	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		gameEnginePointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEnginePointer));
		sendCommandPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(sendCommandPointer), game);
	}



	void sendCommand(std::string commandString)
	{


		uintptr_t pCommand;
		if (!sendCommandPointer->resolve(&pCommand)) throw HCMRuntimeException("Could not resolve pointer to sendCommand function");

		uintptr_t pEngine;
		if (!gameEnginePointer->resolve(&pEngine)) throw HCMRuntimeException("Could not resolve pointer to gameEngine");



		typedef void(*EngineCommand_t)(uintptr_t pEngine, const char* commandString);
		EngineCommand_t engine_command_vptr;
		engine_command_vptr = static_cast<EngineCommand_t>((void*)pCommand);


		std::string command = "HS: " + commandString;

		engine_command_vptr(pEngine, command.c_str());


		/* TODO: how to get output of a command? eg calling something like "cheat_deathless_player" should return a true or a false.
		IDA reckons pEngine is PSLIST_HEADER (a sequenced singlely-linked list - i think), and the func returns a PSLIST_ENTRY.
		But in practice the return is always 0. so uh not sure what's going on there.

		However there definitely does seem to be a PSLIST_HEADER structure in the game engine pointer. 
			- I can see the entry count getting incremented every time I call this function

		Handy resources:
		https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/singly-and-doubly-linked-lists#singly-linked-lists
		https://github.com/MicrosoftDocs/windows-driver-docs-ddi/blob/9b77e5eebca6410941311ecaf65507cf12734ed9/wdk-ddi-src/content/wdm/ns-wdm-_slist_entry.md
		https://www.nirsoft.net/kernel_struct/vista/SLIST_HEADER.html


		could try calling ExQueryDepthSList function (wdm.h) on enginePointer? https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exquerydepthslist
		
		48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 48 8b f9 48 8b f2 48 81 c1 40040000
		*/

	}


};



ConsoleCommand::ConsoleCommand(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<ConsoleCommandImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<ConsoleCommandImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<ConsoleCommandImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<ConsoleCommandImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<ConsoleCommandImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<ConsoleCommandImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

ConsoleCommand::~ConsoleCommand() = default;