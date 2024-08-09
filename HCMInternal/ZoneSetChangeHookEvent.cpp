#include "pch.h"
#include "ZoneSetChangeHookEvent.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "GetCurrentZoneSet.h"
#include "IMakeOrGetCheat.h"


template<GameState::Value gameT>
class ZoneSetChangeHookEventTemplated : public IZoneSetChangeHookEvent
{
private:
	static inline ZoneSetChangeHookEventTemplated<gameT>* instance = nullptr;

	std::shared_ptr< GetCurrentZoneSet> getCurrentZoneSet;
	std::shared_ptr<ModuleMidHook> ZoneSetChangeHook;
	std::shared_ptr<ObservedEvent<eventpp::CallbackList<void(uint32_t)>>> ZoneSetChangeEvent;

	void onCallbackListChanged()
	{
		ZoneSetChangeHook->setWantsToBeAttached(ZoneSetChangeEvent->isEventSubscribed());
	}


	static void ZoneSetChangeHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		try
		{
			instance->ZoneSetChangeEvent->fireEvent(instance->getCurrentZoneSet->getCurrentZoneSet());
		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << ex.what();
		}


	}
public:
	ZoneSetChangeHookEventTemplated(GameState game, IDIContainer& dicon)
		: getCurrentZoneSet(resolveDependentCheat(GetCurrentZoneSet)),
		ZoneSetChangeEvent(std::make_shared<ObservedEvent<eventpp::CallbackList<void(uint32_t)>>>([this]() {onCallbackListChanged(); }))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto ZoneSetChangeFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(ZoneSetChangeFunction), game);
		ZoneSetChangeHook = ModuleMidHook::make(game.toModuleName(), ZoneSetChangeFunction, ZoneSetChangeHookFunction);
		instance = this;


	}

	virtual std::shared_ptr<ObservedEvent<eventpp::CallbackList<void(uint32_t)>>> getZoneSetChangeEvent() override {
		return ZoneSetChangeEvent;
	}

	~ZoneSetChangeHookEventTemplated()
	{
		instance = nullptr;
	}
};

ZoneSetChangeHookEvent::ZoneSetChangeHookEvent(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		throw HCMInitException("ZoneSet not applicable to h1");
		break;

	case GameState::Value::Halo2:
		throw HCMInitException("ZoneSet not applicable to h2");
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<ZoneSetChangeHookEventTemplated<GameState::Value::Halo3>>(game, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<ZoneSetChangeHookEventTemplated<GameState::Value::Halo3ODST>>(game, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<ZoneSetChangeHookEventTemplated<GameState::Value::HaloReach>>(game, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<ZoneSetChangeHookEventTemplated<GameState::Value::Halo4>>(game, dicon);
		break;

	default:
		throw HCMInitException("Not impl yet");
	}
}
ZoneSetChangeHookEvent::~ZoneSetChangeHookEvent() = default;
