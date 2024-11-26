#include "pch.h"
#include "BlockPlayerCharacterInput.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"
#include "IMCCStateHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "safetyhook.hpp"



template <GameState::Value gameT>
class BlockPlayerCharacterInputImplMidHook :  public TokenSharedRequestProvider
{
private:


	// main hook function
	static void BlockPlayerCharacterInputHookFunction(SafetyHookContext& ctx)
	{
		BlockPlayerCharacterInputFlagSetter->setFlag(ctx);
	}


	// hook
	static inline std::shared_ptr<ModuleMidHook> BlockPlayerCharacterInputHook;
	static inline std::shared_ptr< MidhookFlagInterpreter> BlockPlayerCharacterInputFlagSetter;


public:
	BlockPlayerCharacterInputImplMidHook(GameState gameImpl, IDIContainer& dicon)

	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto BlockPlayerCharacterInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(BlockPlayerCharacterInputFunction), gameImpl);
		BlockPlayerCharacterInputFlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(BlockPlayerCharacterInputFlagSetter), gameImpl);
		BlockPlayerCharacterInputHook = ModuleMidHook::make(gameImpl.toModuleName(), BlockPlayerCharacterInputFunction, BlockPlayerCharacterInputHookFunction);


	}

	virtual void updateService() override
	{
		BlockPlayerCharacterInputHook->setWantsToBeAttached(serviceIsRequested());
	}



};

template <GameState::Value gameT>
class BlockPlayerCharacterInputImplPatch :  public TokenSharedRequestProvider
{
private:
	// hook
	 std::shared_ptr<ModulePatch> BlockPlayerCharacterInputPatch;

public:
	BlockPlayerCharacterInputImplPatch(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto BlockPlayerCharacterInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(BlockPlayerCharacterInputFunction), game);
		auto BlockPlayerCharacterInputCode = ptr->getVectorData<byte>(nameof(BlockPlayerCharacterInputCode), game);
		BlockPlayerCharacterInputPatch = ModulePatch::make(game.toModuleName(), BlockPlayerCharacterInputFunction, *BlockPlayerCharacterInputCode.get());

	}

	virtual void updateService() override
	{
		BlockPlayerCharacterInputPatch->setWantsToBeAttached(serviceIsRequested());
	}


};







BlockPlayerCharacterInput::BlockPlayerCharacterInput(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<BlockPlayerCharacterInputImplMidHook<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<BlockPlayerCharacterInputImplMidHook<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<BlockPlayerCharacterInputImplPatch<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<BlockPlayerCharacterInputImplPatch<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<BlockPlayerCharacterInputImplPatch<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<BlockPlayerCharacterInputImplPatch<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

BlockPlayerCharacterInput::~BlockPlayerCharacterInput()
{
	PLOG_DEBUG << "~" << getName();
}
