#include "pch.h"
#include "BSPSetChangeHookEvent.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "GetCurrentBSPSet.h"
#include "IMakeOrGetCheat.h"
#include "SwitchBSPSet.h"
#include "ObservedEventFactory.h"

template<GameState::Value gameT>
class BSPSetChangeHookEventTemplated : public IBSPSetChangeHookEvent
{
private:
	static inline BSPSetChangeHookEventTemplated<gameT>* instance = nullptr;

	std::shared_ptr< GetCurrentBSPSet> getCurrentBSPSet;
	std::shared_ptr<ModuleMidHook> BSPSetChangeHook;
	std::shared_ptr<ObservedEvent<eventpp::CallbackList<void(BSPSet)>>> BSPSetChangeEvent = ObservedEventFactory::makeObservedEvent<eventpp::CallbackList<void(BSPSet)>>();
	std::unique_ptr<ScopedCallback<ActionEvent>> callbackListChangedEvent;


	std::unique_ptr<ScopedCallback< eventpp::CallbackList<void(BSPSet)>>> forceBSPSetChangeEvent;


	void onCallbackListChanged()
	{
		BSPSetChangeHook->setWantsToBeAttached(BSPSetChangeEvent->isEventSubscribed());
	}


	static void BSPSetChangeHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		try
		{
			instance->BSPSetChangeEvent->fireEvent(instance->getCurrentBSPSet->getCurrentBSPSet());
		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << ex.what();
		}


	}
public:
	BSPSetChangeHookEventTemplated(GameState game, IDIContainer& dicon)
		: getCurrentBSPSet(resolveDependentCheat(GetCurrentBSPSet))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto BSPSetChangeFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(BSPSetChangeFunction), game);
		BSPSetChangeHook = ModuleMidHook::make(game.toModuleName(), BSPSetChangeFunction, BSPSetChangeHookFunction);
		instance = this;

		callbackListChangedEvent = ObservedEventFactory::getCallbackListChangedCallback(BSPSetChangeEvent, [this]() {onCallbackListChanged(); });

		// subscribe to SwitchBSPSet event that it fires when the user forces a BSP set change
		try
		{
			auto SwitchBSPSetLock = resolveDependentCheat(SwitchBSPSet);
			forceBSPSetChangeEvent = std::make_unique<ScopedCallback< eventpp::CallbackList<void(BSPSet)>>>(SwitchBSPSetLock->getBSPSetChangeEvent(), [this](BSPSet n) { BSPSetChangeEvent->fireEvent(n); });
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Failed to resolve SwitchBSPSet, contuining anyway";
		}

	}

	std::shared_ptr<ObservedEvent<eventpp::CallbackList<void(BSPSet)>>> getBSPSetChangeEvent() {
		return BSPSetChangeEvent;
	}

	~BSPSetChangeHookEventTemplated()
	{
		instance = nullptr;
	}
};

BSPSetChangeHookEvent::BSPSetChangeHookEvent(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		throw HCMInitException("BSPSet not applicable to h1");
		break;

	case GameState::Value::Halo2:
		throw HCMInitException("BSPSet not applicable to h2");
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<BSPSetChangeHookEventTemplated<GameState::Value::Halo3>>(game, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<BSPSetChangeHookEventTemplated<GameState::Value::Halo3ODST>>(game, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<BSPSetChangeHookEventTemplated<GameState::Value::HaloReach>>(game, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<BSPSetChangeHookEventTemplated<GameState::Value::Halo4>>(game, dicon);
		break;

	default:
		throw HCMInitException("Not impl yet");
	}
}
BSPSetChangeHookEvent::~BSPSetChangeHookEvent() = default;

