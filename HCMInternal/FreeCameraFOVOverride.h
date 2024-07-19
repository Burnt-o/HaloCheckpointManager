#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedServiceRequest.h"


class FreeCameraFOVOverride : public IOptionalCheat
{
private:
	// shared for shared_from_this
	std::shared_ptr<GenericScopedServiceProvider> pimpl;
public:
	FreeCameraFOVOverride(GameState gameImpl, IDIContainer& dicon);
	~FreeCameraFOVOverride();

	std::string_view getName() override { return nameof(FreeCameraFOVOverride); }

	// overrides the games FOV set-func while held 
	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID) {
		return pimpl->makeRequest(callerID);
	} 
};