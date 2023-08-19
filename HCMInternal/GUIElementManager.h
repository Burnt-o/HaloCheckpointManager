#pragma once
#include "GUIElementBase.h"
#include "CheatManager.h"
#include "GUISimpleButton.h"
#include "OptionsState.h"

class GUIElementManager
{
private:
	typedef std::map<GameState, std::shared_ptr<GUIElementBase>> GUIElementCollection;

	static GUIElementManager* instance;

	template <GUIElementBaseTemplate T, typename... Args>
	void constructGUIElementCollection(std::set<GameState> supportedGames, GUIElementCollection& GUICollection, Args... constructorArgs)
	{
		for (auto game : supportedGames)
		{
			GUICollection.emplace(game, std::make_shared<T>(game, constructorArgs...));
		}
	}

	GUIElementCollection forceCheckpointGUI; // can't decide if this should be private or not. will anything else need to grab these things specifically instead of iterating over all elements?


	std::set< GUIElementCollection*> allGUIElements {&forceCheckpointGUI}; // deffo public, this is what HCMInternalGUI iterates over

public:

	static std::set< GUIElementCollection*>& getAllGUIElements() { return instance->allGUIElements; }


	GUIElementManager()
	{
		if (instance) throw HCMInitException("Cannot have more than one GUIElementManager");
		instance = this;

		constructGUIElementCollection<GUISimpleButton>({ GameState::Halo1 }, forceCheckpointGUI, CheatManager::forceCheckpointCollection, "Force Checkpoint", OptionsState::forceCheckpointEvent);
	}


};