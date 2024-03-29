#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class TriggerOverlayUntemplated { public: virtual ~TriggerOverlayUntemplated() = default; };
class TriggerOverlay : public IOptionalCheat
{
private:

	std::unique_ptr<TriggerOverlayUntemplated> pimpl;

public:
	TriggerOverlay(GameState gameImpl, IDIContainer& dicon);
	~TriggerOverlay();

	std::string_view getName() override { return nameof(TriggerOverlay); }

};