#pragma once
#include "MultilevelPointer.h"
#include "MidhookFlagInterpreter.h".
#include "ScopedServiceRequest.h"

class FreeMCCCursor
{
public:
	class FreeMCCCursorImpl;
private:
	std::shared_ptr< FreeMCCCursorImpl> pimpl;
public:

	FreeMCCCursor(std::shared_ptr<MultilevelPointer> freeCursorFunc, std::shared_ptr<MidhookFlagInterpreter> flagSetter);
	~FreeMCCCursor();


	ScopedServiceRequest scopedRequest(std::string callerID);

};

