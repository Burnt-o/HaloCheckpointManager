#pragma once
#include "IGUIElement.h"
// only TOPLEVEL gui elements go in here. elements nested inside another element do not.
class GUIElementStore {
private:

	// filled up by guiConstructor later
	std::map<GameState, std::vector<std::shared_ptr<IGUIElement>>> allTopLevelGUIElements
	{
	{GameState::Value::Halo1, {}},
	{GameState::Value::Halo2, {}},
	{GameState::Value::Halo3, {}},
	{GameState::Value::Halo3ODST, {}},
	{GameState::Value::HaloReach, {}},
	{GameState::Value::Halo4, {}},
	{GameState::Value::NoGame, {}},
	};

	friend class GUIElementConstructor;
	std::map<GameState, std::vector<std::shared_ptr<IGUIElement>>>& getTopLevelGUIElementsMutable() { return allTopLevelGUIElements; };
	
public:
	const std::vector<std::shared_ptr<IGUIElement>>& getTopLevelGUIElements(GameState game)
	{
		return allTopLevelGUIElements.at(game);
	}

};