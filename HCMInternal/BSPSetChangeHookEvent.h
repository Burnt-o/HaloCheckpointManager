#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "BSPSet.h"

class IBSPSetChangeHookEvent
{
public:
	virtual std::shared_ptr<eventpp::CallbackList<void(BSPSet)>> getBSPSetChangeEvent() = 0;
	virtual ~IBSPSetChangeHookEvent() = default;
};

// fires an event when the game loads a new bsp, passing the bitmask (BSPSet) of the new loaded bsps
// only applicable to third gen. Only impl for h3/od so far
class BSPSetChangeHookEvent : public IOptionalCheat
{
private:
	std::unique_ptr<IBSPSetChangeHookEvent> pimpl;

public:
	BSPSetChangeHookEvent(GameState game, IDIContainer& dicon);
	~BSPSetChangeHookEvent();

	std::shared_ptr<eventpp::CallbackList<void(BSPSet)>> getBSPSetChangeEvent() { return pimpl->getBSPSetChangeEvent(); }

	virtual std::string_view getName() override { return nameof(BSPSetChangeHookEvent); }
};

