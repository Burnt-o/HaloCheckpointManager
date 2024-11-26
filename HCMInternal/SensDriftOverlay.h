#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

class SensDriftOverlayUntemplated { public: virtual ~SensDriftOverlayUntemplated() = default; };
class SensDriftOverlay : public IOptionalCheat
{
private:

	std::unique_ptr<SensDriftOverlayUntemplated> pimpl;

public:
	SensDriftOverlay(GameState gameImpl, IDIContainer& dicon);
	~SensDriftOverlay();

	std::string_view getName() override { return nameof(SensDriftOverlay); }

};