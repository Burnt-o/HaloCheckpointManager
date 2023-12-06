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

	virtual const SimpleMath::Vector2 getPlayerViewAngle() override
	{
		SimpleMath::Vector2 outAngles; // inits to null datum
		if (!playerViewAngle->readData(&outAngles)) throw HCMRuntimeException(std::format("Failed to read playerViewAngle: {}", MultilevelPointer::GetLastError()));
		return outAngles;
	}

	virtual void setPlayerViewAngle(SimpleMath::Vector2 newAngles) override
	{
		if (!playerViewAngle->writeData(&newAngles)) throw HCMRuntimeException(std::format("Failed to write playerViewAngle: {}", MultilevelPointer::GetLastError()));
	}
};

GetPlayerViewAngle::GetPlayerViewAngle(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetPlayerViewAngleImpl>(game, dicon);
}

GetPlayerViewAngle::~GetPlayerViewAngle() = default;