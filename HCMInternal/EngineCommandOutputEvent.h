#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ObservedEvent.h"


using EngineOutputEvent = eventpp::CallbackList<void(const std::string&)>;

class EngineCommandOutputEventImpl {
public:
	virtual ~EngineCommandOutputEventImpl() = default;
	virtual std::shared_ptr<ObservedEvent<EngineOutputEvent>> getEngineCommandOutputEvent() = 0;
};
class EngineCommandOutputEvent : public IOptionalCheat
{
private:

	std::unique_ptr<EngineCommandOutputEventImpl> pimpl;
public:
	EngineCommandOutputEvent(GameState game, IDIContainer& dicon);
	~EngineCommandOutputEvent();

	std::shared_ptr<ObservedEvent<EngineOutputEvent>> getEngineCommandOutputEvent() { return pimpl->getEngineCommandOutputEvent(); }

	virtual std::string_view getName() override { return nameof(EngineCommandOutputEvent); }
};

