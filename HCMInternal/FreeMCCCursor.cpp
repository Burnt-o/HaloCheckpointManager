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
	
	static void aiFreezeHookFunction(SafetyHookContext& ctx)
	{
		instance->shouldCursorBeFreeFunctionFlagSetter->setFlag(ctx);
	}

public:
	FreeMCCCursorImpl(std::shared_ptr<MultilevelPointer> freeCursorFunc, std::shared_ptr<MidhookFlagInterpreter> flagSetter)
		: shouldCursorBeFreeFunctionFlagSetter(flagSetter)
	{
		if (instance) throw HCMInitException("Cannot have more than one FreeMCCCursor");
		instance = this;
		shouldCursorBeFreeHook = ModuleMidHook::make(L"", freeCursorFunc, aiFreezeHookFunction, false);
	}

	~FreeMCCCursorImpl() 
	{ 
		PLOG_DEBUG << "~" << nameof(FreeMCCCursorImpl); 
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








FreeMCCCursor::FreeMCCCursor(std::shared_ptr<MultilevelPointer> freeCursorFunc, std::shared_ptr<MidhookFlagInterpreter> flagSetter)
	: pimpl(std::make_shared< FreeMCCCursorImpl>(freeCursorFunc, flagSetter)) {}

FreeMCCCursor::~FreeMCCCursor() = default;

ScopedServiceRequest FreeMCCCursor::scopedRequest(std::string callerID) { return ScopedServiceRequest(pimpl, callerID); }