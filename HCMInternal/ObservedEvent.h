#pragma once
#include "pch.h"
#include "ObservedScopedCallback.h"
#include "IFireCallbackListChanged.h"
// A wrapper for eventpp events. Whenever a client makes or destroys a callback to an event, the wrapper fires an onCallbackListChanged event.



template <typename ret, typename... args>
class ObservedEvent;

template <typename ret, typename... args>
class ObservedEvent< eventpp::CallbackList<ret(args...)>> : public IFireCallbackListChanged, public std::enable_shared_from_this< ObservedEvent< eventpp::CallbackList<ret(args...)>>>
{
private:
	std::shared_ptr<eventpp::CallbackList<ret(args...)>> mEvent = std::make_shared< eventpp::CallbackList<ret(args...)>>();
	std::shared_ptr<ActionEvent> callbackListChangedEvent = std::make_shared< eventpp::CallbackList<void()>>();

	friend class ObservedEventFactory;
	explicit ObservedEvent() {}
	std::unique_ptr<ScopedCallback<ActionEvent>> getCallbackListChangedCallback(std::function<void()> functor)
	{
		return std::make_unique<ScopedCallback<ActionEvent>>(callbackListChangedEvent, functor);
	}
public:

	std::unique_ptr<ScopedCallback<eventpp::CallbackList<ret(args...)>>> subscribe(std::function<ret(args...)> functor)
	{
		// create the callback before firing so observer can get accurate count of how many subscribers there are
		std::unique_ptr<ScopedCallback<eventpp::CallbackList<ret(args...)>>> out ( 
			new ObservedScopedCallback<eventpp::CallbackList<ret(args...)>>(mEvent, functor, this->shared_from_this()) // have to create object in place because std::make_unique doesn't have access to the private constructor
		);
		callbackListChangedEvent->operator()();
		return std::move(out);
	}

	bool isEventSubscribed() const { return !mEvent->empty(); }

	virtual std::shared_ptr<eventpp::CallbackList<void()>> getCallbackListChangedEvent() override {
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