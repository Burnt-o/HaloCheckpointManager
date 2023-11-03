#pragma once
#include "IGUIRequiredServices.h"

// A sequence of pairs, where the first element of a pair is the GUIElementEnum name, and the second element is a tuple of supported games for that GUIElementEnum
#define TOPGUIELEMENTS_RELEASE \
((controlHeadingGUI,(ALL_GAMES_AND_MAINMENU)))\
((saveManagementHeadingGUI,(ALL_SUPPORTED_GAMES)))\
((cheatsHeadingGUI,(ALL_SUPPORTED_GAMES)))\
((overlaysHeadingGUI,(ALL_SUPPORTED_GAMES)))\
((cameraHeadingGUI,(ALL_SUPPORTED_GAMES)))\
((theaterHeadingGUI,(Halo3,Halo3ODST,HaloReach,Halo4)))

#define TOPGUIELEMENTS_DEBUG \
((debugHeadingGUI, (ALL_SUPPORTED_GAMES)))


#ifdef HCM_DEBUG

#define TOPGUIELEMENTS_ANDSUPPORTEDGAMES BOOST_PP_CAT(TOPGUIELEMENTS_RELEASE, TOPGUIELEMENTS_DEBUG)

#else 

#define TOPGUIELEMENTS_ANDSUPPORTEDGAMES TOPGUIELEMENTS_RELEASE

#endif


class GUIRequiredServices : public IGUIRequiredServices {
private:
	static const std::vector<std::pair< GUIElementEnum, GameState>> toplevelGUIElements;
	static const std::map <GUIElementEnum, std::vector<OptionalCheatEnum>> requiredServicesPerGUIElement;
	static const std::map<GUIElementEnum, std::set<GameState>> supportedGamesPerGUIElement;

public:
	virtual const std::vector<std::pair< GUIElementEnum, GameState>>& getToplevelGUIElements() override
	{
		return toplevelGUIElements;
	}
	virtual const std::map < GUIElementEnum, std::vector<OptionalCheatEnum>>& getRequiredServicesPerGUIElement() override
	{
		return requiredServicesPerGUIElement;
	}

	virtual const std::map < GUIElementEnum, std::set<GameState>>& getSupportedGamesPerGUIElement() override
	{
		return supportedGamesPerGUIElement;
	}

	virtual const std::set<std::pair<GameState, OptionalCheatEnum>>& getAllRequiredServices() override;

	GUIRequiredServices() { PLOG_DEBUG << "GUIRequiredServices con"; }
	~GUIRequiredServices() { PLOG_DEBUG << "~GUIRequiredServices"; }
};