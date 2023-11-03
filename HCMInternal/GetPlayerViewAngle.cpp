#include "pch.h"
#include "GetPlayerViewAngle.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"


class GetPlayerViewAngleImpl : public GetPlayerViewAngle::IGetPlayerViewAngleImpl
{
private:
	GameState mGame;

	//data
	std::shared_ptr<MultilevelPointer> playerViewAngle;

public:
	GetPlayerViewAngleImpl(GameState game, IDIContainer& dicon) : mGame(game)
	{
		playerViewAngle = dicon.Resolve< PointerManager>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(playerViewAngle), mGame);
	}

	virtual SimpleMath::Vector2 getPlayerViewAngle() override
	{
		SimpleMath::Vector2 outAngles; // inits to null datum
		if (!playerViewAngle->readData(&outAngles)) PLOG_ERROR << "Failed to read playerViewAngle: " << MultilevelPointer::GetLastError();
		return outAngles;
	}
};

GetPlayerViewAngle::GetPlayerViewAngle(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetPlayerViewAngleImpl>(game, dicon);
}

GetPlayerViewAngle::~GetPlayerViewAngle() = default;