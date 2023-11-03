#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"

class IGetObjectPhysics {
public:
	virtual const SimpleMath::Vector3* getObjectPosition(Datum datum) = 0;
	virtual std::pair<SimpleMath::Vector3*, SimpleMath::Vector3*> getObjectPositionMutableAndVisual(Datum datum) = 0;
	virtual const SimpleMath::Vector3* getObjectVelocity(Datum datum) = 0;
	virtual SimpleMath::Vector3* getObjectVelocityMutable(Datum datum) = 0;
	virtual ~IGetObjectPhysics() = default;
};


// Note: this class promotes bipeds to vehicles.
// That is to say, if you pass it a bipeds datum, and that biped is inside a vehicle,
// then it will act on the vehicle instead.

class GetObjectPhysics : public IGetObjectPhysics, public IOptionalCheat
{

private:
	std::unique_ptr<IGetObjectPhysics> pimpl;

public:
	GetObjectPhysics(GameState game, IDIContainer& dicon);
	~GetObjectPhysics();

	virtual const SimpleMath::Vector3* getObjectPosition(Datum datum) override;
	virtual std::pair<SimpleMath::Vector3*, SimpleMath::Vector3*> getObjectPositionMutableAndVisual(Datum datum) override;
	virtual const SimpleMath::Vector3* getObjectVelocity(Datum datum) override;
	virtual SimpleMath::Vector3* getObjectVelocityMutable(Datum datum) override;

	virtual std::string_view getName() override { return nameof(GetObjectPhysics); }
};

