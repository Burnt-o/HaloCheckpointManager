#pragma once
template <typename ret, typename... args>
class ScopedCallback;

template <typename ret, typename... args>
class ScopedCallback<eventpp::CallbackList<ret(args...)>>
{
private:
	//using MyFunc = std::function<ret(args...)>;

	std::weak_ptr <eventpp::CallbackList<ret(args...)>> m_pEventWeak;
	eventpp::CallbackList<ret(args...)>::Handle mHandle;
	std::function<ret(args...)> mFunctor;



public:

	ScopedCallback() = delete;

	explicit ScopedCallback(std::shared_ptr <eventpp::CallbackList<ret(args...)>> pEvent, std::function<ret(args...)> functor)
		: m_pEventWeak(pEvent), mFunctor(functor)
	{

		PLOG_DEBUG << "ScopedCallback constructor called";
		mHandle = pEvent->append(mFunctor);

	}

	~ScopedCallback()
	{
		PLOG_VERBOSE << "removing ScopedCallback";
		auto m_pEvent = m_pEventWeak.lock();
		if (!m_pEvent)
		{
			std::stringstream buffer;
			buffer << boost::stacktrace::basic_stacktrace();
			PLOG_ERROR << "m_pEvent bad weak ptr on ScopedCallback destruction" << buffer.str();
			return;
		}

		PLOG_VERBOSE << "removing ScopedCallback";
		if (!m_pEvent.get())
		{
			std::stringstream buffer;
			buffer << boost::stacktrace::stacktrace();
			PLOG_ERROR << "null event?!!!" << buffer.str();
			return;
		}

		if (!mHandle)
		{
			std::stringstream buffer;
			buffer << boost::stacktrace::basic_stacktrace();
			PLOG_ERROR << "null handle?!?!!! " << buffer.str();
				return;
		}

//#if HCM_DEBUG
//		std::stringstream buffer;
//		buffer << boost::stacktrace::basic_stacktrace();
//		PLOG_VERBOSE << "attempting removal, stacktrace: " << buffer.str();
//#endif

		m_pEvent->remove(mHandle);
		PLOG_DEBUG << "scoped callback removed";
	}

	void removeCallback() // used in class destructors with destruction guards, prevents any new calls of functor
	{
		auto m_pEvent = m_pEventWeak.lock();
		if (!m_pEvent)
		{
			std::stringstream buffer;
			buffer << boost::stacktrace::basic_stacktrace();
			PLOG_ERROR << "m_pEvent bad weak ptr on ScopedCallback removeCallback" << buffer.str();
			return;
		}

		PLOG_VERBOSE << "removing ScopedCallback";
		if (!m_pEvent.get())
		{
			std::stringstream buffer;
			buffer << boost::stacktrace::basic_stacktrace();
			PLOG_ERROR << "null event?!!!" << buffer.str();
			return;
		}

		if (!mHandle)
		{
			std::stringstream buffer;
			buffer << boost::stacktrace::basic_stacktrace();
			PLOG_ERROR << "null handle?!?!!!" << buffer.str();
				return;
		}
//
//#if HCM_DEBUG
//		std::stringstream buffer;
//		buffer << boost::stacktrace::basic_stacktrace();
//		PLOG_VERBOSE << "attempting removal, stacktrace: " << buffer.str();
//#endif


		m_pEvent->remove(mHandle);

		PLOG_DEBUG << "scoped callback removed";
	}

	// copy is banned
	ScopedCallback(const ScopedCallback& that) = delete;
	ScopedCallback& operator=(const ScopedCallback& that) = delete;

	// move is fine
	ScopedCallback(ScopedCallback&& that) = default;
	ScopedCallback& operator=(ScopedCallback&& that) = default;


};
