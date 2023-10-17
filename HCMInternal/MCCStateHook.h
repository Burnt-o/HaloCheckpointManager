#pragma once
#include "ModuleHookManager.h"
#include "MultilevelPointer.h"
#include "GameState.h"
#include "PointerManager.h"
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
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;

	//data
	MCCState currentMCCState;

	// hooks just call updateMCCState() which fires the MCCStateChanged event
	std::unique_ptr<ModuleMidHook> gameLoadStartHook; // hit when loading screen starts
	std::unique_ptr<ModuleMidHook> gameLoadEndHook;	// hit when loading screen ends
	std::unique_ptr<ModuleMidHook> gameQuitHook;	// hit when quiting to menu from game


	std::shared_ptr<MultilevelPointer> gameIndicator;
	std::shared_ptr<MultilevelPointer> loadIndicator;
	std::shared_ptr<MultilevelPointer> menuIndicator;
	std::shared_ptr<MultilevelPointer> levelIndicator;

	void updateMCCState();

	static void loadHookFunction(SafetyHookContext ctx)
	{
		//auto guard = instance->shared_from_this();
		instance->updateMCCState();
	}

	std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedEvent = std::make_shared<eventpp::CallbackList<void(const MCCState&)>>();
public:

	explicit MCCStateHook(std::shared_ptr<PointerManager> ptrMan, std::shared_ptr<RuntimeExceptionHandler> exp)
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
			gameIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("gameIndicator");
			levelIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("levelIndicator");
			loadIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("loadIndicator");
			menuIndicator = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("menuIndicator");

			auto gameLoadStartFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("gameLoadStartFunction");
			auto gameLoadEndFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("gameLoadEndFunction");
			auto gameQuitFunction = ptrMan->getData<std::shared_ptr<MultilevelPointer>>("gameQuitFunction");
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
		}
		catch (HCMInitException& ex)
		{
			ex.prepend("MCCStateHook could not resolve hooks: ");
			throw ex;
		}

		instance = this;
		updateMCCState();


	}


	virtual const MCCState& getCurrentMCCState() override { return currentMCCState; }
	virtual bool isGameCurrentlyPlaying(GameState gameToCheck) override
	{
		PLOG_VERBOSE << "checking if game is currently playing: " << gameToCheck.toString();
		PLOG_VERBOSE << "actual game currently playing: " << currentMCCState.currentGameState.toString();
		return (currentMCCState.currentGameState == gameToCheck) && (currentMCCState.currentPlayState == PlayState::Ingame); 
	}


	// main event we fire when a new game/level loads
	virtual std::shared_ptr<eventpp::CallbackList<void(const MCCState&)>> getMCCStateChangedEvent() override { return MCCStateChangedEvent; }


	~MCCStateHook()
	{
		PLOG_VERBOSE << "~MCCStateHook";
		instance = nullptr;
	}
};

