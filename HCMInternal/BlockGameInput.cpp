#include "pch.h"
#include "BlockGameInput.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"





class BlockGameInput::BlockGameInputImpl : public IProvideScopedRequests
{
private:
	static inline BlockGameInputImpl* instance = nullptr;

	std::set<std::string> callersRequestingBlockedInput{};

	std::shared_ptr<ModulePatch> blockGameKeyboardInputHook;
	std::shared_ptr<ModulePatch> blockGameMouseMoveInputPatch;
	std::shared_ptr<ModulePatch> blockGameMouseClickInputPatch;
	std::shared_ptr<ModulePatch> blockGameControllerInputPatch;


public:
	BlockGameInputImpl(std::shared_ptr<PointerDataStore> ptr)
	{
		if (instance) throw HCMInitException("Cannot have more than one BlockGameInputImpl");

		auto blockGameKeyboardInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameKeyboardInputFunction));
		auto blockGameKeyboardInputCode = ptr->getVectorData<byte>(nameof(blockGameKeyboardInputCode));
		blockGameKeyboardInputHook = ModulePatch::make(L"main", blockGameKeyboardInputFunction, *blockGameKeyboardInputCode.get());

		auto blockGameMouseMoveInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameMouseMoveInputFunction));
		auto blockGameMouseMoveInputCode = ptr->getVectorData<byte>(nameof(blockGameMouseMoveInputCode));
		blockGameMouseMoveInputPatch = ModulePatch::make(L"main", blockGameMouseMoveInputFunction, *blockGameMouseMoveInputCode.get());

		auto blockGameMouseClickInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameMouseClickInputFunction));
		auto blockGameMouseClickInputCode = ptr->getVectorData<byte>(nameof(blockGameMouseClickInputCode));
		blockGameMouseClickInputPatch = ModulePatch::make(L"main", blockGameMouseClickInputFunction, *blockGameMouseClickInputCode.get());

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
			PLOG_DEBUG << "attaching blockKeyboard hook";
			blockGameKeyboardInputHook->setWantsToBeAttached(true);

			PLOG_DEBUG << "attaching blockMouseMove hook";
			blockGameMouseMoveInputPatch->setWantsToBeAttached(true);

			PLOG_DEBUG << "attaching blockMouseClick hook";
			blockGameMouseClickInputPatch->setWantsToBeAttached(true);

			PLOG_DEBUG << "attaching blockController hook";
			blockGameControllerInputPatch->setWantsToBeAttached(true);
		}
	}

	void unrequestService(std::string callerID)
	{
		callersRequestingBlockedInput.erase(callerID);
		if (callersRequestingBlockedInput.empty() == true)
		{
			blockGameKeyboardInputHook->setWantsToBeAttached(false);
			blockGameMouseMoveInputPatch->setWantsToBeAttached(false);
			blockGameMouseClickInputPatch->setWantsToBeAttached(false);
			blockGameControllerInputPatch->setWantsToBeAttached(false);
		}
	}

};








BlockGameInput::BlockGameInput(std::shared_ptr<PointerDataStore> ptr)
	: pimpl(std::make_shared< BlockGameInputImpl>(ptr)) {}

BlockGameInput::~BlockGameInput() = default;

std::unique_ptr<ScopedServiceRequest> BlockGameInput::scopedRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(pimpl, callerID); }