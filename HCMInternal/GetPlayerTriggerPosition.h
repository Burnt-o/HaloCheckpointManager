#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "pch.h"

class GetPlayerTriggerPosition : public IOptionalCheat
{
public:
	class IGetPlayerTriggerPosition
	{
	public:
		virtual ~IGetPlayerTriggerPosition() = default;
		virtual  std::expected<const SimpleMath::Vector3, HCMRuntimeException> getPlayerTriggerPosition() = 0;
	};
private:
	std::unique_ptr<IGetPlayerTriggerPosition> pimpl;

public:
	GetPlayerTriggerPosition(GameState game, IDIContainer& dicon);

	std::expected<const SimpleMath::Vector3, HCMRuntimeException> getPlayerTriggerPosition() { return pimpl->getPlayerTriggerPosition(); }

	virtual std::string_view getName() override { return nameof(GetPlayerTriggerPosition); }
};

