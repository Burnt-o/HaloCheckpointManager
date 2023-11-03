#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

// Note: this class promotes bipeds to vehicles.
// That is to say, if you pass it a bipeds datum, and that biped is inside a vehicle,
// then it will act on the vehicle instead.

class ForceTeleportImplUntemplated { public: virtual ~ForceTeleportImplUntemplated() = default; };
class ForceTeleport : public IOptionalCheat
{
private:
	std::unique_ptr<ForceTeleportImplUntemplated> pimpl;

public:
	ForceTeleport(GameState gameImpl, IDIContainer& dicon);
	~ForceTeleport();

	virtual std::string_view getName() override { return nameof(ForceTeleport); }
};

