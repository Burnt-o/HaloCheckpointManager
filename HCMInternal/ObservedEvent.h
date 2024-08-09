#pragma once
#include "pch.h"
#include "ObservedScopedCallback.h"
#include "SharedRequestProvider.h"
// A wrapper for eventpp events. Whenever a client makes or destroys a callback to an event, the wrapper fires an onCallbackListChanged event.



template <typename ret, typename... args>
class ObservedEvent;

template <typename ret, typename... args>
class ObservedEvent< eventpp::CallbackList<ret(args...)>> : TokenSharedRequestProvider
{
private:
	std::unique_ptr<ScopedCallback<ActionEvent>> initOnCallbackListChanged;

	std::shared_ptr<eventpp::CallbackList<ret(args...)>> mEvent = std::make_shared< eventpp::CallbackList<ret(args...)>>();
	std::shared_ptr<ActionEvent> callbackListChangedEvent = std::make_shared<ActionEvent>();


	virtual void updateService() override 
	{
		callbackListChangedEvent->operator()();
	}

public:
	explicit ObservedEvent() {}

	explicit ObservedEvent(std::function<void()> onCallbackListChanged)
	{
		initOnCallbackListChanged = std::make_unique<ScopedCallback<ActionEvent>>(callbackListChangedEvent, onCallbackListChanged);
	}

	std::unique_ptr<ScopedCallback<eventpp::CallbackList<ret(args...)>>> subscribe(std::function<ret(args...)> functor)
	{
		// create the callback before firing so observer can get accurate count of how many subscribers there are
		std::unique_ptr<ScopedCallback<eventpp::CallbackList<ret(args...)>>> out(
			new ObservedScopedCallback<eventpp::CallbackList<ret(args...)>>(mEvent, functor, this->makeScopedRequest()) // have to create object in place because std::make_unique doesn't have access to the private constructor
		);
		return std::move(out);
	}

	bool isEventSubscribed() const {
		return this->serviceIsRequested(); }

	std::shared_ptr<ActionEvent> getCallbackListChangedEvent() {
		return callbackListChangedEvent;
	}

	ret fireEvent(args... a) { return mEvent->operator()(a...); }

	// copy is banned
	ObservedEvent(const ObservedEvent& that) = delete;
	ObservedEvent& operator=(const ObservedEvent& that) = delete;

	// move is banned
	ObservedEvent(ObservedEvent&& that) = delete;
	ObservedEvent& operator=(ObservedEvent&& that) = delete;
};
