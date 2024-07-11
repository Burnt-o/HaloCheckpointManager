#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class IConsoleCommand
{
public:
	virtual ~IConsoleCommand() = default;
	virtual std::optional<std::string> sendCommand(std::string commandString) = 0;
};


class ConsoleCommand : public IOptionalCheat
{
private:
	
	std::unique_ptr<IConsoleCommand> pimpl;

public:

	ConsoleCommand(GameState gameImpl, IDIContainer& dicon);
	~ConsoleCommand();

	virtual std::string_view getName() override {
		return nameof(ConsoleCommand);
	}


	std::optional<std::string> sendCommand(std::string commandString) { return pimpl->sendCommand(commandString); }


};
