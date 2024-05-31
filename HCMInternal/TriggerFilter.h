#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class TriggerFilterImpl;

class TriggerFilter : public IOptionalCheat
{
private:

	std::unique_ptr<TriggerFilterImpl> pimpl;

public:
	TriggerFilter(GameState gameImpl, IDIContainer& dicon);
	~TriggerFilter();

	std::string_view getName() override { return nameof(TriggerFilter); }

};