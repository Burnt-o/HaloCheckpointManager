#pragma once
#include "pch.h"


// this is a token that while held (shared_ptr to it has positive reference count), indicates a service is in use/requested. 
class ScopedRequestToken
{
protected:
	friend class TokenScopedServiceProvider;
	explicit ScopedRequestToken()
	{
#ifdef HCM_DEBUG
		PLOG_DEBUG << "Creating ScopedRequestToken()";
#endif
	}
public:

	virtual ~ScopedRequestToken()
	{
#ifdef HCM_DEBUG
		PLOG_DEBUG << "Destroying ~ScopedRequestToken()";
#endif
	}

	// no copy or move
	ScopedRequestToken(ScopedRequestToken const&) = delete; // copy constructor
	ScopedRequestToken& operator = (ScopedRequestToken const&) = delete; // copy assignment operator
	ScopedRequestToken(ScopedRequestToken&&) = delete; // move constructor
	ScopedRequestToken& operator = (ScopedRequestToken&&) = delete; // move assignment operator
};




