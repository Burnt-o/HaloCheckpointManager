#pragma once
#include "pch.h"
#include "EngineCommand.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "ModuleHook.h"

template <GameState::Value gameT>
class EngineCommandImpl : public IEngineCommand
{
private:

	//data
	std::shared_ptr<MultilevelPointer> gameEnginePointer;
	std::shared_ptr<MultilevelPointer> sendCommandPointer;


public:

	EngineCommandImpl(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		gameEnginePointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEnginePointer));
		sendCommandPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(sendCommandPointer), game);
	}


	void sendEngineCommand(std::string commandString)
	{

		uintptr_t pCommand;
		if (!sendCommandPointer->resolve(&pCommand)) throw HCMRuntimeException("Could not resolve pointer to sendCommand function");

		uintptr_t pEngine;
		if (!gameEnginePointer->resolve(&pEngine)) throw HCMRuntimeException("Could not resolve pointer to gameEngine");

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pCommand: 0x" << std::hex << p, p = pCommand);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pEngine: 0x" << std::hex << p, p = pEngine);

		typedef void(*EngineCommand_t)(uintptr_t pEngine, const char* commandString);
		EngineCommand_t engine_command_vptr;
		engine_command_vptr = static_cast<EngineCommand_t>((void*)pCommand);

		std::string command = "HS: " + commandString;

		// Execute the command
		engine_command_vptr(pEngine, command.c_str());

	}


};



EngineCommand::EngineCommand(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<EngineCommandImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<EngineCommandImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<EngineCommandImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<EngineCommandImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<EngineCommandImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<EngineCommandImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

EngineCommand::~EngineCommand() = default;