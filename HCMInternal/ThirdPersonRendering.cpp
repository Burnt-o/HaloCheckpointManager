#include "pch.h"
#include "ThirdPersonRendering.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "ModuleHook.h"
#include "MidhookContextInterpreter.h"
#include "IMCCStateHook.h"
#include "PointerManager.h"
#include "MultilevelPointer.h"
#include "safetyhook.hpp"

template <GameState::Value gameT>
class ThirdPersonRenderingImpl : public ThirdPersonRenderingImplUntemplated
{
private:


	// main hook function
	static void thirdPersonRenderingHookFunction(SafetyHookContext& ctx)
	{
		enum class param
		{
			ValueToSet
		};
		auto* ctxInterpreter = thirdPersonRenderingContextInterpreter.get();

		auto* valRef = ctxInterpreter->getParameterRef(ctx, (int)param::ValueToSet);
		*valRef = thirdPersonRenderingValueToSet;
	}

	// hook
	static inline std::shared_ptr<ModuleMidHook> thirdPersonRenderingHook;
	static inline std::shared_ptr<MidhookContextInterpreter> thirdPersonRenderingContextInterpreter;
	static inline int thirdPersonRenderingValueToSet;

public:
	ThirdPersonRenderingImpl(GameState gameImpl, IDIContainer& dicon)
		
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();

		auto thirdPersonRenderingFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(thirdPersonRenderingFunction), gameImpl);
		thirdPersonRenderingContextInterpreter = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(thirdPersonRenderingContextInterpreter), gameImpl);
		thirdPersonRenderingValueToSet = *ptr->getData<std::shared_ptr<int64_t>>(nameof(thirdPersonRenderingValueToSet), gameImpl).get();
		thirdPersonRenderingHook = ModuleMidHook::make(gameImpl.toModuleName(), thirdPersonRenderingFunction, thirdPersonRenderingHookFunction);

	}

	virtual void toggleThirdPersonRendering(bool enabled) override
	{
		safetyhook::ThreadFreezer threadFreezer;
		thirdPersonRenderingHook->setWantsToBeAttached(enabled);
	}

};






ThirdPersonRendering::ThirdPersonRendering(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<ThirdPersonRenderingImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<ThirdPersonRenderingImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<ThirdPersonRenderingImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<ThirdPersonRenderingImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<ThirdPersonRenderingImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<ThirdPersonRenderingImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

ThirdPersonRendering::~ThirdPersonRendering()
{
		PLOG_DEBUG << "~" << getName();
}

void ThirdPersonRendering::toggleThirdPersonRendering(bool enabled) { return pimpl->toggleThirdPersonRendering(enabled); }