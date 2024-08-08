#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SharedRequestProvider.h"


class FreeCameraFOVOverride : public IOptionalCheat
{
private:
	// shared for shared_from_this
	std::shared_ptr<TokenSharedRequestProvider> pimpl;
public:
	FreeCameraFOVOverride(GameState gameImpl, IDIContainer& dicon);
	~FreeCameraFOVOverride();

	std::string_view getName() override { return nameof(FreeCameraFOVOverride); }

	// overrides the games FOV set-func while held 
	std::shared_ptr<SharedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	} 
};