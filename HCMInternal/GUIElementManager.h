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
	void constructGUIElementCollection(std::vector<GameState> supportedGames, std::shared_ptr<GUIElementCollection> GUICollection, Args... constructorArgs)
	{
		for (auto game : supportedGames)
		{
			GUICollection.get()->emplace(game, std::make_shared<T>(game, constructorArgs...));
		}
	}

	std::shared_ptr<GUIElementCollection> forceCheckpointGUI = std::make_shared<GUIElementCollection>(); // starts empty


	std::set< std::shared_ptr<GUIElementCollection>> allGUIElements {forceCheckpointGUI}; // deffo public, this is what HCMInternalGUI iterates over

public:

	static std::set< std::shared_ptr<GUIElementCollection>>& getAllGUIElements() { return instance->allGUIElements; }


	GUIElementManager()
	{
		if (instance) throw HCMInitException("Cannot have more than one GUIElementManager");
		instance = this;

		// how to make sure the event is passed by ref and not value? 
		constructGUIElementCollection<GUISimpleButton>(AllSupportedGames, forceCheckpointGUI, std::vector{CheatManager::forceCheckpointCollection}, "Force Checkpoint", OptionsState::forceCheckpointEvent );
	}


};