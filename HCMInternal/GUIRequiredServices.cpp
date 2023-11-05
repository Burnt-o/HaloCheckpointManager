#include "pch.h"
#include "GUIRequiredServices.h"


// ALL GUI ELEMENTS, nested or not, are allowed to go in here - but if they have no required services, you don't have to
const std::map <GUIElementEnum, std::vector<OptionalCheatEnum>> GUIRequiredServices::requiredServicesPerGUIElement =
{
	{GUIElementEnum::GUIShowingPausesGame,
			{OptionalCheatEnum::TogglePause}
	},
	{GUIElementEnum::GUIShowingFreesCursor,
			{OptionalCheatEnum::ToggleFreeCursor}
	},
	{GUIElementEnum::GUIShowingBlocksInput,
			{OptionalCheatEnum::ToggleBlockInput}
	},
	// Above elements don't directly use the optionalCheat but rather the control service container directly. 
	// But the optionalCheats only successfully construct when the control services do.

	{GUIElementEnum::togglePauseGUI,
			{OptionalCheatEnum::TogglePause}
	},
	{GUIElementEnum::advanceTicksGUI,
			{OptionalCheatEnum::AdvanceTicks, OptionalCheatEnum::TogglePause}
	},
	{GUIElementEnum::pauseAlsoFreesCursorGUI,
			{OptionalCheatEnum::TogglePause, OptionalCheatEnum::ToggleFreeCursor}
	},
	{GUIElementEnum::pauseAlsoBlocksInputGUI,
			{OptionalCheatEnum::TogglePause, OptionalCheatEnum::ToggleBlockInput}
	},
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
	{GUIElementEnum::injectCheckpointForcesRevert,
			{OptionalCheatEnum::InjectCheckpoint, OptionalCheatEnum::ForceRevert}
	},
	{GUIElementEnum::injectCheckpointLevelCheck,
			{OptionalCheatEnum::InjectCheckpoint, OptionalCheatEnum::GetCurrentLevelCode}
	},
	{GUIElementEnum::injectCheckpointVersionCheck,
			{OptionalCheatEnum::InjectCheckpoint}
	},
	{GUIElementEnum::injectCheckpointDifficultyCheck,
			{OptionalCheatEnum::InjectCheckpoint, OptionalCheatEnum::GetCurrentDifficulty}
	},
	{GUIElementEnum::injectCheckpointIgnoresChecksum,
			{OptionalCheatEnum::InjectCheckpoint, OptionalCheatEnum::ForceRevert, OptionalCheatEnum::IgnoreCheckpointChecksum}
	},
	{GUIElementEnum::dumpCheckpointGUI,
			{OptionalCheatEnum::DumpCheckpoint} 
	},
	{GUIElementEnum::dumpCheckpointAutonameGUI,
			{OptionalCheatEnum::DumpCheckpoint}
	},
	{GUIElementEnum::dumpCheckpointForcesSave,
			{OptionalCheatEnum::DumpCheckpoint, OptionalCheatEnum::ForceCheckpoint}
	},
	{GUIElementEnum::injectCoreGUI,
			{OptionalCheatEnum::InjectCore} 
	},
	{GUIElementEnum::injectCoreForcesRevert,
			{OptionalCheatEnum::InjectCore, OptionalCheatEnum::ForceCoreLoad}
	},
	{GUIElementEnum::injectCoreLevelCheck,
			{OptionalCheatEnum::InjectCore, OptionalCheatEnum::GetCurrentLevelCode}
	},
	{GUIElementEnum::injectCoreVersionCheck,
			{OptionalCheatEnum::InjectCore}
	},
	{GUIElementEnum::injectCoreDifficultyCheck,
			{OptionalCheatEnum::InjectCore, OptionalCheatEnum::GetCurrentDifficulty}
	},
	{GUIElementEnum::dumpCoreGUI,
			{OptionalCheatEnum::DumpCore} 
	},
	{GUIElementEnum::dumpCoreAutonameGUI,
			{OptionalCheatEnum::DumpCore}
	},
	{GUIElementEnum::dumpCoreForcesSave,
			{OptionalCheatEnum::DumpCore, OptionalCheatEnum::ForceCoreSave}
	},
	{GUIElementEnum::speedhackGUI,
			{OptionalCheatEnum::Speedhack} 
	},
	{GUIElementEnum::invulnGUI,
			{OptionalCheatEnum::Invulnerability} 
	},
	{GUIElementEnum::aiFreezeGUI,
			{OptionalCheatEnum::AIFreeze}
	},
	{ GUIElementEnum::medusaGUI,
	{OptionalCheatEnum::Medusa}
	},
	{ GUIElementEnum::forceTeleportGUI,
			{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportSettingsSubheading,
			{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportApplyToPlayer,
			{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportCustomObject,
			{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportSettingsRadioGroup,
			{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportForward,
			{OptionalCheatEnum::ForceTeleport, OptionalCheatEnum::GetPlayerViewAngle}
	},
	{ GUIElementEnum::forceTeleportRelativeVec3,
	{OptionalCheatEnum::ForceTeleport ,OptionalCheatEnum::GetPlayerViewAngle}
	},
	{ GUIElementEnum::forceTeleportForwardIgnoreZ,
	{OptionalCheatEnum::ForceTeleport ,OptionalCheatEnum::GetPlayerViewAngle}
	},
	{ GUIElementEnum::forceTeleportManual,
	{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportAbsoluteVec3,
{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceTeleportFillWithCurrentPositionEvent,
{OptionalCheatEnum::ForceTeleport}
	},
	{ GUIElementEnum::forceLaunchGUI,
			{OptionalCheatEnum::ForceLaunch}
	},
	{ GUIElementEnum::forceLaunchSettingsSubheading,
			{OptionalCheatEnum::ForceLaunch}
	},
	{ GUIElementEnum::forceLaunchApplyToPlayer,
			{OptionalCheatEnum::ForceLaunch}
	},
		{ GUIElementEnum::forceLaunchCustomObject,
			{OptionalCheatEnum::ForceLaunch}
	},
		{ GUIElementEnum::forceLaunchSettingsRadioGroup,
			{OptionalCheatEnum::ForceLaunch}
	},
	{ GUIElementEnum::forceLaunchForward,
			{OptionalCheatEnum::ForceLaunch ,OptionalCheatEnum::GetPlayerViewAngle }
	},
		{ GUIElementEnum::forceLaunchRelativeVec3,
			{OptionalCheatEnum::ForceLaunch ,OptionalCheatEnum::GetPlayerViewAngle }
	},
		{ GUIElementEnum::forceLaunchForwardIgnoreZ,
			{OptionalCheatEnum::ForceLaunch ,OptionalCheatEnum::GetPlayerViewAngle }
	},
		{ GUIElementEnum::forceLaunchManual,
			{OptionalCheatEnum::ForceLaunch}
	},
		{ GUIElementEnum::forceLaunchAbsoluteVec3,
			{OptionalCheatEnum::ForceLaunch}
	},

#ifdef HCM_DEBUG

	{ GUIElementEnum::getObjectAddressGUI,
		{OptionalCheatEnum::GetObjectAddressCLI}
	},
	{ GUIElementEnum::consoleCommandGUI,
		{OptionalCheatEnum::ConsoleCommand}
	},

#endif

};




#define MAKE_MAPPAIR(r, element, i, game) {GUIElementEnum::element, GameState::Value::game},
#define MAKE_PAIRWISE_SET(r, d, seq) 	BOOST_PP_SEQ_FOR_EACH_I(MAKE_MAPPAIR, BOOST_PP_TUPLE_ELEM(0, seq), BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_ELEM(1, seq)))
#define MAKE_ALL_PAIRWISE(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_PAIRWISE_SET, _, seq)



const std::vector<std::pair< GUIElementEnum, GameState>> GUIRequiredServices::toplevelGUIElements =
{
	MAKE_ALL_PAIRWISE(TOPGUIELEMENTS_ANDSUPPORTEDGAMES)
};




#define MAKE_GAMESTATE(s, data, game) GameState::Value::game
#define MAKE_MAPSET(element, games) {GUIElementEnum::element, {BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(MAKE_GAMESTATE, _, games))} },
#define MAKE_MAPSET_SET(r, d, i, seq) 	MAKE_MAPSET(BOOST_PP_TUPLE_ELEM(0, seq), BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_ELEM(1, seq))) 
#define MAKE_ALL_MAPSET(seq) BOOST_PP_SEQ_FOR_EACH_I(MAKE_MAPSET_SET, _, seq) 

const std::map<GUIElementEnum, std::set<GameState>> GUIRequiredServices::supportedGamesPerGUIElement =
{
	MAKE_ALL_MAPSET(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)
};




std::set<std::pair<GameState, OptionalCheatEnum>> outRequiredServices;
bool initRequiredServices = false;
const std::set<std::pair<GameState, OptionalCheatEnum>>& GUIRequiredServices::getAllRequiredServices()
{
	if (initRequiredServices) return outRequiredServices;

	for (auto& [element, cheat] : requiredServicesPerGUIElement)
	{
		if (!supportedGamesPerGUIElement.contains(element)) throw HCMInitException("Somehow supportedGames was missing an element");
		for (auto& game : supportedGamesPerGUIElement.at(element))
		{
			if (!requiredServicesPerGUIElement.contains(element)) continue; // element has no required services
			auto reqServs = requiredServicesPerGUIElement.at(element);

			for (auto req : reqServs)
			{
				outRequiredServices.insert(std::make_pair( game, req ));
			}
		}
	}


	initRequiredServices = true;
	return outRequiredServices;

	//if (initRequiredServices) return outRequiredServices;

	//for (auto& [element, game] : toplevelGUIElements)
	//{
	//		if (!requiredServicesPerGUIElement.contains(element)) continue; // element has no required services

	//		auto reqServs = requiredServicesPerGUIElement.at(element);

	//		for (auto req : reqServs)
	//		{
	//			outRequiredServices.push_back({ game, req });
	//		}
	//}
	//initRequiredServices = true;
	//return outRequiredServices;
};