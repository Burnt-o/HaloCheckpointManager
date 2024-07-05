#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "BSPSet.h"
#include <bitset>


using BSPSet = std::bitset<32>;

class GetCurrentBSPSet : public IOptionalCheat
{
private:
	class GetCurrentBSPSetImpl;
	std::unique_ptr<GetCurrentBSPSetImpl> pimpl;

public:
	GetCurrentBSPSet(GameState game, IDIContainer& dicon);
	~GetCurrentBSPSet();

	// returns set of BSP indexes currently active
	const BSPSet getCurrentBSPSet();

	// index of the last BSP in this level
	const int getMaxBSPIndex();


	virtual std::string_view getName() override { return nameof(GetCurrentZoneSet); }
};

