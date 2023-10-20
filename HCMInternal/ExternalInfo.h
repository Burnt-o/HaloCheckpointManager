#pragma once
#include "rpc\msgpack.hpp"


// These two structs are used by the inject/dump checkpoint/core implementation 
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
	std::string selectedFolderName = "";
	std::string selectedFolderPath = "";
};


// Similiar to above but can be sent over rpc from HCMInterproc
struct SelectedCheckpointDataExternal // what checkpoint to inject from HCMExternal and info about it
{
	bool selectedCheckpointNull = true;
	int selectedCheckpointGame = 0;
	char selectedCheckpointName[260]; 
	char selectedCheckpointFilePath[260]; 
	char selectedCheckpointLevelCode[260];
	char selectedCheckpointGameVersion[260];
	int selectedCheckpointDifficulty = 0;
	MSGPACK_DEFINE_ARRAY(selectedCheckpointNull, selectedCheckpointGame, selectedCheckpointName, selectedCheckpointFilePath, selectedCheckpointLevelCode, selectedCheckpointGameVersion, selectedCheckpointDifficulty);
	
	operator SelectedCheckpointData () {
		return SelectedCheckpointData{ selectedCheckpointNull, selectedCheckpointGame, selectedCheckpointName, selectedCheckpointFilePath, selectedCheckpointLevelCode, selectedCheckpointGameVersion, selectedCheckpointDifficulty };
		}
	
};


struct SelectedFolderDataExternal // what saveFolder to dump to
{
	char selectedFolderName[260];
	char selectedFolderPath[260];
	MSGPACK_DEFINE_ARRAY(selectedFolderName, selectedFolderPath);

	operator SelectedFolderData () {
		return SelectedFolderData{ selectedFolderName, selectedFolderPath };
	}
};
