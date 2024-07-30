#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedRequestProvider.h"



class UpdateTriggerLastChecked : public IOptionalCheat
{
private:

	std::shared_ptr<TokenScopedServiceProvider> pimpl;

public:
	UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon);
	~UpdateTriggerLastChecked();

	std::shared_ptr<ScopedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	}

	std::string_view getName() override { return nameof(UpdateTriggerLastChecked); }

};