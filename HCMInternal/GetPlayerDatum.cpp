#include "pch.h"
#include "GetPlayerDatum.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
class SimpleGetPlayerDatum : public GetPlayerDatum::IGetPlayerDatumImpl
{
private:
	GameState mGame; 

	//data
	std::shared_ptr<MultilevelPointer> playerDatumPointer;

public:
	SimpleGetPlayerDatum(GameState game, IDIContainer& dicon) : mGame(game)
	{
		playerDatumPointer = dicon.Resolve< PointerManager>()->getData<std::shared_ptr<MultilevelPointer>>("playerDatum", mGame);
	}

	virtual Datum getPlayerDatum() override
	{
		Datum playerDatum;
		if (!playerDatumPointer->readData(&playerDatum)) throw HCMRuntimeException(std::format("Failed to read playerDatum {}", MultilevelPointer::GetLastError()));
		return playerDatum;
	}
};

GetPlayerDatum::GetPlayerDatum(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<SimpleGetPlayerDatum>(game, dicon);
		return;

	default:
		throw HCMInitException("not impl yet");
	}
}

GetPlayerDatum::~GetPlayerDatum() = default;