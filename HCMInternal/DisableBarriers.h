#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

class DisableBarriers : public IOptionalCheat
{
private:
	class DisableBarriersImpl;
	std::unique_ptr<DisableBarriersImpl> pimpl;

public:
	DisableBarriers(GameState game, IDIContainer& dicon);
	~DisableBarriers();

	virtual std::string_view getName() override { return nameof(DisableBarriers); }
};