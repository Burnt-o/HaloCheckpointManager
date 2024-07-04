#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

// fires an event when the game loads a new bsp, passing the index of the new bsp
// only applicable to h1/h2, use BSPSetChangeHookEvent for third gen
class BSPChangeHookEvent : public IOptionalCheat
{
private:
	class BSPChangeHookEventImpl;
	std::unique_ptr<BSPChangeHookEventImpl> pimpl;

public:
	BSPChangeHookEvent(GameState game, IDIContainer& dicon);
	~BSPChangeHookEvent();

	std::shared_ptr<eventpp::CallbackList<void(uint32_t)>> getBSPChangeEvent();

	virtual std::string_view getName() override { return nameof(BSPChangeHookEvent); }
};

