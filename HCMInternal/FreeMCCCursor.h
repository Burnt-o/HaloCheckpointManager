#pragma once
#include "SharedRequestProvider.h"
#include "PointerDataStore.h"
class FreeMCCCursor
{
private:
	std::shared_ptr< TokenSharedRequestProvider> pimpl;
public:

	FreeMCCCursor(std::shared_ptr<PointerDataStore> ptr);
	~FreeMCCCursor();


	std::shared_ptr<SharedRequestToken> makeScopedRequest() { return pimpl->makeScopedRequest(); }

};

