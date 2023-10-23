#include "pch.h"
#include "BlockGameInput.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"





class BlockGameInput::BlockGameInputImpl : public IProvideScopedRequests
{
private:
	static inline BlockGameInputImpl* instance = nullptr;

	std::set<std::string> callersRequestingBlockedInput{};
	std::shared_ptr<ModuleMidHook> blockGameKeyboardInputHook;
	std::shared_ptr< MidhookFlagInterpreter> blockGameKeyboardInputFunctionFlagSetter;

	std::shared_ptr<ModulePatch> blockGameMouseInputPatch;

	std::shared_ptr<ModulePatch> blockGameControllerInputPatch;


	static void blockGameKeyboardInputHookFunction(SafetyHookContext& ctx)
	{
		instance->blockGameKeyboardInputFunctionFlagSetter->setFlag(ctx);
	}


	bool previousGamePadState = false;

public:
	BlockGameInputImpl(std::shared_ptr<PointerManager> ptr)
	{
		if (instance) throw HCMInitException("Cannot have more than one BlockGameInputImpl");

		auto blockGameKeyboardInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameKeyboardInputFunction));
		blockGameKeyboardInputFunctionFlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(blockGameKeyboardInputFunctionFlagSetter));
		blockGameKeyboardInputHook = ModuleMidHook::make(L"main", blockGameKeyboardInputFunction, blockGameKeyboardInputHookFunction, false);

		auto blockGameMouseInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameMouseInputFunction));
		auto blockGameMouseInputCode = ptr->getVectorData<byte>(nameof(blockGameMouseInputCode));
		blockGameMouseInputPatch = ModulePatch::make(L"main", blockGameMouseInputFunction, *blockGameMouseInputCode.get());

		auto blockGameControllerInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameControllerInputFunction));
		auto blockGameControllerInputCode = ptr->getVectorData<byte>(nameof(blockGameControllerInputCode));
		blockGameControllerInputPatch = ModulePatch::make(L"main", blockGameControllerInputFunction, *blockGameControllerInputCode.get());


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
			blockGameKeyboardInputHook->setWantsToBeAttached(true);
			blockGameMouseInputPatch->setWantsToBeAttached(true);
			blockGameControllerInputPatch->setWantsToBeAttached(true);
		}
	}

	void unrequestService(std::string callerID)
	{
		callersRequestingBlockedInput.erase(callerID);
		if (callersRequestingBlockedInput.empty() == true)
		{
			blockGameKeyboardInputHook->setWantsToBeAttached(false);
			blockGameMouseInputPatch->setWantsToBeAttached(false);
			blockGameControllerInputPatch->setWantsToBeAttached(false);
		}
	}

};








BlockGameInput::BlockGameInput(std::shared_ptr<PointerManager> ptr)
	: pimpl(std::make_shared< BlockGameInputImpl>(ptr)) {}

BlockGameInput::~BlockGameInput() = default;

std::unique_ptr<ScopedServiceRequest> BlockGameInput::scopedRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(pimpl, callerID); }