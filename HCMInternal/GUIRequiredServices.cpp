#include "pch.h"
#include "GUIRequiredServices.h"


// ALL GUI ELEMENTS, nested or not, are allowed to go in here - but if they have no required services, you don't have to
const std::map <GUIElementEnum, std::vector<OptionalCheatEnum>> GUIRequiredServices::requiredServicesPerGUIElement =
{
	{GUIElementEnum::forceCheckpointGUI, 
			{OptionalCheatEnum::ForceCheckpoint} 
	},
	{GUIElementEnum::forceRevertGUI,
			{OptionalCheatEnum::ForceRevert} 
	},
	{GUIElementEnum::forceDoubleRevertGUI,
			{OptionalCheatEnum::ForceDoubleRevert} 
	},
	{GUIElementEnum::forceCoreSaveGUI,
			{OptionalCheatEnum::ForceCoreSave} 
	},
	{GUIElementEnum::forceCoreLoadGUI,
			{OptionalCheatEnum::ForceCoreLoad} 
	},
	{GUIElementEnum::injectCheckpointGUI,
			{OptionalCheatEnum::InjectCheckpoint} 
	},
	{GUIElementEnum::dumpCheckpointGUI,
			{OptionalCheatEnum::DumpCheckpoint} 
	},
	{GUIElementEnum::injectCoreGUI,
			{OptionalCheatEnum::InjectCore} 
	},
	{GUIElementEnum::dumpCoreGUI,
			{OptionalCheatEnum::DumpCore} 
	},
	{GUIElementEnum::speedhackGUI,
			{OptionalCheatEnum::Speedhack} 
	},
	{GUIElementEnum::invulnGUI,
			{OptionalCheatEnum::Invulnerability} 
	},

};

// all gui elements must go in here. the bool is whether the element is a top-level one or not
const std::map< GUIElementEnum, std::pair<std::vector<GameState>, bool>> GUIRequiredServices::GUIElementGamesAndTopness =
{
	{GUIElementEnum::forceCheckpointGUI, {
	AllSupportedGames,
	true
	}},
	{GUIElementEnum::forceRevertGUI, {
	AllSupportedGames,
	true
	}},
	{GUIElementEnum::forceDoubleRevertGUI, {
	AllSupportedGames,
	true
	}},
	{GUIElementEnum::forceCoreSaveGUI, {
	{GameState::Value::Halo1},
	true
	}},
	{GUIElementEnum::forceCoreLoadGUI, {
	{GameState::Value::Halo1},
	true
	}},
	{GUIElementEnum::injectCheckpointGUI, {
	AllSupportedGames,
	true
	}},
	{GUIElementEnum::dumpCheckpointGUI, {
	AllSupportedGames,
	true
	}},
	{GUIElementEnum::injectCoreGUI, {
	{GameState::Value::Halo1},
	true
	}},
	{GUIElementEnum::dumpCoreGUI, {
	{GameState::Value::Halo1},
	true
	}},
	{GUIElementEnum::speedhackGUI, {
	AllSupportedGames,
	true
	}},
	{GUIElementEnum::invulnGUI, {
	AllSupportedGames,
	true
	}},

};

std::vector<std::pair<GameState, OptionalCheatEnum>> outRequiredServices;
bool initRequiredServices = false;
const std::vector<std::pair<GameState, OptionalCheatEnum>>& GUIRequiredServices::getAllRequiredServices()
{
	if (initRequiredServices) return outRequiredServices;

	for (auto [element, gamesTopnessPair] : GUIElementGamesAndTopness) 
	{
		for (auto game : gamesTopnessPair.first)
		{
			if (!requiredServicesPerGUIElement.contains(element)) continue; // element has no dependencies

			auto reqServs = requiredServicesPerGUIElement.at(element);

			for (auto req : reqServs)
			{
				outRequiredServices.push_back({ game, req });
			}

		}

	}
	initRequiredServices = true;
	return outRequiredServices;
};