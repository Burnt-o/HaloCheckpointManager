#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "GameInputEnum.h"
#include "ScopedServiceRequest.h"

class IScopedGameInputter {
public:
	virtual ~IScopedGameInputter() = default;
	virtual void setQueuedInput(const GameInputEnum& input, int ticksToPress) = 0;
	virtual void setCurrentInput(const GameInputEnum& input, bool value) = 0;
	virtual void advanceInputQueue(int ticksToAdvance) = 0;
	virtual void clearInputQueue() = 0;
};

class GameInputterManagerImpl;
class GameInputterManager : public IOptionalCheat
{
private:
	// shared since ScopedGameInputterImpl will need to keep shared ptr to it
	std::shared_ptr<GameInputterManagerImpl> pimpl;


public:
	GameInputterManager(GameState gameImpl, IDIContainer& dicon);

	~GameInputterManager();

	std::unique_ptr<IScopedGameInputter> getScopedGameInputter(std::string callerID);

	std::string_view getName() override { return nameof(GameInputterManager); }



};