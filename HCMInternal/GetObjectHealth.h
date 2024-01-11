#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"
#include "ObjectTypes.h"

// For safety, so the user doesn't mix them up
typedef struct { float value; } ObjectHealth;
typedef struct { float value; } ObjectShields;

class GetObjectHealth : public IOptionalCheat
{
private:
	class GetObjectHealthImpl;
	std::unique_ptr<GetObjectHealthImpl> pimpl;

public:
	GetObjectHealth(GameState gameImpl, IDIContainer& dicon);
	~GetObjectHealth();

	const ObjectHealth getObjectHealth(Datum entityDatum);
	const ObjectHealth getObjectHealth(uintptr_t entityAddress, CommonObjectType entityType); // must be biped or vehicle

	const ObjectShields getObjectShields(Datum entityDatum);
	const ObjectShields getObjectShields(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	ObjectHealth* getObjectHealthMutable(Datum entityDatum);
	ObjectHealth* getObjectHealthMutable(uintptr_t entityAddress, CommonObjectType entityType); // must be biped or vehicle

	ObjectShields* getObjectShieldsMutable(Datum entityDatum);
	ObjectShields* getObjectShieldsMutable(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	const uint16_t getObjectShieldCooldown(Datum entityDatum);
	const uint16_t getObjectShieldCooldown(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	const uint16_t getObjectHealthCooldown(Datum entityDatum);
	const uint16_t getObjectHealthCooldown(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	std::string_view getName() override { return nameof(GetObjectHealth); }

};