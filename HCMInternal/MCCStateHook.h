#pragma once
#include "ModuleHookManager.h"
#include "MultilevelPointer.h"
#include "GameState.h"
#include "PointerDataStore.h"
#include "RuntimeExceptionHandler.h"
#include "MCCState.h"
#include "IMCCStateHook.h"
// Evaluates which game MCC is currently running + which level is loaded
// and fires event when either changes
class MCCStateHook : public std::enable_shared_from_this<MCCStateHook>, public IMCCStateHook
{
private:
	static inline MCCStateHook* instance = nullptr; // Private Singleton instance so static hooks/callbacks can access

	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	MCCState currentMCCState;

	// hooks just call updateMCCState() which fires the MCCStateChanged event
	std::unique_ptr<ModuleMidHook> gameLoadStartHook; // hit when loading screen starts
	std::unique_ptr<ModuleMidHook> gameLoadEndHook;	// hit when loading screen ends
	std::unique_ptr<ModuleMidHook> gameQuitHook;	// hit when quiting to menu from game
	std::unique_ptr<ModuleMidHook> gameEngineChangedHook;	// hit when loaded game engine changes (usually in main menu)


	std::shared_ptr<MultilevelPointer> gameEngineIndicator;
	std::shared_ptr<MultilevelPointer> loadIndicator;
	std::shared_ptr<MultilevelPointer> menuIndicator;
	std::shared_ptr<MultilevelPointer> levelIndicator;

	void updateMCCState();

	static void loadHookFunction(SafetyHookContext ctx)
	{
		instance->updateMCCState();
	}

	std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedEvent = std::make_shared<eventpp::CallbackList<void(const MCCState&)>>();


protected:
	virtual const MCCState& getCurrentMCCState() override { return currentMCCState; }
	virtual bool isGameCurrentlyPlaying(GameState gameToCheck) override
	{
		bool out = (currentMCCState.currentGameState == gameToCheck) && (currentMCCState.currentPlayState == PlayState::Ingame);
#ifdef HCM_DEBUG
		PLOG_VERBOSE << "checking if game is currently playing: " << gameToCheck.toString();
		PLOG_VERBOSE << "actual game currently playing: " << currentMCCState.currentGameState.toString();
		PLOG_VERBOSE << "current playstate: " << magic_enum::enum_name(currentMCCState.currentPlayState);
		PLOG_VERBOSE << "so the answer is: " << (out ? "true" : "false");

		PLOG_VERBOSE << "called by: " << std::to_string(std::stacktrace::current().at(1));
#endif
		return (currentMCCState.currentGameState == gameToCheck) && (currentMCCState.currentPlayState == PlayState::Ingame);
	}


	// main event we fire when a new game/level loads
	virtual std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> getMCCStateChangedEvent() override { return MCCStateChangedEvent; }


public:

	explicit MCCStateHook(std::shared_ptr<PointerDataStore> ptrMan, std::shared_ptr<RuntimeExceptionHandler> exp)
		: runtimeExceptions(exp),
		currentMCCState(GameState::Value::Halo1, PlayState::MainMenu, LevelID::_map_id_halo1_pillar_of_autumn)
	{
		if (instance != nullptr)
		{
			throw HCMInitException("Cannot have more than one MCCStateHook");
		}



		// Get pointers
		try
		{
			gameEngineIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEngineIndicator));
			levelIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(levelIndicator));
			loadIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(loadIndicator));
			menuIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(menuIndicator));

			auto gameLoadStartFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameLoadStartFunction));
			auto gameLoadEndFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameLoadEndFunction));
			auto gameQuitFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameQuitFunction));
			auto gameEngineChangedFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEngineChangedFunction));
			// bind both hooks to same function
			gameLoadStartHook = ModuleMidHook::make(
				L"main",
				gameLoadStartFunction,
				(safetyhook::MidHookFn)&loadHookFunction,
				true);
			gameLoadEndHook = ModuleMidHook::make(
				L"main",
				gameLoadEndFunction,
				(safetyhook::MidHookFn)&loadHookFunction,
				true);
			gameQuitHook = ModuleMidHook::make(
				L"main",
				gameQuitFunction,
				(safetyhook::MidHookFn)&loadHookFunction,
				true);
			gameEngineChangedHook = ModuleMidHook::make(
				L"main",
				gameEngineChangedFunction,
				(safetyhook::MidHookFn)&loadHookFunction,
				true);
		}
		catch (HCMInitException& ex)
		{
			ex.prepend("MCCStateHook could not resolve hooks: ");
			throw ex;
		}

		instance = this;
		updateMCCState();


	}




	~MCCStateHook()
	{
		PLOG_VERBOSE << "~MCCStateHook";
		instance = nullptr;
	}
};

