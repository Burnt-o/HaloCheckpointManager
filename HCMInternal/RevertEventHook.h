#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "ObservedEvent.h"

// fires an event when the game is reverted. Includes natural reverts, cutscene skips, restart missions, forced reverts, and core loads
class RevertEventHook : public IOptionalCheat
{
public:
	class RevertEventHookImpl
	{
	public:
		virtual ~RevertEventHookImpl() = default;
		virtual std::shared_ptr<ObservedEvent<ActionEvent>> getRevertEvent() = 0;
	};
private:
	std::unique_ptr<RevertEventHookImpl> pimpl;

public:
	RevertEventHook(GameState game, IDIContainer& dicon);
	~RevertEventHook();

	std::shared_ptr<ObservedEvent<ActionEvent>> getRevertEvent();

	virtual std::string_view getName() override { return nameof(RevertEventHook); }
};

