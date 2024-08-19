#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SharedRequestProvider.h"

// enables 3rd person rendering of the players model, and hides game UI


class ThirdPersonRendering : public IOptionalCheat
{
private:
	std::unique_ptr<TokenSharedRequestProvider> pimpl;


public:
	ThirdPersonRendering(GameState gameImpl, IDIContainer& dicon);

	~ThirdPersonRendering();

	std::string_view getName() override { return nameof(ThirdPersonRendering); }

	// engages third person rendering while held 
	std::shared_ptr<SharedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	} 

};