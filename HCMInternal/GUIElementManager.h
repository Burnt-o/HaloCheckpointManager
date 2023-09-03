#pragma once
#include "GUIElementBase.h"
#include "CheatManager.h"
#include "GUISimpleButton.h"
#include "GUISpeedhack.h"
#include "OptionsState.h"
#include "Hotkeys.h"

class GUIElementManager
{
private:
	typedef std::map<GameState, std::shared_ptr<GUIElementBase>> GUIElementCollection;

	static inline GUIElementManager* instance = nullptr;

	template <GUIElementBaseTemplate T, typename... Args>
	void constructGUIElementCollection(std::vector<GameState> supportedGames, std::shared_ptr<GUIElementCollection> GUICollection, Args... constructorArgs)
	{
		for (auto game : supportedGames)
		{
			GUICollection.get()->emplace(game, std::make_shared<T>(game, constructorArgs...));
		}
		allGUICollections.emplace_back(GUICollection);

	}

	// all collections start empty
	std::shared_ptr<GUIElementCollection> forceCheckpointGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> forceRevertGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> forceDoubleRevertGUI = std::make_shared<GUIElementCollection>(); 

	std::shared_ptr<GUIElementCollection> forceCoreSaveGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> forceCoreLoadGUI = std::make_shared<GUIElementCollection>(); 

	std::shared_ptr<GUIElementCollection> injectCheckpointGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> dumpCheckpointGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> injectCoreGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> dumpCoreGUI = std::make_shared<GUIElementCollection>(); 
	std::shared_ptr<GUIElementCollection> speedhackGUI = std::make_shared<GUIElementCollection>();


	std::vector< std::shared_ptr<GUIElementCollection>> allGUICollections {}; // deffo public, this is what HCMInternalGUI iterates over

public:

	static std::vector< std::shared_ptr<GUIElementCollection>>& getAllGUICollections() { return instance->allGUICollections; }


	GUIElementManager()
	{
		if (instance) throw HCMInitException("Cannot have more than one GUIElementManager");
		instance = this;

		std::optional<std::shared_ptr<Hotkey>> nullhotkey = {};

		constructGUIElementCollection<GUISimpleButton>(AllSupportedGames, injectCheckpointGUI, std::vector{CheatManager::injectCheckpointCollection}, nullhotkey, "Inject Checkpoint", OptionsState::injectCheckpointEvent);
		constructGUIElementCollection<GUISimpleButton>(AllSupportedGames, dumpCheckpointGUI, std::vector{CheatManager::dumpCheckpointCollection}, nullhotkey, "Dump Checkpoint", OptionsState::dumpCheckpointEvent);

		constructGUIElementCollection<GUISimpleButton>({ GameState::Value::Halo1 }, injectCoreGUI, std::vector{CheatManager::injectCoreCollection}, nullhotkey, "Inject Core", OptionsState::injectCoreEvent);
		constructGUIElementCollection<GUISimpleButton>({ GameState::Value::Halo1 }, dumpCoreGUI, std::vector{CheatManager::dumpCoreCollection}, nullhotkey,  "Dump Core", OptionsState::dumpCoreEvent);

		constructGUIElementCollection<GUISimpleButton>(AllSupportedGames, forceCheckpointGUI, std::vector{CheatManager::forceCheckpointCollection}, Hotkeys::forceCheckpoint, "Force Checkpoint", OptionsState::forceCheckpointEvent );
		constructGUIElementCollection<GUISimpleButton>(AllSupportedGames, forceRevertGUI, std::vector{CheatManager::forceRevertCollection}, nullhotkey, "Force Revert", OptionsState::forceRevertEvent);
		constructGUIElementCollection<GUISimpleButton>(DoubleCheckpointGames, forceDoubleRevertGUI, std::vector{CheatManager::forceCheckpointCollection}, nullhotkey, "Force Double Revert", OptionsState::forceDoubleRevertEvent);

		constructGUIElementCollection<GUISimpleButton>({ GameState::Value::Halo1 }, forceCoreSaveGUI, std::vector{CheatManager::forceCoreSaveCollection}, nullhotkey, "Force Core Save", OptionsState::forceCoreSaveEvent);
		constructGUIElementCollection<GUISimpleButton>({ GameState::Value::Halo1 }, forceCoreLoadGUI, std::vector{CheatManager::forceCoreLoadCollection}, nullhotkey, "Force Core Load", OptionsState::forceCoreLoadEvent);

		constructGUIElementCollection<GUISpeedhack>(AllSupportedGames, speedhackGUI, std::vector{CheatManager::speedhackCollection}, nullhotkey);

	}


};