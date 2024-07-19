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

	virtual ~ScopedServiceRequest()
	{
		if (init)
		{
		PLOG_DEBUG << "destroying ScopedServiceRequest";
		mService->unrequestService(mCallerID);
		}

	}
};




class GenericScopedServiceProvider : public IProvideScopedRequests, public std::enable_shared_from_this<GenericScopedServiceProvider>
{
private:
	std::set<std::string> callersRequestingService{};
	virtual void requestService(std::string callerID) override
	{
		callersRequestingService.emplace(callerID);
		updateService();
	}

	virtual void unrequestService(std::string callerID) override
	{
		callersRequestingService.erase(callerID);
		updateService();
	}


public:

	std::unique_ptr<ScopedServiceRequest> makeRequest(std::string callerID)
	{
		PLOG_DEBUG << "making GenericScopedServiceProvider request";
		return std::make_unique<ScopedServiceRequest>(shared_from_this(), callerID);
	}

	bool serviceIsRequested()
	{
		return !callersRequestingService.empty();
	}

	virtual void updateService() {} // function fired when callersRequestingService set changes
	virtual ~GenericScopedServiceProvider() = default;

};

template<class ScopedRequestDerivedType> requires std::derived_from<ScopedRequestDerivedType, ScopedServiceRequest>
class TemplatedScopedServiceProvider : public IProvideScopedRequests, public std::enable_shared_from_this<TemplatedScopedServiceProvider<ScopedRequestDerivedType>>
{
private:
	std::set<std::string> callersRequestingService{};
	virtual void requestService(std::string callerID) override
	{
		callersRequestingService.emplace(callerID);
		updateService();
	}

	virtual void unrequestService(std::string callerID) override
	{
		callersRequestingService.erase(callerID);
		updateService();
	}


public:

	virtual std::unique_ptr<ScopedRequestDerivedType> makeRequest(std::string callerID) = 0;

	bool serviceIsRequested()
	{
		return !callersRequestingService.empty();
	}

	virtual void updateService() {} // function fired when callersRequestingService set changes
	virtual ~TemplatedScopedServiceProvider() = default;

};