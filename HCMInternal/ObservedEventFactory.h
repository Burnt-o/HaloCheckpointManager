#include "pch.h"
#include "ObservedEvent.h"


// static factory

class ObservedEventFactory
{
public:
	template <typename T>
	static std::shared_ptr<ObservedEvent<T>> makeObservedEvent()
	{
		return std::make_shared< ObservedEvent<T>>
			( // have to construct in place because make_shared doesn't have access to private constructor
				std::move(ObservedEvent<T>())
			);
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