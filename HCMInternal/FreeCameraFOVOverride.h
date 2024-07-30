#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedRequestProvider.h"


class FreeCameraFOVOverride : public IOptionalCheat
{
private:
	// shared for shared_from_this
	std::shared_ptr<TokenScopedServiceProvider> pimpl;
public:
	FreeCameraFOVOverride(GameState gameImpl, IDIContainer& dicon);
	~FreeCameraFOVOverride();

	std::string_view getName() override { return nameof(FreeCameraFOVOverride); }

	// overrides the games FOV set-func while held 
	std::shared_ptr<ScopedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	} 
};