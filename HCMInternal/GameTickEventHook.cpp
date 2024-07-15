#include "pch.h"
#include "GameTickEventHook.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "RuntimeExceptionHandler.h"
#include "ObservedEventFactory.h"
template <GameState::Value gameT>
class GameTickEventHookTemplated : public GameTickEventHook::GameTickEventHookImpl
{
private:
	GameState mGame;

	static inline std::atomic_bool gameTickHookRunningMutex = false;
	static inline GameTickEventHookTemplated<gameT>* instance = nullptr;

	std::shared_ptr<ObservedEvent<GameTickEvent>> gameTickEvent;
	std::unique_ptr<ScopedCallback<ActionEvent>> gameTickEventCallbackListChanged;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	std::unique_ptr<ModuleMidHook> tickIncrementHook;
	std::shared_ptr<MultilevelPointer> tickCounter;


	void onGameTickEventCallbackListChanged()
	{
		tickIncrementHook->setWantsToBeAttached(gameTickEvent->isEventSubscribed());
	}

	static void tickIncrementHookFunction(SafetyHookContext& ctx)
	{
		static int errorCount = 0;
		if (!instance) { PLOG_ERROR << "null GameTickEventHookTemplated instance"; return; }
		ScopedAtomicBool lock(gameTickHookRunningMutex);
		try
		{
			instance->gameTickEvent->fireEvent(instance->getCurrentGameTick());
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
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock())
	{
		if (instance) throw HCMInitException("Cannot have more than one GameTickEventHookTemplated per game");
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto tickIncrementFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickIncrementFunction), game);
		tickCounter = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickCounter), game);
		tickIncrementHook = ModuleMidHook::make(game.toModuleName(), tickIncrementFunction, tickIncrementHookFunction); 


		gameTickEvent = ObservedEventFactory::makeObservedEvent<GameTickEvent>();
		gameTickEventCallbackListChanged = ObservedEventFactory::getCallbackListChangedCallback(gameTickEvent, [this]() {onGameTickEventCallbackListChanged(); });


		instance = this;
	}

	std::shared_ptr<ObservedEvent<GameTickEvent>> getGameTickEvent()
	{
		return gameTickEvent;
	}

	virtual uint32_t getCurrentGameTick() override
	{
			uint32_t out;
			if (!tickCounter->readData(&out)) throw HCMRuntimeException(std::format("Could not resolve tickcounter: {}", MultilevelPointer::GetLastError()));
			return out;
	}

	~GameTickEventHookTemplated()
	{
		PLOG_DEBUG << "~GameTickEventHookTemplated";
		if (gameTickHookRunningMutex)
		{
			PLOG_INFO << "Waiting for gameTickHook to finish execution";
			gameTickHookRunningMutex.wait(true);
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

std::shared_ptr<ObservedEvent<GameTickEvent>> GameTickEventHook::getGameTickEvent() { return pimpl->getGameTickEvent();  }

uint32_t GameTickEventHook::getCurrentGameTick() { return pimpl->getCurrentGameTick(); }