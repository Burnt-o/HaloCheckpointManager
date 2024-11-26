#include "pch.h"
#include "HideHUD.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"
#include "SettingsStateAndEvents.h"
#include "PointerDataStore.h"

template <GameState::Value gameT>
class HideHUDImplH1 : public TokenSharedRequestProvider
{
private:
	GameState mGame;

	static inline std::shared_ptr<ModulePatch> hideHUDPatchHook1;
	static inline std::shared_ptr<ModulePatch> hideHUDPatchHook2;
	static inline std::shared_ptr<ModulePatch> hideHUDPatchHook3;



public:
	HideHUDImplH1(GameState gameImpl, IDIContainer& dicon)
		:
		mGame(gameImpl)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto hideHUDPatchFunction1 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDPatchFunction1), gameImpl); 
		auto hideHUDPatchCode1 = ptr->getVectorData<byte>(nameof(hideHUDPatchCode1), gameImpl);
		hideHUDPatchHook1 = ModulePatch::make(gameImpl.toModuleName(), hideHUDPatchFunction1, *hideHUDPatchCode1.get());

		auto hideHUDPatchFunction2 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDPatchFunction2), gameImpl);
		auto hideHUDPatchCode2 = ptr->getVectorData<byte>(nameof(hideHUDPatchCode2), gameImpl);
		hideHUDPatchHook2 = ModulePatch::make(gameImpl.toModuleName(), hideHUDPatchFunction2, *hideHUDPatchCode2.get());

		auto hideHUDPatchFunction3 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDPatchFunction3), gameImpl);
		auto hideHUDPatchCode3 = ptr->getVectorData<byte>(nameof(hideHUDPatchCode3), gameImpl);
		hideHUDPatchHook3 = ModulePatch::make(gameImpl.toModuleName(), hideHUDPatchFunction3, *hideHUDPatchCode3.get());

	}

	virtual void updateService() override
	{
		// attach if requested
		PLOG_VERBOSE << "HideHUDImplH1::updateService";



		bool newState = serviceIsRequested();
		hideHUDPatchHook1->setWantsToBeAttached(newState);
		hideHUDPatchHook2->setWantsToBeAttached(newState);
		hideHUDPatchHook3->setWantsToBeAttached(newState);
		PLOG_VERBOSE << "HideHUDImplH1::updateService DONE";
	}
};


template <GameState::Value gameT>
class HideHUDImplSinglePatch : public TokenSharedRequestProvider
{
private:
	GameState mGame;


	static inline std::shared_ptr<ModulePatch> hideHUDPatchHook1;




public:
	HideHUDImplSinglePatch(GameState gameImpl, IDIContainer& dicon)
		:
		mGame(gameImpl)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto hideHUDPatchFunction1 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDPatchFunction1), gameImpl);
		auto hideHUDPatchCode1 = ptr->getVectorData<byte>(nameof(hideHUDPatchCode1), gameImpl);
		hideHUDPatchHook1 = ModulePatch::make(gameImpl.toModuleName(), hideHUDPatchFunction1, *hideHUDPatchCode1.get());

	}

	virtual void updateService() override
	{
		// attach if requested
		PLOG_VERBOSE << "HideHUDImplSinglePatch::updateService";
		hideHUDPatchHook1->setWantsToBeAttached(serviceIsRequested());
		PLOG_VERBOSE << "HideHUDImplSinglePatch::updateService DONE";
	}
};

template <GameState::Value gameT>
class HideHUDImplDoublePatch : public TokenSharedRequestProvider
{
private:
	GameState mGame;


	static inline std::shared_ptr<ModulePatch> hideHUDPatchHook1;
	static inline std::shared_ptr<ModulePatch> hideHUDPatchHook2;




public:
	HideHUDImplDoublePatch(GameState gameImpl, IDIContainer& dicon)
		:
		mGame(gameImpl)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto hideHUDPatchFunction1 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDPatchFunction1), gameImpl);
		auto hideHUDPatchCode1 = ptr->getVectorData<byte>(nameof(hideHUDPatchCode1), gameImpl);
		hideHUDPatchHook1 = ModulePatch::make(gameImpl.toModuleName(), hideHUDPatchFunction1, *hideHUDPatchCode1.get());

		auto hideHUDPatchFunction2 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(hideHUDPatchFunction2), gameImpl);
		auto hideHUDPatchCode2 = ptr->getVectorData<byte>(nameof(hideHUDPatchCode2), gameImpl);
		hideHUDPatchHook2 = ModulePatch::make(gameImpl.toModuleName(), hideHUDPatchFunction2, *hideHUDPatchCode2.get());

	}

	virtual void updateService() override
	{
		// attach if requested
		PLOG_VERBOSE << "HideHUDImplDoublePatch::updateService";
		hideHUDPatchHook1->setWantsToBeAttached(serviceIsRequested());
		hideHUDPatchHook2->setWantsToBeAttached(serviceIsRequested());
		PLOG_VERBOSE << "HideHUDImplDoublePatch::updateService DONE";
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
		pimpl = std::make_shared<HideHUDImplSinglePatch<GameState::Value::Halo2>>(gameImpl, dicon);
		break;


	case GameState::Value::Halo3:
		pimpl = std::make_unique<HideHUDImplSinglePatch<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<HideHUDImplSinglePatch<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<HideHUDImplDoublePatch<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<HideHUDImplDoublePatch<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet!");
	}
}

HideHUD::~HideHUD()
{
	PLOG_DEBUG << "~" << getName();
}