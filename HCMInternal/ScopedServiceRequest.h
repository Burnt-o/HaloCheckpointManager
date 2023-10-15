#pragma once
#include "pch.h"

class IProvideScopedRequests
{
public:
	virtual void requestService(std::string callerID) = 0;
	virtual void unrequestService(std::string callerID) = 0;
	virtual ~IProvideScopedRequests() = default;
};

class ScopedServiceRequest
{
private:
	gsl::not_null<std::shared_ptr<IProvideScopedRequests>> mService;
	std::string mCallerID;
public:
	ScopedServiceRequest(std::shared_ptr<IProvideScopedRequests> service, std::string callerID)
		: mService(service), mCallerID(callerID)
	{
		PLOG_DEBUG << "creating ScopedServiceRequest";
		mService->requestService(mCallerID);;
	}

	~ScopedServiceRequest()
	{
		PLOG_DEBUG << "destroying ScopedServiceRequest";
		mService->unrequestService(mCallerID);
	}
};