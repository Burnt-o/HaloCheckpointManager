#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"
class GetPlayerDatum : public IOptionalCheat
{
public:
	class IGetPlayerDatumImpl {
	public:
		virtual Datum getPlayerDatum() = 0;
	};

private:
	std::unique_ptr<IGetPlayerDatumImpl> pimpl;

public:
	GetPlayerDatum(GameState game, IDIContainer& dicon);
	~GetPlayerDatum();

	Datum getPlayerDatum() { return pimpl->getPlayerDatum(); }

	virtual std::string_view getName() override { return nameof(GetPlayerDatum); }
};

