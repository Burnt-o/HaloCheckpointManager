#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

struct AbilityData
{
	float abilityPercent;
	uint32_t abilityCooldown;
	uint32_t abilityMaxCooldown;
};

class IGetAbilityData
{
public:
	virtual std::expected<AbilityData, pHCMError> getAbilityData(bool heroAssist) = 0;
	virtual ~IGetAbilityData() = default;
};

class GetAbilityData : public IOptionalCheat
{
private:
	std::unique_ptr<IGetAbilityData> pimpl;

public:
	GetAbilityData(GameState game, IDIContainer& dicon);
	~GetAbilityData();

	// if (heroAssist && game is h4), will return heroAssist ability data instead of regular equipment data
	std::expected<AbilityData, pHCMError> getAbilityData(bool heroAssist) { return pimpl->getAbilityData(heroAssist); }

	virtual std::string_view getName() override { return nameof(GetAbilityData); }
};

