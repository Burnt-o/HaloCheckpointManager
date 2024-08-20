#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class ChangeOOBBackground : public IOptionalCheat
{
private:
	class ChangeOOBBackgroundImpl;
	std::unique_ptr<ChangeOOBBackgroundImpl> pimpl;

public:
	ChangeOOBBackground(GameState gameImpl, IDIContainer& dicon);
	~ChangeOOBBackground();
	std::string_view getName() override { return nameof(ChangeOOBBackground); }
};