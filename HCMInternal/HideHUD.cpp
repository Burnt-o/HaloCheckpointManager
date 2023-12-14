#include "pch.h"
#include "HideHUD.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"
#include "SettingsStateAndEvents.h"
#include "PointerManager.h"

template <GameState::Value gameT>
class HideHUDImplH1 : public GenericScopedServiceProvider
{
private:
	GameState mGame;

	static inline std::shared_ptr<ModuleMidHook> hideHUDFlagHook1;
	static inline std::shared_ptr< MidhookFlagInterpreter> hideHUDFlagInterpreter1;
	static void hideHUDFlagHookFunction1(SafetyHookContext& ctx) { hideHUDFlagInterpreter1->setFlag(ctx); }


	static inline std::shared_ptr<ModuleMidHook> hideHUDFlagHook2;
	static inline std::shared_ptr< MidhookFlagInterpreter> hideHUDFlagInterpreter2;
	static void hideHUDFlagHookFunction2(SafetyHookContext& ctx) { hideHUDFlagInterpreter2->setFlag(ctx); }


	static inline std::shared_ptr<ModuleMidHook> hideHUDFlagHook3;
	static inline std::shared_ptr< MidhookFlagInterpreter> hideHUDFlagInterpreter3;
	static void hideHUDFlagHookFunction3(SafetyHookContext& ctx) { hideHUDFlagInterpreter3->setFlag(ctx); }

public:
	HideHUDImplH1(GameState gameImpl, IDIContainer& dicon)
		:
		mGame(gameImpl)
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();

		auto hideHUDFlagFunction1 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDFlagFunction1), mGame);
		hideHUDFlagInterpreter1 = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(hideHUDFlagInterpreter1), mGame);
		hideHUDFlagHook1 = ModuleMidHook::make(GameState(mGame).toModuleName(), hideHUDFlagFunction1, hideHUDFlagHookFunction1);

		auto hideHUDFlagFunction2 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDFlagFunction2), mGame);
		hideHUDFlagInterpreter2 = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(hideHUDFlagInterpreter2), mGame);
		hideHUDFlagHook2 = ModuleMidHook::make(GameState(mGame).toModuleName(), hideHUDFlagFunction2, hideHUDFlagHookFunction2);

		auto hideHUDFlagFunction3 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDFlagFunction3), mGame);
		hideHUDFlagInterpreter3 = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(hideHUDFlagInterpreter3), mGame);
		hideHUDFlagHook3 = ModuleMidHook::make(GameState(mGame).toModuleName(), hideHUDFlagFunction3, hideHUDFlagHookFunction3);
	}

	virtual void updateService() override
	{
		// attach if requested
		PLOG_VERBOSE << "HideHUDImplH1::updateService";
		safetyhook::ThreadFreezer freezeThreads;
		hideHUDFlagHook1->setWantsToBeAttached(serviceIsRequested());
		hideHUDFlagHook2->setWantsToBeAttached(serviceIsRequested());
		hideHUDFlagHook3->setWantsToBeAttached(serviceIsRequested());
		PLOG_VERBOSE << "HideHUDImplH1::updateService DONE";
	}
};


template <GameState::Value gameT>
class HideHUDImplH2 : public GenericScopedServiceProvider
{
private:
	GameState mGame;

	static inline std::shared_ptr<ModuleMidHook> hideHUDFlagHook1;
	static inline std::shared_ptr< MidhookFlagInterpreter> hideHUDFlagInterpreter1;
	static void hideHUDFlagHookFunction1(SafetyHookContext& ctx) { hideHUDFlagInterpreter1->setFlag(ctx); }



public:
	HideHUDImplH2(GameState gameImpl, IDIContainer& dicon)
		:
		mGame(gameImpl)
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();

		auto hideHUDFlagFunction1 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDFlagFunction1), mGame);
		hideHUDFlagInterpreter1 = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(hideHUDFlagInterpreter1), mGame);
		hideHUDFlagHook1 = ModuleMidHook::make(GameState(mGame).toModuleName(), hideHUDFlagFunction1, hideHUDFlagHookFunction1);

	}

	virtual void updateService() override
	{
		// attach if requested
		PLOG_VERBOSE << "HideHUDImplH2::updateService";
		safetyhook::ThreadFreezer freezeThreads;
		hideHUDFlagHook1->setWantsToBeAttached(serviceIsRequested());
		PLOG_VERBOSE << "HideHUDImplH2::updateService DONE";
	}
};



HideHUD::HideHUD(GameState gameImpl, IDIContainer& dicon)
	:
	mHideHUDToggleCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->hideHUDToggle->valueChangedEvent, [this](bool& n) {onHideHUDToggleEvent(n); }),
	mGame(gameImpl),
	mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
	runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
	messagesGUIWeak(dicon.Resolve<IMessagesGUI>())
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_shared<HideHUDImplH1<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_shared<HideHUDImplH2<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	/*case GameState::Value::Halo2:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo2>>(dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo3>>(dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo3ODST>>(dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::HaloReach>>(dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo4>>(dicon);
		break;*/
	default:
		throw HCMInitException("not impl yet");
	}
}

HideHUD::~HideHUD()
{
	PLOG_DEBUG << "~" << getName();
}