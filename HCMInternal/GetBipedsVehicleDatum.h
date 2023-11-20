#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"

// todo (cambid has some of the s8 pointers already, check bttz discord)
class GetBipedsVehicleDatum : public IOptionalCheat
{
private:
	class GetBipedsVehicleDatumImpl;
	std::unique_ptr<GetBipedsVehicleDatumImpl> pimpl;

public:
	GetBipedsVehicleDatum(GameState gameImpl, IDIContainer& dicon);
	~GetBipedsVehicleDatum();

	Datum getBipedsVehicleDatum(Datum bipedDatum); // will double check that the object is a biped
	Datum getBipedsVehicleDatum(uintptr_t bipedAddress); // you must only call this if you've confirmed its a biped object

	std::string_view getName() override { return nameof(GetBipedsVehicleDatum); }

};