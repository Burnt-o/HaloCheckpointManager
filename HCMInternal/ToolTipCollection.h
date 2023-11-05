#pragma once
#include "pch.h"
#include "GameState.h"

class ToolTipCollection
{
private:
	std::map<GameState, std::string> toolTipStrings
	{
		{GameState::Value::Halo1,""},
		{GameState::Value::Halo2,""},
		{GameState::Value::Halo2MP,""},
		{GameState::Value::Halo3,""},
		{GameState::Value::Halo4,""},
		{GameState::Value::Halo3ODST,""},
		{GameState::Value::HaloReach,""},
		{GameState::Value::NoGame,""},
	};

public:
	ToolTipCollection(std::string sharedTooltip)
	{
		// all games have the same tool tip
		for (auto game : AllGameStateValues)
		{
			toolTipStrings.at(game) = sharedTooltip;
		}
	}

	ToolTipCollection(std::map<GameState::Value, std::string> toolTipCollection)
	{
		// deep copy
		for (auto [game, string] : toolTipCollection)
		{
			toolTipStrings.at(game) = string;
		}
	}

	std::string_view getToolTip(GameState game) 
	{ 
		if (toolTipStrings.contains(game))
			return toolTipStrings.at(game);
	}
};