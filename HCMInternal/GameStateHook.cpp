#include "pch.h"
#include "GameStateHook.h"

GameStateHook* GameStateHook::instance = nullptr;

GameState GameStateHook::parseGameState()
{
	int gameStateAsInt = 0;
	if (!instance->gameIndicator.get()->readData(&gameStateAsInt))
	{
		throw  HCMRuntimeException ("Failed to read gameIndicator when gameState changed");
	}
	return (GameState)gameStateAsInt;
}

std::string GameStateHook::parseHaloLevel()
{
	std::string out;
	if (!instance->globalLevelName.get()->readData(&out))
	{
		throw  HCMRuntimeException("Failed to read currentHaloLevel when level changed");
	}
	return out;
}