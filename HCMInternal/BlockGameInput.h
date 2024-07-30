#pragma once
#include "ScopedRequestProvider.h"
#include "PointerDataStore.h"

class BlockGameInput 
{
private:
	std::shared_ptr<TokenScopedServiceProvider> pimpl;
public:

	BlockGameInput(std::shared_ptr<PointerDataStore> ptr);
	~BlockGameInput();


	std::shared_ptr<ScopedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	}
};