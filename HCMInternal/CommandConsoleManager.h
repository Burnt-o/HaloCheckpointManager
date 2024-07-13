#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

// renders gui for command console, sends inputs to EngineCommand

class CommandConsoleManager : public IOptionalCheat
{
private:
	class CommandConsoleManagerImpl;
	std::unique_ptr<CommandConsoleManagerImpl> pimpl;

public:

	CommandConsoleManager(GameState gameImpl, IDIContainer& dicon);
	~CommandConsoleManager();

	virtual std::string_view getName() override {
		return nameof(CommandConsoleManager);
	}



};

