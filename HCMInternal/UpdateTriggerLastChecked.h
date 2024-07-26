#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedServiceRequest.h"



class UpdateTriggerLastChecked : public IOptionalCheat
{
private:

	std::shared_ptr<GenericScopedServiceProvider> pimpl;

public:
	UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon);
	~UpdateTriggerLastChecked();

	std::unique_ptr<ScopedServiceRequest> makeRequest(std::string callerID) {
		return pimpl->makeRequest(callerID);
	}

	std::string_view getName() override { return nameof(UpdateTriggerLastChecked); }

};