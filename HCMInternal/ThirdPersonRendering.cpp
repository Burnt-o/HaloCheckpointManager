#include "pch.h"
#include "ThirdPersonRendering.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "ModuleHook.h"
#include "MidhookContextInterpreter.h"
#include "MidhookFlagInterpreter.h"
#include "IMCCStateHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "safetyhook.hpp"
#include "IMakeOrGetCheat.h"
#include "HideHUD.h"

template <GameState::Value gameT>
class ThirdPersonRenderingInlineImpl : public ThirdPersonRenderingImplUntemplated
{
private:
	GameState mGame;


	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<HideHUD> hideHUDWeak;

	// main hook function
	static __int64 thirdPersonRenderingHookFunction(int arg)
	{
		return thirdPersonRenderingValueToSet;
	}

	// hook
	static inline std::shared_ptr<ModuleInlineHook> thirdPersonRenderingHook;

	static inline int thirdPersonRenderingValueToSet;

	// hide hud service request
	std::optional<std::unique_ptr<ScopedServiceRequest>> hideHUDServiceRequest = std::nullopt;


public:
	ThirdPersonRenderingInlineImpl(GameState game, IDIContainer& dicon)
		:
		mGame(game),
		hideHUDWeak(resolveDependentCheat(HideHUD)),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>())
		
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto thirdPersonRenderingFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(thirdPersonRenderingFunction), game);
		thirdPersonRenderingValueToSet = *ptr->getData<std::shared_ptr<int64_t>>(nameof(thirdPersonRenderingValueToSet), game).get();
		thirdPersonRenderingHook = ModuleInlineHook::make(game.toModuleName(), thirdPersonRenderingFunction, thirdPersonRenderingHookFunction);

	}

	virtual void toggleThirdPersonRendering(bool enabled) override
	{
		try
		{
			{
				safetyhook::ThreadFreezer threadFreezer;
				thirdPersonRenderingHook->setWantsToBeAttached(enabled);
			}

			if (enabled)
			{
				lockOrThrow(hideHUDWeak, hideHUD);
				hideHUDServiceRequest = hideHUD->makeRequest(std::format("{}:{}", nameof(ThirdPersonRenderingInlineImpl), mGame.toString()));
			}
			else
			{
				hideHUDServiceRequest = std::nullopt;
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
		
	}

};



template <GameState::Value gameT>
class ThirdPersonRenderingFlagImpl : public ThirdPersonRenderingImplUntemplated
{
private:
	GameState mGame;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<HideHUD> hideHUDWeak;

	// main hook function
	static void thirdPersonRenderingHookFunction(SafetyHookContext& ctx)
	{
		thirdPersonRenderingFlagInterpreter->setFlag(ctx);
	}

	// hook
	static inline std::shared_ptr<ModuleMidHook> thirdPersonRenderingHook;
	static inline std::shared_ptr< MidhookFlagInterpreter> thirdPersonRenderingFlagInterpreter;

	// hide hud service request
	std::optional<std::unique_ptr<ScopedServiceRequest>> hideHUDServiceRequest = std::nullopt;

public:
	ThirdPersonRenderingFlagImpl(GameState game, IDIContainer& dicon)
		:
		mGame(game),
		hideHUDWeak(resolveDependentCheat(HideHUD)),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>())
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto thirdPersonRenderingFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(thirdPersonRenderingFunction), game);
		thirdPersonRenderingFlagInterpreter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(thirdPersonRenderingFlagInterpreter), game);
		thirdPersonRenderingHook = ModuleMidHook::make(game.toModuleName(), thirdPersonRenderingFunction, thirdPersonRenderingHookFunction);

	}

	virtual void toggleThirdPersonRendering(bool enabled) override
	{
		try
		{
			{
				safetyhook::ThreadFreezer threadFreezer;
				thirdPersonRenderingHook->setWantsToBeAttached(enabled);
			}

			if (enabled)
			{
				lockOrThrow(hideHUDWeak, hideHUD);
				hideHUDServiceRequest = hideHUD->makeRequest(std::format("{}:{}", nameof(ThirdPersonRenderingInlineImpl), mGame.toString()));
			}
			else
			{
				hideHUDServiceRequest = std::nullopt;
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}

};


//template <GameState::Value gameT>
//class ThirdPersonRenderingContextImpl : public ThirdPersonRenderingImplUntemplated
//{
//private:
//
//
//	// main hook function
//	static void thirdPersonRenderingHookFunction(SafetyHookContext& ctx)
//	{
//			enum class param
//			{
//				ValueToSet
//			};
//			auto* ctxInterpreter = thirdPersonRenderingContextInterpreter.get();
//
//			auto* valRef = ctxInterpreter->getParameterRef(ctx, (int)param::ValueToSet);
//			*valRef = thirdPersonRenderingValueToSet;
//
//	}
//
//	// hook
//	static inline std::shared_ptr<ModuleMidHook> thirdPersonRenderingHook;
//	static inline int thirdPersonRenderingValueToSet;
//	static inline std::shared_ptr< MidhookContextInterpreter> thirdPersonRenderingContextInterpreter;
//
//public:
//	ThirdPersonRenderingContextImpl(GameState gameImpl, IDIContainer& dicon)
//
//	{
//		auto ptr = dicon.Resolve<PointerDataStore>().lock();
//
//		auto thirdPersonRenderingFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(thirdPersonRenderingFunction), gameImpl);
//		thirdPersonRenderingContextInterpreter = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(thirdPersonRenderingContextInterpreter), gameImpl);
//		thirdPersonRenderingValueToSet = *ptr->getData<std::shared_ptr<int64_t>>(nameof(thirdPersonRenderingValueToSet), gameImpl).get();
//		thirdPersonRenderingHook = ModuleMidHook::make(gameImpl.toModuleName(), thirdPersonRenderingFunction, thirdPersonRenderingHookFunction);
//
//	}
//
//	virtual void toggleThirdPersonRendering(bool enabled) override
//	{
//		safetyhook::ThreadFreezer threadFreezer;
//		thirdPersonRenderingHook->setWantsToBeAttached(enabled);
//	}
//
//};





ThirdPersonRendering::ThirdPersonRendering(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<ThirdPersonRenderingInlineImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<ThirdPersonRenderingInlineImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<ThirdPersonRenderingFlagImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<ThirdPersonRenderingFlagImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<ThirdPersonRenderingFlagImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<ThirdPersonRenderingFlagImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet ?");
	}
}

ThirdPersonRendering::~ThirdPersonRendering()
{
		PLOG_DEBUG << "~" << getName();
}

void ThirdPersonRendering::toggleThirdPersonRendering(bool enabled) { return pimpl->toggleThirdPersonRendering(enabled); }