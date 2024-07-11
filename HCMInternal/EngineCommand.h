#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class IEngineCommand
{
public:
	virtual ~IEngineCommand() = default;
	virtual std::optional<std::string> sendEngineCommand(std::string commandString) = 0;
};


class EngineCommand : public IOptionalCheat
{
private:
	
	std::unique_ptr<IEngineCommand> pimpl;

public:

	EngineCommand(GameState gameImpl, IDIContainer& dicon);
	~EngineCommand();

	virtual std::string_view getName() override {
		return nameof(EngineCommand);
	}


	std::optional<std::string> sendEngineCommand(std::string commandString) { return pimpl->sendEngineCommand(commandString); }


};
