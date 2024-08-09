#pragma once
#include "pch.h"
#include "ScopedCallback.h"
#include "SharedRequestToken.h"

// A wrapper for eventpp callbacks. Whenever a client makes or destroys a callback to an event, the wrapper fires an onCallbackListChanged event.
// Scoped: callback unsubscribes from event in destructor.

template <typename ret, typename... args>
class ObservedEvent;

template <typename ret, typename... args>
class ObservedEvent< eventpp::CallbackList<ret(args...)>>;

template <typename ret, typename... args>
class ObservedScopedCallback;

template <typename ret, typename... args>
class ObservedScopedCallback<eventpp::CallbackList<ret(args...)>> : ScopedCallback<eventpp::CallbackList<ret(args...)>>
{
private:
	std::shared_ptr<SharedRequestToken> mToken;


	friend class ObservedEvent<eventpp::CallbackList<ret(args...) >>; // can only be constructed by ObservedEvents
	explicit ObservedScopedCallback(std::shared_ptr <eventpp::CallbackList<ret(args...)>> pEvent, std::function<ret(args...)> functor, std::shared_ptr<SharedRequestToken> token)
		: ScopedCallback<eventpp::CallbackList<ret(args...)>>(pEvent, functor), mToken(token)
	{
	}


public:
	~ObservedScopedCallback()
	{
		// unsubscribe the scoped callback first so the callbackList->isEmpty is accurate!
		this->removeCallback();

		if (mToken)
			mToken.reset();
	}


	void removeObservedCallback()
	{
		this->removeCallback();

		if (mToken)
			mToken.reset();
	}

	// copy is banned
	ObservedScopedCallback(const ObservedScopedCallback& that) = delete;
	ObservedScopedCallback& operator=(const ObservedScopedCallback& that) = delete;

	// move is fine
	ObservedScopedCallback(ObservedScopedCallback&& that) = default;
	ObservedScopedCallback& operator=(ObservedScopedCallback&& that) = default;
};
