#pragma once
#include "IGUIRequiredServices.h"
class GUIRequiredServices : public IGUIRequiredServices {
private:
	static const std::map< GUIElementEnum, std::pair<std::vector<GameState>, bool>> GUIElementGamesAndTopness;
	static const std::map <GUIElementEnum, std::vector<OptionalCheatEnum>> requiredServicesPerGUIElement;

public:
	virtual const std::map< GUIElementEnum, std::pair<std::vector<GameState>, bool>>& getGUIElementGamesAndTopness() override
	{
		return GUIElementGamesAndTopness;
	}
	virtual const std::map < GUIElementEnum, std::vector<OptionalCheatEnum>>& getRequiredServicesPerGUIElement() override
	{
		return requiredServicesPerGUIElement;
	}

	virtual const std::vector<std::pair<GameState, OptionalCheatEnum>>& getAllRequiredServices() override;


};