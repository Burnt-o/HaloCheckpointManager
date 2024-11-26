#include "pch.h"
#include "DisableScreenEffects.h"
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
class DisableScreenEffectsImpl : public TokenSharedRequestProvider
{
private:

	// hook
	static inline std::shared_ptr<ModulePatch> disableScreenEffectsHook;

public:
	DisableScreenEffectsImpl(GameState gameImpl, IDIContainer& dicon)

	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableScreenEffectsFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableScreenEffectsFunction), gameImpl);
		auto disableScreenEffectsCode = ptr->getVectorData<byte>(nameof(disableScreenEffectsCode), gameImpl);
		disableScreenEffectsHook = ModulePatch::make(gameImpl.toModuleName(), disableScreenEffectsFunction, *disableScreenEffectsCode.get());

	}

	virtual void updateService() override
	{
		disableScreenEffectsHook->setWantsToBeAttached(serviceIsRequested());
	}


};






DisableScreenEffects::DisableScreenEffects(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<DisableScreenEffectsImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<DisableScreenEffectsImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<DisableScreenEffectsImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<DisableScreenEffectsImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<DisableScreenEffectsImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<DisableScreenEffectsImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

DisableScreenEffects::~DisableScreenEffects()
{
	PLOG_DEBUG << "~" << getName();
}
