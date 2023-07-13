#pragma once
#include "ModuleHookManager.h"
#include "MultilevelPointer.h"
#include "HaloEnums.h"
#include "PointerManager.h"

// Evaluates which game MCC is currently running + which level is loaded
// and fires event when either changes
class GameStateHook
{
private:
	static GameStateHook* instance; // Private Singleton instance so static hooks/callbacks can access
	std::mutex mDestructionGuard; // Protects against Singleton destruction while callbacks are executing

	GameState currentGameState = GameState::Halo1;
	std::string currentHaloLevel = "Unknown";

	//pointers + hooks
	std::unique_ptr<ModuleMidHook> gameLoadHook;	// Hook that invokes event
	std::shared_ptr<MultilevelPointer> gameIndicator;
	std::shared_ptr<MultilevelPointer> globalLevelName;

	static GameState parseGameState();
	static std::string parseHaloLevel();

	// Function we run when hook hit
	static void gameLoadHookFunction(SafetyHookContext ctx)
	{
		PLOG_VERBOSE << "gameLoadHookFunction running";
		std::scoped_lock<std::mutex> lock(instance->mDestructionGuard);

		try 
		{
			instance->currentGameState = parseGameState();
			instance->currentHaloLevel = parseHaloLevel();
		}
		catch (HCMRuntimeException& ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
			return;
		}


		// Fire event
		instance->gameLoadEvent(instance->currentGameState, instance->currentHaloLevel);
	}

public:

	explicit GameStateHook()
	{
		if (instance != nullptr)
		{
			throw HCMInitException("Cannot have more than one GameStateHook");
		}
		instance = this;


		// Get pointers
		try
		{
			gameIndicator = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("gameIndicator");
			globalLevelName = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("globalLevelName");
			auto gameLoadFunction = PointerManager::getData<std::shared_ptr<MultilevelPointer>>("gameLoadFunction");

			// Set up the hook 
			gameLoadHook = ModuleMidHook::make(
				L"main",
				gameLoadFunction,
				(safetyhook::MidHookFn)&gameLoadHookFunction,
				true);
		}
		catch (HCMInitException& ex)
		{
			ex.prepend("GameStateHook could not resolve hooks: ");
			throw ex;
		}

	}

	static GameState getCurrentGameState() { return instance->currentGameState; }
	static std::string_view getCurrentHaloLevel() { return instance->currentHaloLevel; }

	eventpp::CallbackList<void(GameState, std::string)> gameLoadEvent;

	~GameStateHook()
	{
		std::scoped_lock<std::mutex> lock(instance->mDestructionGuard);
		instance = nullptr;
	}
};

