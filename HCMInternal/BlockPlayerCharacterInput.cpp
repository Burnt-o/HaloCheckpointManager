#include "pch.h"
#include "BlockPlayerCharacterInput.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"
#include "IMCCStateHook.h"
#include "PointerManager.h"
#include "MultilevelPointer.h"
#include "safetyhook.hpp"

template <GameState::Value gameT>
class BlockPlayerCharacterInputImpl : public BlockPlayerCharacterInputImplUntemplated
{
private:


	// main hook function
	static void BlockPlayerCharacterInput1HookFunction(SafetyHookContext& ctx)
	{
		BlockPlayerCharacterInput1FlagSetter->setFlag(ctx);
	}

	static void BlockPlayerCharacterInput2HookFunction(SafetyHookContext& ctx)
	{
		BlockPlayerCharacterInput2FlagSetter->setFlag(ctx);
	}

	// hook
	static inline std::shared_ptr<ModuleMidHook> BlockPlayerCharacterInput1Hook;
	static inline std::shared_ptr< MidhookFlagInterpreter> BlockPlayerCharacterInput1FlagSetter;

	static inline std::shared_ptr<ModuleMidHook> BlockPlayerCharacterInput2Hook;
	static inline std::shared_ptr< MidhookFlagInterpreter> BlockPlayerCharacterInput2FlagSetter;

public:
	BlockPlayerCharacterInputImpl(GameState gameImpl, IDIContainer& dicon)

	{
		auto ptr = dicon.Resolve<PointerManager>().lock();

		auto BlockPlayerCharacterInput1Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(BlockPlayerCharacterInput1Function), gameImpl);
		BlockPlayerCharacterInput1FlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(BlockPlayerCharacterInput1FlagSetter), gameImpl);
		BlockPlayerCharacterInput1Hook = ModuleMidHook::make(gameImpl.toModuleName(), BlockPlayerCharacterInput1Function, BlockPlayerCharacterInput1HookFunction);

		auto BlockPlayerCharacterInput2Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(BlockPlayerCharacterInput2Function), gameImpl);
		BlockPlayerCharacterInput2FlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(BlockPlayerCharacterInput2FlagSetter), gameImpl);
		BlockPlayerCharacterInput2Hook = ModuleMidHook::make(gameImpl.toModuleName(), BlockPlayerCharacterInput2Function, BlockPlayerCharacterInput2HookFunction);
	}

	virtual void toggleBlockPlayerCharacterInput(bool enabled) override
	{
		safetyhook::ThreadFreezer threadFreezer;
		BlockPlayerCharacterInput1Hook->setWantsToBeAttached(enabled);
		BlockPlayerCharacterInput2Hook->setWantsToBeAttached(enabled);
	}

};






BlockPlayerCharacterInput::BlockPlayerCharacterInput(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<BlockPlayerCharacterInputImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<BlockPlayerCharacterInputImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<BlockPlayerCharacterInputImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<BlockPlayerCharacterInputImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<BlockPlayerCharacterInputImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<BlockPlayerCharacterInputImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

BlockPlayerCharacterInput::~BlockPlayerCharacterInput()
{
	PLOG_DEBUG << "~" << getName();
}

void BlockPlayerCharacterInput::toggleBlockPlayerCharacterInput(bool enabled) { return pimpl->toggleBlockPlayerCharacterInput(enabled); }