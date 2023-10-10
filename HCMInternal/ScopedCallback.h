#pragma once
template <typename ret, typename... args>
class ScopedCallback;

template <typename ret, typename... args>
class ScopedCallback<eventpp::CallbackList<ret(args...)>>
{
private:
	//using MyFunc = std::function<ret(args...)>;

	std::shared_ptr <eventpp::CallbackList<ret(args...)>> m_pEvent;
	eventpp::CallbackList<ret(args...)>::Handle mHandle;
	std::function<ret(args...)> mFunctor;



public:

	ScopedCallback() = delete;

	explicit ScopedCallback(std::shared_ptr <eventpp::CallbackList<ret(args...)>> pEvent, std::function<ret(args...)> functor)
		: m_pEvent(pEvent), mFunctor(functor)
	{
		PLOG_DEBUG << "ScopedCallback constructor called";
		if (m_pEvent)
		{
			mHandle = m_pEvent->append(mFunctor);
		}
		else
		{
			PLOG_ERROR << "Scoped callback constructed with null event arg";
		}
	}

	~ScopedCallback()
	{
		if (!m_pEvent || !mHandle) { PLOG_ERROR << "~ScopedCallback: null callback"; return; }

		PLOG_VERBOSE << "removing ScopedCallback";
		m_pEvent->remove(mHandle);
	}

	void removeCallback() // used in class destructors with destruction guards, prevents any new calls of functor
	{
		if (!m_pEvent || !mHandle) { PLOG_ERROR << "ScopedCallback::removeCallback() null callback"; return; }

		PLOG_VERBOSE << "removing ScopedCallback";
		m_pEvent->remove(mHandle);
	}


	ScopedCallback(const ScopedCallback& that) = delete;
	ScopedCallback& operator=(const ScopedCallback& that) = delete;

	ScopedCallback(ScopedCallback&& that) = default;
	ScopedCallback& operator=(ScopedCallback&& that) = delete;


};
