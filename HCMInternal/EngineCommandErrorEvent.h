#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ObservedEvent.h"


using EngineErrorEvent = eventpp::CallbackList<void(const std::string&)>;

class EngineCommandErrorEventImpl {
public:
	virtual ~EngineCommandErrorEventImpl() = default;
	virtual std::shared_ptr<ObservedEvent<EngineErrorEvent>> getEngineCommandErrorEvent() = 0;
};
class EngineCommandErrorEvent : public IOptionalCheat
{
private:

	std::unique_ptr<EngineCommandErrorEventImpl> pimpl;
public:
	EngineCommandErrorEvent(GameState game, IDIContainer& dicon);
	~EngineCommandErrorEvent();

	std::shared_ptr<ObservedEvent<EngineErrorEvent>> getEngineCommandErrorEvent() { return pimpl->getEngineCommandErrorEvent(); }

	virtual std::string_view getName() override { return nameof(EngineCommandErrorEvent); }
};

