#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SharedRequestProvider.h"



class UpdateTriggerLastChecked : public IOptionalCheat
{
private:

	std::shared_ptr<TokenSharedRequestProvider> pimpl;

public:
	UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon);
	~UpdateTriggerLastChecked();

	std::shared_ptr<SharedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	}

	std::string_view getName() override { return nameof(UpdateTriggerLastChecked); }

};