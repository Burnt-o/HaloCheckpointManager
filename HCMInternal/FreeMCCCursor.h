#pragma once
#include "MultilevelPointer.h"
#include "MidhookFlagInterpreter.h".


class IFreeMCCCursorImpl
{
public:
	virtual void requestFreedCursor(std::string callerID) = 0;
	virtual void unrequestFreedCursor(std::string callerID) = 0;
	virtual ~IFreeMCCCursorImpl() = default;
};

class ScopedFreeCursorRequest
{
private:
	gsl::not_null<std::shared_ptr<IFreeMCCCursorImpl>> freeCursorImpl;
	std::string mCallerID;
public:
	ScopedFreeCursorRequest(std::shared_ptr<IFreeMCCCursorImpl> pimpl, std::string callerID)
		: freeCursorImpl(pimpl), mCallerID(callerID)
	{
		PLOG_DEBUG << "creating ScopedFreeCursorRequest";
		freeCursorImpl->requestFreedCursor(mCallerID);;
	}

	~ScopedFreeCursorRequest()
	{
		PLOG_DEBUG << "destroying ScopedFreeCursorRequest";
		freeCursorImpl->unrequestFreedCursor(mCallerID);
	}
};


class FreeMCCCursor
{
public:
	class FreeMCCCursorImpl;
private:
	std::shared_ptr< FreeMCCCursorImpl> pimpl;
public:

	FreeMCCCursor(std::shared_ptr<MultilevelPointer> freeCursorFunc, std::shared_ptr<MidhookFlagInterpreter> flagSetter);
	~FreeMCCCursor();


	ScopedFreeCursorRequest scopedRequest(std::string callerID);

};

