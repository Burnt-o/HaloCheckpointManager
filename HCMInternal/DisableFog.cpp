#include "pch.h"
#include "DisableFog.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "SettingsStateAndEvents.h"
#include "MidhookFlagInterpreter.h"
class DisableFog::DisableFogImpl
{
public:
	virtual ~DisableFogImpl() = default;
};



//
//template<GameState::Value gameT>
//class DisableFogImplSetFlag : public DisableFog::DisableFogImpl
//{
//private:
//	static inline DisableFogImplSetFlag<gameT>* instance = nullptr;
//	ScopedCallback<ToggleEvent> disableFogToggleCallback;
//
//	std::shared_ptr<ModuleMidHook> disableFogHookFunction;
//	std::shared_ptr< MidhookFlagInterpreter> disableFogFlagInterpreter;
//
//	void onDisableFogToggleChanged(bool newValue)
//	{
//		disableFogMidHook->setWantsToBeAttached(newValue);
//	}
//
//	static void disableFogHookFunction(SafetyHookContext& ctx)
//	{
//		if (!instance)
//			return;
//
//
//	}
//
//
//public:
//	DisableFogImplSetFlag(GameState gameImpl, IDIContainer& dicon)
//		: disableFogToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableFogToggle->valueChangedEvent, [this](bool& n) { onDisableFogToggleChanged(n); })
//	{
//		instance = this;
//		auto ptr = dicon.Resolve<PointerDataStore>().lock();
//
//		auto disableFogPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction), gameImpl);
//		disableFogMidHook = ModuleMidHook::make(gameImpl.toModuleName(), disableFogPatchFunction, );
//	}
//
//	~DisableFogImplSetFlag()
//	{
//		instance = nullptr;
//	}
//};


template<GameState::Value gameT>
class DisableFogImplPatch : public DisableFog::DisableFogImpl
{
private:
	ScopedCallback<ToggleEvent> disableFogToggleCallback;

	std::shared_ptr<ModulePatch> disableFogPatchHook;

	void onDisableFogToggleChanged(bool newValue)
	{
		disableFogPatchHook->setWantsToBeAttached(newValue);
	}


public:
	DisableFogImplPatch(GameState gameImpl, IDIContainer& dicon)
		: disableFogToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableFogToggle->valueChangedEvent, [this](bool& n) { onDisableFogToggleChanged(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableFogPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction), gameImpl);
		auto disableFogPatchCode = ptr->getVectorData<byte>(nameof(disableFogPatchCode), gameImpl);
		disableFogPatchHook = ModulePatch::make(gameImpl.toModuleName(), disableFogPatchFunction, *disableFogPatchCode.get());
	}
};




template<GameState::Value gameT>
class DisableFogImplNopCall : public DisableFog::DisableFogImpl
{
private:
	ScopedCallback<ToggleEvent> disableFogToggleCallback;

	std::shared_ptr<ModulePatch> disableFogPatchHook;

	void onDisableFogToggleChanged(bool newValue)
	{
		disableFogPatchHook->setWantsToBeAttached(newValue);
	}


public:
	DisableFogImplNopCall(GameState gameImpl, IDIContainer& dicon)
		: disableFogToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableFogToggle->valueChangedEvent, [this](bool& n) { onDisableFogToggleChanged(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableFogPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction), gameImpl);
		disableFogPatchHook = ModulePatch::makeNOPCall(gameImpl.toModuleName(), disableFogPatchFunction);
	}
};

// Halo 1 is a module patch + a module nop call patch
class DisableFogImplHalo1 : public DisableFog::DisableFogImpl
{
private:
	ScopedCallback<ToggleEvent> disableFogToggleCallback;

	std::shared_ptr<ModulePatch> disableFogPatchHook;
	std::shared_ptr<ModulePatch> disableFogPatchHook2;

