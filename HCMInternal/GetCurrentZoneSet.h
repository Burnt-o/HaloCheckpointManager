#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

// where CurrentBSPSet returns a mask of loaded BSP indexes, CurrentZoneSet instead returns an index to a zone set (as described in scnr tag)
// only impl for hreach and h4 so far
class GetCurrentZoneSet : public IOptionalCheat
{
private:
	class GetCurrentZoneSetImpl;
	std::unique_ptr<GetCurrentZoneSetImpl> pimpl;

public:
	GetCurrentZoneSet(GameState game, IDIContainer& dicon);
	~GetCurrentZoneSet();

	uint32_t getCurrentZoneSet();

	virtual std::string_view getName() override { return nameof(GetCurrentZoneSet); }
};

