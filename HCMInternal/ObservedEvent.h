#pragma once
#include "pch.h"
#include "ObservedCallback.h"
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
		return std::make_unique<ScopedCallback<ActionEvent>>(ScopedCallback<ActionEvent> (callbackListChangedEvent, std::move(functor)));
	}
public:

	std::unique_ptr<ScopedCallback<eventpp::CallbackList<ret(args...)>>> subscribe(std::function<ret(args...)> functor)
	{
		// create the callback before firing so observer can get accurate count of how many subscribers there are
		auto out = std::make_unique<ObservedScopedCallback<eventpp::CallbackList<ret(args...)>>>(
			ObservedScopedCallback<eventpp::CallbackList<ret(args...)>>(mEvent, functor, this->shared_from_this()) // have to create object in place because std::make_unique doesn't have access to the private constructor
		);
		callbackListChangedEvent->operator()();
		return out;
	}

	bool isEventSubscribed() const { return !mEvent->empty(); }

	virtual std::shared_ptr<eventpp::CallbackList<void()>> getCallbackListChangedEvent() override {
		return callbackListChangedEvent;
	}

	ret fireEvent(args... a) { return mEvent->operator()(a...); }

	// copy is banned
	ObservedEvent(const ObservedEvent& that) = delete;
	ObservedEvent& operator=(const ObservedEvent& that) = delete;

	// move is fine
	ObservedEvent(ObservedEvent&& that) = default;
	ObservedEvent& operator=(ObservedEvent&& that) = default;
};