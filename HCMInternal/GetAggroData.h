#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

struct AggroData
{
	uint16_t aggroLevel; 
	uint16_t aggroDecayTimer;
	bool playerHasAggro;
};

// todo (cambid has some of the s8 pointers already, check bttz discord)
class GetAggroData : public IOptionalCheat
{
private:
	class GetAggroDataImpl;
	std::unique_ptr<GetAggroDataImpl> pimpl;

public:
	GetAggroData(GameState gameImpl, IDIContainer& dicon);
	~GetAggroData();

	AggroData getAggroData();

	std::string_view getName() override { return nameof(GetAggroData); }

};