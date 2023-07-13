#include "pch.h"
#include "HaloEnums.h"

std::map<std::string, GameState> StringToGameState
{
	{"Halo1", GameState::Halo1},
	{"Halo2", GameState::Halo2},
	{"Halo3", GameState::Halo3},
	{"Halo4", GameState::Halo4},
	{"Halo2MP", GameState::Halo2MP},
	{"Halo3ODST", GameState::Halo3ODST},
	{"HaloReach", GameState::HaloReach},
};

std::map<GameState, std::string> GameStateToString
{
		{GameState::Halo1, "Halo1"},
	{ GameState::Halo2, "Halo2"},
	{ GameState::Halo3, "Halo3"},
	{GameState::Halo4, "Halo4", },
	{GameState::Halo2MP, "Halo2MP", },
	{GameState::Halo3ODST, "Halo3ODST", },
	{GameState::HaloReach, "HaloReach", },
};


std::vector<GameState> AllSupportedGames
{
	GameState::Halo1,
	GameState::Halo2,
	GameState::Halo3,
	GameState::Halo4,
	GameState::Halo3ODST,
	GameState::HaloReach,
};
