#pragma once
#include "pch.h"
#include "ScopedRequestToken.h"

// uses shared_ptr custom deleter to fire updateService when the shared_ptr to the token expires. and ofc fires updateService when a new token is requested.
template<class ScopedRequestDerivedType> requires std::derived_from<ScopedRequestDerivedType, ScopedRequestToken>
class ScopedServiceProvider
{
private:
	std::weak_ptr< ScopedRequestDerivedType> mRequest;
	std::function<ScopedRequestDerivedType* ()> mRequestFactory;

public:

	std::shared_ptr<ScopedRequestDerivedType> makeScopedRequest()
	{
		if (mRequest.expired())
		{
			// create new request, fire update event
			auto shared = std::shared_ptr<ScopedRequestDerivedType>(mRequestFactory(),
				[this](ScopedRequestDerivedType* ptr) { delete ptr; updateService(); }); // once last reference expires, updateService is called
			mRequest = shared;
			updateService();
			return shared;
		}
		else
		{
			// return shared ptr to existing request
			return mRequest.lock();
		}
	}

	bool serviceIsRequested() const
	{
		return !mRequest.expired();
	}

	virtual void updateService() {} // function fired when new ScopedRequestToken created or last one expires. override this to act on the new serviceIsRequested().
	virtual ~ScopedServiceProvider() = default;

	ScopedServiceProvider(std::function<ScopedRequestDerivedType* ()> requestFactory) : mRequestFactory(std::move(requestFactory)) {}

};

// no extra functionality, just returns a base ScopedRequestToken.
class TokenScopedServiceProvider : public ScopedServiceProvider<ScopedRequestToken>
{
public:
	TokenScopedServiceProvider() : ScopedServiceProvider<ScopedRequestToken>([]() { return new ScopedRequestToken(); }) {}
};