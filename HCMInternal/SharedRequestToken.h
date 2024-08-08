#pragma once
#include "pch.h"


// this is a token that while held (shared_ptr to it has positive reference count), indicates a service is in use/requested. 
class SharedRequestToken
{
protected:
	friend class TokenSharedRequestProvider;
	explicit SharedRequestToken()
	{
#ifdef HCM_DEBUG
		PLOG_DEBUG << "Creating SharedRequestToken()";
#endif
	}
public:

	virtual ~SharedRequestToken()
	{
#ifdef HCM_DEBUG
		PLOG_DEBUG << "Destroying ~SharedRequestToken()";
#endif
	}

	// no copy or move
	SharedRequestToken(SharedRequestToken const&) = delete; // copy constructor
	SharedRequestToken& operator = (SharedRequestToken const&) = delete; // copy assignment operator
	SharedRequestToken(SharedRequestToken&&) = delete; // move constructor
	SharedRequestToken& operator = (SharedRequestToken&&) = delete; // move assignment operator
};




