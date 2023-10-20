#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "pch.h"
#include "DifficultyEnum.h"

class GetCurrentDifficulty : public IOptionalCheat
{
private:
	class GetCurrentDifficultyImpl;
	std::unique_ptr<GetCurrentDifficultyImpl> pimpl;

public:
	GetCurrentDifficulty(GameState game, IDIContainer& dicon);
	~GetCurrentDifficulty();

	DifficultyEnum getCurrentDifficulty();

	virtual std::string_view getName() override { return nameof(GetCurrentDifficulty); }
};

