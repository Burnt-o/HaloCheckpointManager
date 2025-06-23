#include "pch.h"
#include "FreeMCCCursor.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"




class FreeMCCCursorImpl : public TokenSharedRequestProvider
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
	FreeMCCCursorImpl(std::shared_ptr<PointerDataStore> ptr)
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

	virtual void updateService() override
	{
		bool requested = serviceIsRequested();
		PLOG_INFO << "FreeMCCCursor service is turning " << (requested ? "ON!" : "OFF!");
		shouldCursorBeFreeHook->setWantsToBeAttached(requested);
		PLOG_DEBUG << "FreeMCCCursor service hooks have been updated.";
	}

};








FreeMCCCursor::FreeMCCCursor(std::shared_ptr<PointerDataStore> ptr)
	: pimpl(std::make_shared< FreeMCCCursorImpl>(ptr)) {}

FreeMCCCursor::~FreeMCCCursor() = default;