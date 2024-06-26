#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class IGetScenarioAddressImpl
{
public:
	virtual ~IGetScenarioAddressImpl() = default;
	virtual std::expected<uintptr_t, HCMRuntimeException> getScenarioAddress() = 0;
};

class GetScenarioAddress : public IOptionalCheat
{
private:
	std::unique_ptr<IGetScenarioAddressImpl> pimpl;

public:
	GetScenarioAddress(GameState gameImpl, IDIContainer& dicon);
	~GetScenarioAddress();

	std::expected<uintptr_t, HCMRuntimeException> getScenarioAddress();


	std::string_view getName() override { return nameof(GetScenarioAddress); }

};