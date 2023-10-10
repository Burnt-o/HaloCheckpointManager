#pragma once
#include "IGUIRequiredServices.h"

#define ALL_SUPPORTED_GAMES (Halo1)(Halo2)(Halo3)(Halo3ODST)(HaloReach)(Halo4)

#define TOP_GUI_ELEMENTS \
((forceCheckpointGUI)ALL_SUPPORTED_GAMES)\
((forceRevertGUI)ALL_SUPPORTED_GAMES)\
((forceDoubleRevertGUI)(Halo2)(Halo3)(Halo3ODST)(HaloReach)(Halo4))\
((forceCoreSaveGUI)(Halo1))\
((forceCoreLoadGUI)(Halo1))\
((injectCheckpointGUI)ALL_SUPPORTED_GAMES)\
((dumpCheckpointGUI)ALL_SUPPORTED_GAMES)\
((injectCoreGUI)(Halo1))\
((dumpCoreGUI)(Halo1))\
((speedhackGUI)ALL_SUPPORTED_GAMES)\
((invulnGUI)ALL_SUPPORTED_GAMES)\
((aiFreezeGUI)ALL_SUPPORTED_GAMES)

class GUIRequiredServices : public IGUIRequiredServices {
private:
	static const std::vector<std::pair< GUIElementEnum, GameState>> toplevelGUIElements;
	static const std::map <GUIElementEnum, std::vector<OptionalCheatEnum>> requiredServicesPerGUIElement;

public:
	virtual const std::vector<std::pair< GUIElementEnum, GameState>>& getToplevelGUIElements() override
	{
		return toplevelGUIElements;
	}
	virtual const std::map < GUIElementEnum, std::vector<OptionalCheatEnum>>& getRequiredServicesPerGUIElement() override
	{
		return requiredServicesPerGUIElement;
	}

	virtual const std::vector<std::pair<GameState, OptionalCheatEnum>>& getAllRequiredServices() override;

	GUIRequiredServices() { PLOG_DEBUG << "GUIRequiredServices con"; }
	~GUIRequiredServices() { PLOG_DEBUG << "~GUIRequiredServices"; }
};