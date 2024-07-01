#include "pch.h"
#include "GameTickEventHook.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"

template <GameState::Value gameT>
class GameTickEventHookTemplated : public GameTickEventHook::GameTickEventHookImpl
{
private:
	GameState mGame;

	static inline std::atomic_bool gameTickHookRunning = false;
	static inline GameTickEventHookTemplated<gameT>* instance = nullptr;

	std::shared_ptr<eventpp::CallbackList<void(uint32_t)>> gameTickEvent = std::make_shared<eventpp::CallbackList<void(uint32_t)>>();
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	ScopedCallback < eventpp::CallbackList<void(const MCCState&)>> mccStateChangedCallback;

	void onMccStateChangedCallback(const MCCState&)
	{
		cacheValid = false;
	}

	std::unique_ptr<ModuleMidHook> tickIncrementHook;
	std::shared_ptr<MultilevelPointer> tickCounter;
	uint32_t* cachedTickCounter;
	bool cacheValid = false;

	static void tickIncrementHookFunction(SafetyHookContext& ctx)
	{
		static int errorCount = 0;
		if (!instance) { PLOG_ERROR << "null GameTickEventHookTemplated instance"; return; }
		ScopedAtomicBool lock(gameTickHookRunning);
		try
		{
			instance->gameTickEvent->operator()(instance->getCurrentGameTick());
			errorCount = 0;
		}
		catch (HCMRuntimeException ex)
		{
			errorCount++;
			PLOG_ERROR << "Error in game tick hook. This is only a problem if it happens repeatedly. Error details: " << ex.what();
			if (errorCount == 10)
			{
				ex.append("10x\n");
				instance->runtimeExceptions->handleMessage(ex);
			}
		}

	}

public:
	GameTickEventHookTemplated(GameState game, IDIContainer& dicon) 
		: 
		mGame(game),
		mccStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onMccStateChangedCallback(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock())
	{
		if (instance) throw HCMInitException("Cannot have more than one GameTickEventHookTemplated per game");
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto tickIncrementFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickIncrementFunction), game);
		tickCounter = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickCounter), game);
		tickIncrementHook = ModuleMidHook::make(game.toModuleName(), tickIncrementFunction, tickIncrementHookFunction); 
		// hook starts disabled, will be enabled on first call to getGameTickEvent

		instance = this;
	}

	std::shared_ptr<eventpp::CallbackList<void(uint32_t)>> getGameTickEvent()
	{
		tickIncrementHook->setWantsToBeAttached(true);
		return gameTickEvent;
	}

	virtual uint32_t getCurrentGameTick() override
	{
		if (cacheValid == false)
		{
			uintptr_t temp;
			if (tickCounter->resolve(&temp) == false) throw HCMRuntimeException(std::format("Could not resolve tickcounter: {}", MultilevelPointer::GetLastError()));
			cachedTickCounter = (uint32_t*)temp;
		}

		return *cachedTickCounter;
	}

	~GameTickEventHookTemplated()
	{
		PLOG_DEBUG << "~GameTickEventHookTemplated";
		if (gameTickHookRunning)
		{
			PLOG_INFO << "Waiting for gameTickHook to finish execution";
			gameTickHookRunning.wait(true);
		}

		safetyhook::ThreadFreezer threadFreezer; // freeze threads while we unhook
		tickIncrementHook.reset();


	}
};


GameTickEventHook::GameTickEventHook(GameState game, IDIContainer& dicon)
{
	std::lock_guard<std::mutex> lock(constructionMutex);
	if (pimpl) return;

	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<GameTickEventHookTemplated<GameState::Value::Halo1>>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<GameTickEventHookTemplated<GameState::Value::Halo2>>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<GameTickEventHookTemplated<GameState::Value::Halo3>>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<GameTickEventHookTemplated<GameState::Value::Halo3ODST>>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<GameTickEventHookTemplated<GameState::Value::HaloReach>>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<GameTickEventHookTemplated<GameState::Value::Halo4>>(game, dicon); break;
	}

}

GameTickEventHook::~GameTickEventHook()
{
	PLOG_DEBUG << "~" << getName();
}

std::shared_ptr<eventpp::CallbackList<void(uint32_t)>> GameTickEventHook::getGameTickEvent() { return pimpl->getGameTickEvent();  }

uint32_t GameTickEventHook::getCurrentGameTick() { return pimpl->getCurrentGameTick(); }