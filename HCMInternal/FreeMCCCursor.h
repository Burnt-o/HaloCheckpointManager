#pragma once
#include "ScopedRequestProvider.h"
#include "PointerDataStore.h"
class FreeMCCCursor
{
private:
	std::shared_ptr< TokenScopedServiceProvider> pimpl;
public:

	FreeMCCCursor(std::shared_ptr<PointerDataStore> ptr);
	~FreeMCCCursor();


	std::shared_ptr<ScopedRequestToken> makeScopedRequest() { return pimpl->makeScopedRequest(); }

};

