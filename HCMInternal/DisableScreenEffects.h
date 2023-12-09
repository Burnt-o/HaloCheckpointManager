#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class DisableScreenEffectsImplUntemplated
{
public:
	virtual ~DisableScreenEffectsImplUntemplated() = default;
	virtual void toggleDisableScreenEffects(bool enabled) = 0;
};

class DisableScreenEffects : public IOptionalCheat
{
private:
	std::unique_ptr<DisableScreenEffectsImplUntemplated> pimpl;


public:
	DisableScreenEffects(GameState gameImpl, IDIContainer& dicon);

	~DisableScreenEffects();

	std::string_view getName() override { return nameof(DisableScreenEffects); }

	void toggleDisableScreenEffects(bool enabled); // true means screen effects are disabled

};