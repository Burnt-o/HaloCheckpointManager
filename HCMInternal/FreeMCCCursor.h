#pragma once
#include "ScopedServiceRequest.h"
#include "PointerDataStore.h"
class FreeMCCCursor
{
public:
	class FreeMCCCursorImpl;
private:
	std::shared_ptr< FreeMCCCursorImpl> pimpl;
public:

	FreeMCCCursor(std::shared_ptr<PointerDataStore> ptr);
	~FreeMCCCursor();


	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID);

};

