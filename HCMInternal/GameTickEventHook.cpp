#include "pch.h"
#include "GameTickEventHook.h"
#include "ModuleHook.h"
#include "PointerManager.h"



template <GameState::Value gameT>
class GameTickEventHookTemplated : public GameTickEventHook::GameTickEventHookImpl
{
private:
	GameState mGame;

	static inline std::atomic_bool gameTickHookRunning = false;
	static inline GameTickEventHookTemplated<gameT>* instance = nullptr;

	std::shared_ptr<eventpp::CallbackList<void(int)>> gameTickEvent = std::make_shared<eventpp::CallbackList<void(int)>>();

	std::unique_ptr<ModuleMidHook> tickIncrementHook;
	std::shared_ptr<MultilevelPointer> tickCounter;

	static void tickIncrementHookFunction(SafetyHookContext& ctx)
	{
		if (!instance) { PLOG_ERROR << "null GameTickEventHookTemplated instance"; return; }
		ScopedAtomicBool lock(gameTickHookRunning);
		int tickCount;
		if (!instance->tickCounter->readData(&tickCount))
		{
			LOG_ONCE(PLOG_ERROR << "tickIncrementHookFunction failed to resolve tickCounter");
			instance->gameTickEvent->operator()(0);
		}
		else
		{
			instance->gameTickEvent->operator()(tickCount);
		}

	}

public:
	GameTickEventHookTemplated(GameState game, IDIContainer& dicon) : mGame(game)
	{
		if (instance) throw HCMInitException("Cannot have more than one GameTickEventHookTemplated per game");
		auto ptr = dicon.Resolve<PointerManager>().lock();
		auto tickIncrementFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickIncrementFunction), game);
		tickCounter = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickCounter), game);
		tickIncrementHook = ModuleMidHook::make(game.toModuleName(), tickIncrementFunction, tickIncrementHookFunction); 
		// hook starts disabled, will be enabled on first call to getGameTickEvent

		instance = this;
	}

	std::shared_ptr<eventpp::CallbackList<void(int)>> getGameTickEvent()
	{
		tickIncrementHook->setWantsToBeAttached(true);
		return gameTickEvent;
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

std::shared_ptr<eventpp::CallbackList<void(int)>> GameTickEventHook::getGameTickEvent() { return pimpl->getGameTickEvent();  }