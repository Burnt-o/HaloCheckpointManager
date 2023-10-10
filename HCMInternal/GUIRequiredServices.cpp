#include "pch.h"
#include "GUIRequiredServices.h"
#include <boost\preprocessor.hpp>

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
		{GUIElementEnum::aiFreezeGUI,
			{OptionalCheatEnum::AIFreeze}
	},

};




#define MAKE_MAP_PAIR(r, element, i, game) {GUIElementEnum::element, GameState::Value::game},
#define MAKE_PAIRWISE_SET(r, d, seq) 	BOOST_PP_SEQ_FOR_EACH_I(MAKE_MAP_PAIR, BOOST_PP_SEQ_HEAD(seq), BOOST_PP_SEQ_TAIL(seq))
#define MAKE_ALL_PAIRWISE(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_PAIRWISE_SET, _, seq)


//https://valelab4.ucsf.edu/svn/3rdpartypublic/boost/libs/preprocessor/doc/ref/seq_head.html
const std::vector<std::pair< GUIElementEnum, GameState>> GUIRequiredServices::toplevelGUIElements =
{
	MAKE_ALL_PAIRWISE(TOP_GUI_ELEMENTS)
};




std::vector<std::pair<GameState, OptionalCheatEnum>> outRequiredServices;
bool initRequiredServices = false;
const std::vector<std::pair<GameState, OptionalCheatEnum>>& GUIRequiredServices::getAllRequiredServices()
{
	if (initRequiredServices) return outRequiredServices;

	for (auto& [element, game] : toplevelGUIElements)
	{
			if (!requiredServicesPerGUIElement.contains(element)) continue; // element has no required services

			auto reqServs = requiredServicesPerGUIElement.at(element);

			for (auto req : reqServs)
			{
				outRequiredServices.push_back({ game, req });
			}
	}
	initRequiredServices = true;
	return outRequiredServices;
};