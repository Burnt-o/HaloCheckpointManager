#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"
#include "ObjectTypes.h"


class GetObjectHealth : public IOptionalCheat
{
private:
	class GetObjectHealthImpl;
	std::unique_ptr<GetObjectHealthImpl> pimpl;

public:
	GetObjectHealth(GameState gameImpl, IDIContainer& dicon);
	~GetObjectHealth();

	float getObjectHealth(Datum entityDatum);
	float getObjectHealth(uintptr_t entityAddress, CommonObjectType entityType); // must be biped or vehicle

	float getObjectShields(Datum entityDatum);
	float getObjectShields(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	uint16_t getObjectShieldCooldown(Datum entityDatum);
	uint16_t getObjectShieldCooldown(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	uint16_t getObjectHealthCooldown(Datum entityDatum);
	uint16_t getObjectHealthCooldown(uintptr_t entityAddress, CommonObjectType entityType); // must be biped

	std::string_view getName() override { return nameof(GetObjectHealth); }

};