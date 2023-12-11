#pragma once
#include "pch.h"



class IProvideScopedRequests
{
public:
	virtual void requestService(std::string callerID) = 0;
	virtual void unrequestService(std::string callerID) = 0;
	virtual ~IProvideScopedRequests() = default;
};

class GenericScopedServiceProvider : public IProvideScopedRequests
{
private:
	std::set<std::string> callersRequestingService{};
public:
	virtual void requestService(std::string callerID) override
	{
		callersRequestingService.emplace(callerID);
	}

	virtual void unrequestService(std::string callerID) override
	{
		callersRequestingService.erase(callerID);
	}

	bool serviceIsRequested()
	{
		return !callersRequestingService.empty();
	}

};


class ScopedServiceRequest
{
private:
	std::shared_ptr<IProvideScopedRequests> mService;
	std::string mCallerID;
	bool init = false;
public:
	ScopedServiceRequest() = default; // default constructible, but init flag will be left to false

	ScopedServiceRequest(std::shared_ptr<IProvideScopedRequests> service, std::string callerID)
		: mService(service), mCallerID(callerID), init(true)
	{
		PLOG_DEBUG << "creating ScopedServiceRequest";
		mService->requestService(mCallerID);;
	}

	~ScopedServiceRequest()
	{
		if (init)
		{
		PLOG_DEBUG << "destroying ScopedServiceRequest";
		mService->unrequestService(mCallerID);
		}

	}
};