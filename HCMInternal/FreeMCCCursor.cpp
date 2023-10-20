#include "pch.h"
#include "FreeMCCCursor.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"




class FreeMCCCursor::FreeMCCCursorImpl : public IProvideScopedRequests
{
private:
	static inline FreeMCCCursorImpl* instance = nullptr;

	std::set<std::string> callersRequestingFreedCursor{};
	std::shared_ptr<ModuleMidHook> shouldCursorBeFreeHook;
	std::shared_ptr< MidhookFlagInterpreter> shouldCursorBeFreeFunctionFlagSetter;
	
	static void shouldCursorBeFreeHookFunction(SafetyHookContext& ctx)
	{
		instance->shouldCursorBeFreeFunctionFlagSetter->setFlag(ctx);
	}

public:
	FreeMCCCursorImpl(std::shared_ptr<PointerManager> ptr)
	{
		if (instance) throw HCMInitException("Cannot have more than one FreeMCCCursorImpl");

		auto shouldCursorBeFreeFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(shouldCursorBeFreeFunction));
		shouldCursorBeFreeFunctionFlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(shouldCursorBeFreeFunctionFlagSetter));
		shouldCursorBeFreeHook = ModuleMidHook::make(L"main", shouldCursorBeFreeFunction, shouldCursorBeFreeHookFunction, false);

		instance = this;
	}

	~FreeMCCCursorImpl() 
	{		PLOG_DEBUG << "~" << nameof(FreeMCCCursorImpl);  

		instance = nullptr;
	}
	void requestService(std::string callerID)
	{
		callersRequestingFreedCursor.insert(callerID);
		if (callersRequestingFreedCursor.empty() == false)
		{
			shouldCursorBeFreeHook->setWantsToBeAttached(true);
		}
	}

	void unrequestService(std::string callerID)
	{
		callersRequestingFreedCursor.erase(callerID);
		if (callersRequestingFreedCursor.empty() == true)
		{
			shouldCursorBeFreeHook->setWantsToBeAttached(false);
		}
	}

};








FreeMCCCursor::FreeMCCCursor(std::shared_ptr<PointerManager> ptr)
	: pimpl(std::make_shared< FreeMCCCursorImpl>(ptr)) {}

FreeMCCCursor::~FreeMCCCursor() = default;

std::unique_ptr<ScopedServiceRequest> FreeMCCCursor::scopedRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(pimpl, callerID); }