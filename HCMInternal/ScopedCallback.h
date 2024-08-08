#pragma once
template <typename ret, typename... args>
class ScopedCallback;

template <typename ret, typename... args>
class ScopedCallback<eventpp::CallbackList<ret(args...)>>
{
private:
	std::weak_ptr <eventpp::CallbackList<ret(args...)>> m_pEventWeak;
	eventpp::CallbackList<ret(args...)>::Handle mHandle;
public:

	ScopedCallback() = delete;

	explicit ScopedCallback(std::shared_ptr <eventpp::CallbackList<ret(args...)>> pEvent, std::function<ret(args...)> functor)
		: m_pEventWeak(pEvent), mHandle(pEvent->append(functor)) {}

	virtual ~ScopedCallback()
	{
		auto m_pEvent = m_pEventWeak.lock();
		if (m_pEvent && mHandle)
			m_pEvent->remove(mHandle);
	}

	void removeCallback() // used in class destructors with destruction guards to prevent any new calls of functor
	{
		auto m_pEvent = m_pEventWeak.lock();
		if (m_pEvent && mHandle)
			m_pEvent->remove(mHandle);

		// to prevent double-removal
		m_pEventWeak.reset();
	}

	// copy is banned
	ScopedCallback(const ScopedCallback& that) = delete;
	ScopedCallback& operator=(const ScopedCallback& that) = delete;

	// move is banned
	ScopedCallback(ScopedCallback&& that) = delete;
	ScopedCallback& operator=(ScopedCallback&& that) = delete;


};
