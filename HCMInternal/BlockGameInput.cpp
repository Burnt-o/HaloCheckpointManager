#include "pch.h"
#include "BlockGameInput.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"





class BlockGameInputImpl : public TokenScopedServiceProvider
{
private:


	std::shared_ptr<ModulePatch> blockGameKeyboardInputHook;
	std::shared_ptr<ModulePatch> blockGameMouseMoveInputPatch;
	std::shared_ptr<ModulePatch> blockGameMouseClickInputPatch;
	std::shared_ptr<ModulePatch> blockGameControllerInputPatch;


public:
	BlockGameInputImpl(std::shared_ptr<PointerDataStore> ptr)
	{

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
	}

	~BlockGameInputImpl()
	{
		PLOG_DEBUG << "~" << nameof(BlockGameInputImpl);
	}

	virtual void updateService() override
	{
		bool requested = serviceIsRequested();
		blockGameKeyboardInputHook->setWantsToBeAttached(requested);
		blockGameMouseMoveInputPatch->setWantsToBeAttached(requested);
		blockGameMouseClickInputPatch->setWantsToBeAttached(requested);
		blockGameControllerInputPatch->setWantsToBeAttached(requested);
	}


};








BlockGameInput::BlockGameInput(std::shared_ptr<PointerDataStore> ptr)
	: pimpl(std::make_shared< BlockGameInputImpl>(ptr)) {}

BlockGameInput::~BlockGameInput() = default;
