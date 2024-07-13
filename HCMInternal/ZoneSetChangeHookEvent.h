#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "ObservedEvent.h"

class IZoneSetChangeHookEvent
{
public:
	virtual std::shared_ptr<ObservedEvent<eventpp::CallbackList<void(uint32_t)>>> getZoneSetChangeEvent() = 0;
	virtual ~IZoneSetChangeHookEvent() = default;
};

// fires an event when the game loads a new zoneset, passing the index of the new zoneset
// only impl for hreach and h4 so far
class ZoneSetChangeHookEvent : public IOptionalCheat
{
private:
	std::unique_ptr<IZoneSetChangeHookEvent> pimpl;

public:
	ZoneSetChangeHookEvent(GameState game, IDIContainer& dicon);
	~ZoneSetChangeHookEvent();

	std::shared_ptr<ObservedEvent<eventpp::CallbackList<void(uint32_t)>>> getZoneSetChangeEvent() { return pimpl->getZoneSetChangeEvent(); }

	virtual std::string_view getName() override { return nameof(ZoneSetChangeHookEvent); }
};

