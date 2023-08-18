#pragma once
// Copy of MCC's gameindicator enum, with addition of "Menu"
enum class GameState
{
	Menu = -1,
	Halo1 = 0,
	Halo2 = 1,
	Halo3 = 2,
	Halo4 = 3,
	Halo2MP = 4,
	Halo3ODST = 5,
	HaloReach = 6,

};



extern std::map<std::string, GameState> StringToGameState;
extern std::map<GameState, std::string> GameStateToString;

extern std::vector<GameState> AllSupportedGames;


struct SelectedCheckpointData {
	bool selectedCheckpointNull = true;
	int selectedCheckpointGame = 0;
	std::string selectedCheckpointName = "";
	std::string selectedCheckpointFilePath = "";
	std::string selectedCheckpointLevelCode = "";
	std::string selectedCheckpointGameVersion = "";
	int selectedCheckpointDifficulty = 0;

};

struct SelectedFolderData {
	bool selectedFolderNull = true;
	int selectedFolderGame = 0;
	std::string selectedFolderName = "";
	std::string selectedFolderPath = "";
};

