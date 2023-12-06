#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

class GetPlayerViewAngle : public IOptionalCheat
{
public:
	class IGetPlayerViewAngleImpl {
	public:
		virtual const SimpleMath::Vector2 getPlayerViewAngle() = 0;
		virtual void setPlayerViewAngle(SimpleMath::Vector2 newAngle) = 0;
	};

private:
	std::unique_ptr<IGetPlayerViewAngleImpl> pimpl;

public:
	GetPlayerViewAngle(GameState game, IDIContainer& dicon);
	~GetPlayerViewAngle();

	const SimpleMath::Vector2 getPlayerViewAngle() { return pimpl->getPlayerViewAngle(); }

	void setPlayerViewAngle(SimpleMath::Vector2 newAngle) { return pimpl->setPlayerViewAngle(newAngle); }

	virtual std::string_view getName() override { return nameof(GetPlayerViewAngle); }
};

