#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"

// Note: this class promotes bipeds to vehicles.
// That is to say, if you pass it a bipeds datum, and that biped is inside a vehicle,
// then it will act on the vehicle instead.

class UnfreezeObjectPhysics : public IOptionalCheat
{
private:
	class UnfreezeObjectPhysicsImpl;
	std::unique_ptr<UnfreezeObjectPhysicsImpl> pimpl;

public:
	virtual std::string_view getName() override { return nameof(UnfreezeObjectPhysics); }

	UnfreezeObjectPhysics(GameState gameImpl, IDIContainer& dicon);
	~UnfreezeObjectPhysics();

	void unfreezeObjectPhysics(Datum datumToUnfreeze);
};