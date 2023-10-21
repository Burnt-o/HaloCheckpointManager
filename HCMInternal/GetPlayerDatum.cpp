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
		playerDatumPointer = dicon.Resolve< PointerManager>().lock()->getData<std::shared_ptr<MultilevelPointer>>("playerDatum", mGame);
	}

	virtual Datum getPlayerDatum() override
	{
		Datum playerDatum; // inits to null datum
		if (!playerDatumPointer->readData(&playerDatum)) PLOG_ERROR << "Failed to read playerDatum: " << MultilevelPointer::GetLastError(); 
		return playerDatum;
	}
};

GetPlayerDatum::GetPlayerDatum(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
	case GameState::Value::Halo2:
	case GameState::Value::Halo3:
	case GameState::Value::Halo3ODST:
	case GameState::Value::HaloReach:
	case GameState::Value::Halo4:
		pimpl = std::make_unique<SimpleGetPlayerDatum>(game, dicon);
		return;



	default:
		throw HCMInitException("GetPlayerDatum not impl for this game yet");
	}
}

GetPlayerDatum::~GetPlayerDatum() = default;