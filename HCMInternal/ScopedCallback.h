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

	ScopedCallback(std::shared_ptr <eventpp::CallbackList<ret(args...)>> pEvent, std::function<ret(args...)> functor)
		: m_pEvent(pEvent), mFunctor(functor)
	{
		PLOG_DEBUG << "ScopedCallback constructor called";
		if (m_pEvent)
		{
			mHandle = m_pEvent->append(mFunctor);
		}
		else
		{
			throw ("Scoped callback constructed with null event arg");
		}
	}

	~ScopedCallback()
	{
		if (!m_pEvent || !mHandle) { PLOG_ERROR << "~ScopedCallback: null callback"; return; }

		PLOG_VERBOSE << "removing ScopedCallback";
		m_pEvent->remove(mHandle);
	}

	

	//// copy is not allowed
	//ScopedCallback(const ScopedCallback& that) = delete;
	//ScopedCallback& operator=(const ScopedCallback& that) = delete;


	//ScopedCallback(ScopedCallback&& that) = default;
	//ScopedCallback& operator=(ScopedCallback&& that) = delete;

	// moves okay but needs special append logic
	//ScopedCallback(ScopedCallback&& that)
	//{
	//	swap(*this, that);
	//	/*m_pEvent = that.m_pEvent;
	//	mFunctor = that.mFunctor;

	//	if (m_pEvent)
	//		mHandle = m_pEvent->append(mFunctor);*/
	//}

	//ScopedCallback& operator=(ScopedCallback&& that)
	//{
	//	swap(*this, that);
	//	return *this;
	//	/*std::swap(m_pEvent, that.m_pEvent);
	//	std::swap(mFunctor, that.mFunctor);
	//	if (m_pEvent)
	//		mHandle = m_pEvent->append(mFunctor);
	//	return *this;*/
	//}

	//friend void swap(ScopedCallback& lhs, ScopedCallback& rhs)
	//{
	//	using std::swap;
	//	swap(lhs.m_pEvent, rhs.m_pEvent);
	//	swap(lhs.mFunctor, rhs.mFunctor);
	//	if (lhs.m_pEvent)
	//	{
	//		lhs.mHandle = lhs.m_pEvent->append(lhs.mFunctor);
	//	}
	//}


};
