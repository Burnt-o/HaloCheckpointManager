#pragma once
#include "pch.h"
#include "SharedRequestToken.h"

// uses shared_ptr custom deleter to fire updateService when the shared_ptr to the token expires. and ofc fires updateService when a new token is requested.
template<class SharedRequestDerivedType> requires std::derived_from<SharedRequestDerivedType, SharedRequestToken>
class SharedRequestProvider
{
private:
	std::weak_ptr< SharedRequestDerivedType> mRequest;
	std::function<SharedRequestDerivedType* ()> mRequestFactory;

public:

	std::shared_ptr<SharedRequestDerivedType> makeScopedRequest()
	{

		if (auto lock = mRequest.lock())
		{
			return lock;
		}
		else
		{
			// create new request, fire update event
			auto shared = std::shared_ptr<SharedRequestDerivedType>(mRequestFactory(),
				[this](SharedRequestDerivedType* ptr) { delete ptr; this->updateService(); }); // once last reference expires, updateService is called
			mRequest = shared;
			this->updateService();
			return shared;
		}
	}

	bool serviceIsRequested() const
	{
		return !mRequest.expired();
	}

	virtual void updateService() {} // function fired when new SharedRequestToken created or last one expires. override this to act on the new serviceIsRequested().


	virtual ~SharedRequestProvider()
	{
		// if token outlives provider, the deleter would call a garbade updateService! so wait for token to die first.
		while (mRequest.expired() == false)
		{
			Sleep(1);
		}
	}

	SharedRequestProvider(std::function<SharedRequestDerivedType* ()> requestFactory) : mRequestFactory(std::move(requestFactory)) {}

};

// no extra functionality, just returns a base SharedRequestToken.
class TokenSharedRequestProvider : public SharedRequestProvider<SharedRequestToken>
{
public:
	TokenSharedRequestProvider() : SharedRequestProvider<SharedRequestToken>([]() { return new SharedRequestToken(); }) {}
};