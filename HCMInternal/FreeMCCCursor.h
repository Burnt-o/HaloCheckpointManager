#pragma once
#include "ScopedServiceRequest.h"
#include "PointerManager.h"
class FreeMCCCursor
{
public:
	class FreeMCCCursorImpl;
private:
	std::shared_ptr< FreeMCCCursorImpl> pimpl;
public:

	FreeMCCCursor(std::shared_ptr<PointerManager> ptr);
	~FreeMCCCursor();


	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID);

};

