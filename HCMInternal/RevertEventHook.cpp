#include "pch.h"
#include "RevertEventHook.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "RuntimeExceptionHandler.h"


template <GameState::Value gameT>
class RevertEventHookTemplated : public RevertEventHook::RevertEventHookImpl
{
private:
	static inline std::atomic_bool revertHookRunningMutex = false;
	static inline RevertEventHookTemplated<gameT>* instance = nullptr;
	std::unique_ptr<ModuleMidHook> revertHook;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr<ObservedEvent<ActionEvent>> revertEvent;

	void onRevertEventCallbackListChanged()
	{
		revertHook->setWantsToBeAttached(revertEvent->isEventSubscribed());
	}

	static void revertHookFunction(SafetyHookContext& ctx)
	{
		if (!instance) { PLOG_ERROR << "null RevertEventHookTemplated instance"; return; }
		ScopedAtomicBool lock(revertHookRunningMutex);
		try
		{
			instance->revertEvent->fireEvent();
		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
		}

	}

public:
	RevertEventHookTemplated(GameState game, IDIContainer& dicon)
	{
		if (instance) throw HCMInitException("Cannot have more than one GameTickEventHookTemplated per game");

		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto revertFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(revertFunction), game);
		revertHook = ModuleMidHook::make(game.toModuleName(), revertFunction, revertHookFunction);

		instance = this;
	}

	~RevertEventHookTemplated()
	{
		PLOG_DEBUG << "~RevertEventHookTemplated";
		if (revertHookRunningMutex)
		{
			PLOG_INFO << "Waiting for revertHook to finish execution";
			revertHookRunningMutex.wait(true);
		}

		revertHook.reset();

	}

	std::shared_ptr<ObservedEvent<ActionEvent>> getRevertEvent()
	{
		return revertEvent;
	}
};

RevertEventHook::RevertEventHook(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<RevertEventHookTemplated<GameState::Value::Halo1>>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<RevertEventHookTemplated<GameState::Value::Halo2>>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<RevertEventHookTemplated<GameState::Value::Halo3>>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<RevertEventHookTemplated<GameState::Value::Halo3ODST>>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<RevertEventHookTemplated<GameState::Value::HaloReach>>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<RevertEventHookTemplated<GameState::Value::Halo4>>(game, dicon); break;
	}
}
RevertEventHook::~RevertEventHook() = default;
std::shared_ptr<ObservedEvent<ActionEvent>> RevertEventHook::getRevertEvent() { return pimpl->getRevertEvent(); }