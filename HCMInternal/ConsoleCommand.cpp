#pragma once
#include "pch.h"
#include "ConsoleCommand.h"
#include "IMCCStateHook.h"

#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "MidhookContextInterpreter.h"
#include "ModuleHook.h"

template <GameState::Value gameT>
class ConsoleCommandImpl : public IConsoleCommand
{
private:
	static inline std::mutex mDestructionGuard{};

	static inline ConsoleCommandImpl<gameT>* instance = nullptr;

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

	std::shared_ptr<ModuleMidHook> commandOutputStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandOutputStringFunctionContext;

	std::optional<std::string> lastCommandOutputString;


	// primary user fired event callback
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

			auto commandOutput = sendCommand(command);

			if (commandOutput)
				messagesGUI->addMessage(std::format("Command Output: {}", commandOutput.value()));
			else
			{
#ifdef HCM_DEBUG 
				messagesGUI->addMessage("Command had no output!");
#endif
			}

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


	// for extracting command output strings
	static void commandOutputStringHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		std::unique_lock<std::mutex> lock(mDestructionGuard);
		LOG_ONCE(PLOG_DEBUG << "commandOutputStringHookFunction running");

		try
		{
			enum class param
			{
				pOutputString
			};
			auto* ctxInterpreter = instance->commandOutputStringFunctionContext.get();

			const char* outputChars = (const char*)ctxInterpreter->getParameterRef(ctx, (int)param::pOutputString);
			if (IsBadReadPtr(outputChars, 4))
				throw HCMRuntimeException(std::format("Bad read of command string output characters! at {:X}", (uintptr_t)outputChars));

			instance->lastCommandOutputString = std::string(outputChars);
		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
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
		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		gameEnginePointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEnginePointer));
		sendCommandPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(sendCommandPointer), game);

		auto commandOutputStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandOutputStringFunction), game);
		commandOutputStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandOutputStringFunctionContext), game);
		commandOutputStringHook = ModuleMidHook::make(game.toModuleName(), commandOutputStringFunction, commandOutputStringHookFunction);
	}

	~ConsoleCommandImpl()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks/hooks finish executing
		instance = nullptr;
	}

	std::optional<std::string> sendCommand(std::string commandString)
	{

		uintptr_t pCommand;
		if (!sendCommandPointer->resolve(&pCommand)) throw HCMRuntimeException("Could not resolve pointer to sendCommand function");

		uintptr_t pEngine;
		if (!gameEnginePointer->resolve(&pEngine)) throw HCMRuntimeException("Could not resolve pointer to gameEngine");



		typedef void(*EngineCommand_t)(uintptr_t pEngine, const char* commandString);
		EngineCommand_t engine_command_vptr;
		engine_command_vptr = static_cast<EngineCommand_t>((void*)pCommand);


		std::string command = "HS: " + commandString;


		lastCommandOutputString = std::nullopt;
		commandOutputStringHook->setWantsToBeAttached(true);

		// Execute the command
		engine_command_vptr(pEngine, command.c_str());

		// Command output generation happens on a different game thread, er there's probably a better way to do this.
		// Keep in mind that depending on the command, there may be no output at all.
		int maxSleepTime = 3;
		while (lastCommandOutputString.has_value() == false && maxSleepTime > 0)
		{
			maxSleepTime--;
			Sleep(1);
		}

		commandOutputStringHook->setWantsToBeAttached(false);
		return lastCommandOutputString;

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