	void onDisableFogToggleChanged(bool newValue)
	{
		disableFogPatchHook->setWantsToBeAttached(newValue);
		disableFogPatchHook2->setWantsToBeAttached(newValue);
	}


public:
	DisableFogImplHalo1(GameState gameImpl, IDIContainer& dicon)
		: disableFogToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableFogToggle->valueChangedEvent, [this](bool& n) { onDisableFogToggleChanged(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableFogPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction), gameImpl);
		auto disableFogPatchCode = ptr->getVectorData<byte>(nameof(disableFogPatchCode), gameImpl);
		disableFogPatchHook = ModulePatch::make(gameImpl.toModuleName(), disableFogPatchFunction, *disableFogPatchCode.get());

		auto disableFogPatchFunction2 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction2), gameImpl);
		disableFogPatchHook2 = ModulePatch::makeNOPCall(gameImpl.toModuleName(), disableFogPatchFunction2);
	}
};



// two patches
class DisableFogImplHalo3ODST : public DisableFog::DisableFogImpl
{
private:
	ScopedCallback<ToggleEvent> disableFogToggleCallback;

	std::shared_ptr<ModulePatch> disableFogPatchHook;
	std::shared_ptr<ModulePatch> disableFogPatchHook2;

	void onDisableFogToggleChanged(bool newValue)
	{
		disableFogPatchHook->setWantsToBeAttached(newValue);
		disableFogPatchHook2->setWantsToBeAttached(newValue);
	}


public:
	DisableFogImplHalo3ODST(GameState gameImpl, IDIContainer& dicon)
		: disableFogToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableFogToggle->valueChangedEvent, [this](bool& n) { onDisableFogToggleChanged(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableFogPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction), gameImpl);
		auto disableFogPatchCode = ptr->getVectorData<byte>(nameof(disableFogPatchCode), gameImpl);
		disableFogPatchHook = ModulePatch::make(gameImpl.toModuleName(), disableFogPatchFunction, *disableFogPatchCode.get());

		auto disableFogPatchFunction2 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction2), gameImpl);
		auto disableFogPatchCode2 = ptr->getVectorData<byte>(nameof(disableFogPatchCode2), gameImpl);
		disableFogPatchHook2 = ModulePatch::make(gameImpl.toModuleName(), disableFogPatchFunction2, *disableFogPatchCode2.get());
	}
};

// two nop patches
class DisableFogImplHaloReach : public DisableFog::DisableFogImpl
{
private:
	ScopedCallback<ToggleEvent> disableFogToggleCallback;

	std::shared_ptr<ModulePatch> disableFogPatchHook;
	std::shared_ptr<ModulePatch> disableFogPatchHook2;

	void onDisableFogToggleChanged(bool newValue)
	{
		disableFogPatchHook->setWantsToBeAttached(newValue);
		disableFogPatchHook2->setWantsToBeAttached(newValue);
	}


public:
	DisableFogImplHaloReach(GameState gameImpl, IDIContainer& dicon)
		: disableFogToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableFogToggle->valueChangedEvent, [this](bool& n) { onDisableFogToggleChanged(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableFogPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction), gameImpl);
		disableFogPatchHook = ModulePatch::makeNOPCall(gameImpl.toModuleName(), disableFogPatchFunction);

		auto disableFogPatchFunction2 = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableFogPatchFunction2), gameImpl);
		disableFogPatchHook2 = ModulePatch::makeNOPCall(gameImpl.toModuleName(), disableFogPatchFunction2);
	}
};


DisableFog::DisableFog(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<DisableFogImplHalo1>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<DisableFogImplPatch<GameState::Value::Halo2>>(gameImpl, dicon);
		break;


	case GameState::Value::Halo3:
		pimpl = std::make_unique<DisableFogImplPatch<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<DisableFogImplHalo3ODST>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<DisableFogImplHaloReach>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<DisableFogImplNopCall<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet!");
	}
}

DisableFog::~DisableFog() = default;