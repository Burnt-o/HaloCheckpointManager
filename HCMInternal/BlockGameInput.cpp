#include "pch.h"
#include "BlockGameInput.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"




class BlockGameInput::BlockGameInputImpl : public IProvideScopedRequests
{
private:
	static inline BlockGameInputImpl* instance = nullptr;

	std::set<std::string> callersRequestingBlockedInput{};
	std::shared_ptr<ModuleMidHook> blockGameInputHook;
	std::shared_ptr< MidhookFlagInterpreter> blockGameInputFunctionFlagSetter;

	static void blockGameInputHookFunction(SafetyHookContext& ctx)
	{
		instance->blockGameInputFunctionFlagSetter->setFlag(ctx);
	}

public:
	BlockGameInputImpl(std::shared_ptr<PointerManager> ptr)
	{
		if (instance) throw HCMInitException("Cannot have more than one BlockGameInputImpl");

		auto blockGameInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameInputFunction));
		blockGameInputFunctionFlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(blockGameInputFunctionFlagSetter));
		blockGameInputHook = ModuleMidHook::make(L"main", blockGameInputFunction, blockGameInputHookFunction, false);

		instance = this;
	}

	~BlockGameInputImpl()
	{
		PLOG_DEBUG << "~" << nameof(BlockGameInputImpl);

		instance = nullptr;
	}
	void requestService(std::string callerID)
	{
		callersRequestingBlockedInput.insert(callerID);
		if (callersRequestingBlockedInput.empty() == false)
		{
			blockGameInputHook->setWantsToBeAttached(true);
		}
	}

	void unrequestService(std::string callerID)
	{
		callersRequestingBlockedInput.erase(callerID);
		if (callersRequestingBlockedInput.empty() == true)
		{
			blockGameInputHook->setWantsToBeAttached(false);
		}
	}

};








BlockGameInput::BlockGameInput(std::shared_ptr<PointerManager> ptr)
	: pimpl(std::make_shared< BlockGameInputImpl>(ptr)) {}

BlockGameInput::~BlockGameInput() = default;

std::unique_ptr<ScopedServiceRequest> BlockGameInput::scopedRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(pimpl, callerID); }