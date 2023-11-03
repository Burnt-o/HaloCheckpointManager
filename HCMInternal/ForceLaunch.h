#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

// Note: this class promotes bipeds to vehicles.
// That is to say, if you pass it a bipeds datum, and that biped is inside a vehicle,
// then it will act on the vehicle instead.

class ForceLaunchImplUntemplated { public: virtual ~ForceLaunchImplUntemplated() = default; };
class ForceLaunch : public IOptionalCheat
{
private:
	std::unique_ptr<ForceLaunchImplUntemplated> pimpl;

public:
	ForceLaunch(GameState gameImpl, IDIContainer& dicon);
	~ForceLaunch();

	virtual std::string_view getName() override { return nameof(ForceLaunch); }
};

