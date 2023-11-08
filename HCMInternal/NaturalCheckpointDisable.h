#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class NaturalCheckpointDisable : public IOptionalCheat
{
private:
	class NaturalCheckpointDisableImpl;
	std::unique_ptr<NaturalCheckpointDisableImpl> pimpl;

public:
	NaturalCheckpointDisable(GameState gameImpl, IDIContainer& dicon);
	~NaturalCheckpointDisable();

	std::string_view getName() override { return nameof(NaturalCheckpointDisable); }

};