#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "pch.h"

class GetCurrentLevelCode : public IOptionalCheat
{
private:
	class GetCurrentLevelCodeImpl;
	std::unique_ptr<GetCurrentLevelCodeImpl> pimpl;

public:
	GetCurrentLevelCode(GameState game, IDIContainer& dicon);
	~GetCurrentLevelCode();

	std::string getCurrentLevelCode();

	virtual std::string_view getName() override { return nameof(GetCurrentLevelCode); }
};

