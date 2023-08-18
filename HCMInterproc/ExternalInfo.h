#pragma once
#include "rpc\msgpack.hpp"
struct SelectedCheckpointDataExternal // used by rpc/interproc to get info about what checkpoint to inject from HCMExternal
{
	bool selectedCheckpointNull = true;
	int selectedCheckpointGame = 0;
	char selectedCheckpointName[260];
	char selectedCheckpointFilePath[260];
	char selectedCheckpointLevelCode[260];
	char selectedCheckpointGameVersion[260];
	int selectedCheckpointDifficulty = 0;
	MSGPACK_DEFINE_ARRAY(selectedCheckpointNull, selectedCheckpointGame, selectedCheckpointName, selectedCheckpointFilePath, selectedCheckpointLevelCode, selectedCheckpointGameVersion, selectedCheckpointDifficulty);

};

struct SelectedFolderDataExternal // used by rpc/interproc to get info about what dumpfolder and game
{
	bool selectedFolderNull = true;
	int selectedFolderGame = 0;
	char selectedFolderName[260];
	char selectedFolderPath[260];
	MSGPACK_DEFINE_ARRAY(selectedFolderNull, selectedFolderGame, selectedFolderName, selectedFolderPath);
};
