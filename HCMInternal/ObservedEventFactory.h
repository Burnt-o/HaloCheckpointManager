#include "pch.h"
#include "ObservedEvent.h"


// static factory

class ObservedEventFactory
{
public:
	template <typename T>
	static std::shared_ptr<ObservedEvent<T>> makeObservedEvent()
	{
		std::shared_ptr<ObservedEvent<T>> pOE(new ObservedEvent<T>());
		return std::move(pOE);
	}

	template <typename ret, typename... args>
	static std::unique_ptr<ScopedCallback<ActionEvent>> getCallbackListChangedCallback(
		std::shared_ptr<ObservedEvent< eventpp::CallbackList<ret(args...)>>> pEvent, 
		std::function<void()> functor
	)
	{
		return std::move(pEvent->getCallbackListChangedCallback(std::move(functor)));
	}
};