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
		virtual SimpleMath::Vector2 getPlayerViewAngle() = 0;
	};

private:
	std::unique_ptr<IGetPlayerViewAngleImpl> pimpl;

public:
	GetPlayerViewAngle(GameState game, IDIContainer& dicon);
	~GetPlayerViewAngle();

	SimpleMath::Vector2 getPlayerViewAngle() { return pimpl->getPlayerViewAngle(); }

	virtual std::string_view getName() override { return nameof(GetPlayerViewAngle); }
};

