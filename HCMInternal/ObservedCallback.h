#pragma once
#include "pch.h"
#include "ScopedCallback.h"
#include "IFireCallbackListChanged.h"
// A wrapper for eventpp callbacks. Whenever a client makes or destroys a callback to an event, the wrapper fires an onCallbackListChanged event.
// Scoped: callback unsubscribes from event in destructor.


template <typename ret, typename... args>
class ScopedCallback;

template <typename ret, typename... args>
class ScopedCallback<eventpp::CallbackList<ret(args...)>>;

template <typename ret, typename... args>
class ObservedEvent;

template <typename ret, typename... args>
class ObservedEvent< eventpp::CallbackList<ret(args...)>>;

template <typename ret, typename... args>
class ObservedScopedCallback;

template <typename ret, typename... args>
class ObservedScopedCallback<eventpp::CallbackList<ret(args...)>> : public ScopedCallback<eventpp::CallbackList<ret(args...)>>
{
private:
	std::weak_ptr<IFireCallbackListChanged> mObserver;

	friend class ObservedEvent<eventpp::CallbackList<ret(args...) >> ; // only provided by ObservedEvents

	explicit ObservedScopedCallback(std::shared_ptr <eventpp::CallbackList<ret(args...)>> pEvent, std::function<ret(args...)> functor, std::shared_ptr<IFireCallbackListChanged> observer)
		: ScopedCallback<eventpp::CallbackList<ret(args...)>>(pEvent, functor), mObserver(observer)
	{
		PLOG_DEBUG << "ObservedScopedCallback constructor called";
	}


public:
	~ObservedScopedCallback()
	{
		PLOG_DEBUG << "~ObservedScopedCallback destructor called";
		// unsubscribe the scoped callback first so the callbackList->isEmpty is accurate!
		this->removeCallback();
		
		auto observer = mObserver.lock();
		if (observer)
			observer->getCallbackListChangedEvent()->operator()();
		else
			PLOG_ERROR << "null weak ptr to observer on ObservedScopedCallback destruction";
	}

	// copy is banned
	ObservedScopedCallback(const ObservedScopedCallback& that) = delete;
	ObservedScopedCallback& operator=(const ObservedScopedCallback& that) = delete;

	// move is fine
	ObservedScopedCallback(ObservedScopedCallback&& that) = default;
	ObservedScopedCallback& operator=(ObservedScopedCallback&& that) = default;


};