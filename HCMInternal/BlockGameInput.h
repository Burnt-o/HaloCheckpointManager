#pragma once
#include "SharedRequestProvider.h"
#include "PointerDataStore.h"

class BlockGameInput 
{
private:
	std::shared_ptr<TokenSharedRequestProvider> pimpl;
public:

	BlockGameInput(std::shared_ptr<PointerDataStore> ptr);
	~BlockGameInput();


	std::shared_ptr<SharedRequestToken> makeScopedRequest() {
		return pimpl->makeScopedRequest();
	}
};