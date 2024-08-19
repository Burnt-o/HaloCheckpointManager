#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SharedRequestProvider.h"


class DisableScreenEffects : public IOptionalCheat
{
private:
	std::unique_ptr<TokenSharedRequestProvider> pimpl;


public:
	DisableScreenEffects(GameState gameImpl, IDIContainer& dicon);

	~DisableScreenEffects();

	std::string_view getName() override { return nameof(DisableScreenEffects); }

	// disables screen effects while held 
	std::shared_ptr<SharedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	} 

};