#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"



class GetCurrentRNG : public IOptionalCheat
{
private:
	class GetCurrentRNGImpl;
	std::unique_ptr<GetCurrentRNGImpl> pimpl;

public:
	GetCurrentRNG(GameState game, IDIContainer& dicon);
	~GetCurrentRNG();

	DWORD getCurrentRNG();

	virtual std::string_view getName() override { return nameof(GetCurrentRNG); }
};

