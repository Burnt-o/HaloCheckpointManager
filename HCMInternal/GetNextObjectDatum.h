#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"

// todo 
class GetNextObjectDatum : public IOptionalCheat
{
private:
	class GetNextObjectDatumImpl;
	std::unique_ptr<GetNextObjectDatumImpl> pimpl;

public:
	GetNextObjectDatum(GameState gameImpl, IDIContainer& dicon);
	~GetNextObjectDatum();

	Datum getNextObjectDatum();

	std::string_view getName() override { return nameof(GetNextObjectDatum); }

};