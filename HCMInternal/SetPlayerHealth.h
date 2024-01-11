#pragma once
#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

class SetPlayerHealth : public IOptionalCheat
{
private:
	class SetPlayerHealthImpl;
	std::unique_ptr<SetPlayerHealthImpl> pimpl;

public:
	SetPlayerHealth(GameState game, IDIContainer& dicon);
	~SetPlayerHealth();

	virtual std::string_view getName() override { return nameof(SetPlayerHealth); }
};