#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class ITriggerAlphaImpl {
public: virtual ~ITriggerAlphaImpl() = default;

};
// Keeps individual trigger colour settings alpha value synced to the global trigger alpha setting value
class TriggerAlpha : public IOptionalCheat
{

private:
	static inline std::unique_ptr<ITriggerAlphaImpl> pimpl;

public:



	TriggerAlpha(GameState gameImpl, IDIContainer& dicon);
	~TriggerAlpha();

	std::string_view getName() override { return nameof(TriggerAlpha); }

};