#pragma once
//#include "SharedMemoryExternal.h"
#include "RPCServerExternal.h"


extern "C"
{
	__declspec(dllexport) extern void updateSelectedCheckpoint(bool nullData, int game, const char* name, const char* path, const char* levelcode, const char* gameVersion, int difficulty)
	{
		RPCServerExternal::cII.selectedCheckpointNull = nullData;
		if (nullData) return;
		RPCServerExternal::cII.selectedCheckpointGame = game;
		strcpy_s(RPCServerExternal::cII.selectedCheckpointName, name);
		strcpy_s(RPCServerExternal::cII.selectedCheckpointFilePath, path);
		strcpy_s(RPCServerExternal::cII.selectedCheckpointLevelCode, levelcode);
		strcpy_s(RPCServerExternal::cII.selectedCheckpointGameVersion, gameVersion);
		RPCServerExternal::cII.selectedCheckpointDifficulty = difficulty;
		//SharedMemoryExternal::updateSelectedCheckpoint(nullData, game, name, path, levelcode, gameVersion, difficulty);
	}
	__declspec(dllexport) extern void updateSelectedFolder(bool nullData, int game, const char* name, const char* path)
	{
		RPCServerExternal::cDI.selectedFolderNull = nullData;
		if (nullData) return;
		RPCServerExternal::cDI.selectedFolderGame = game;
		strcpy_s(RPCServerExternal::cDI.selectedFolderName, name);
		strcpy_s(RPCServerExternal::cDI.selectedFolderPath, path);
		//SharedMemoryExternal::updateSelectedFolderData(nullData, game, name, path);
	}


}