#pragma once
#include "OptionalCheatEnum.h"
#include "GUIElementEnum.h"
#include "GameState.h"
// provides a full list of what services (optionalCheats) are required for each GUIElement-gameState combo
class IGUIRequiredServices {
public:
	
	virtual const std::map< GUIElementEnum, std::pair<std::vector<GameState>, bool>>& getGUIElementGamesAndTopness() = 0;
	virtual const std::map<GUIElementEnum, std::vector<OptionalCheatEnum>>& getRequiredServicesPerGUIElement() = 0;
	virtual const std::vector<std::pair<GameState, OptionalCheatEnum>>& getAllRequiredServices() = 0;
};