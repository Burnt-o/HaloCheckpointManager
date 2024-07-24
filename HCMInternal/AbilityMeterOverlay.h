#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

class AbilityMeterOverlayUntemplated { public: virtual ~AbilityMeterOverlayUntemplated() = default; };
class AbilityMeterOverlay : public IOptionalCheat
{
private:

std::unique_ptr<AbilityMeterOverlayUntemplated> pimpl;

public:
AbilityMeterOverlay(GameState gameImpl, IDIContainer& dicon);
~AbilityMeterOverlay();

std::string_view getName() override { return nameof(AbilityMeterOverlay); }

};