#pragma once
//#include "SharedMemoryExternal.h"
#include "SharedMemoryExternal.h"

extern "C"
{
	__declspec(dllexport) extern void updateSelectedCheckpoint(bool nullData, int game, const char* name, const char* path, const char* levelcode, const char* gameVersion, int difficulty)
	{
		if (!g_SharedMemoryExternal.get())
		{
			PLOG_ERROR << "g_SharedMemoryExternal not initialised!";
			return;
		}
		*g_SharedMemoryExternal->selectedCheckpointNull = nullData;
		*g_SharedMemoryExternal->selectedCheckpointGame = game;
		g_SharedMemoryExternal->selectedCheckpointName->assign(name);
		g_SharedMemoryExternal->selectedCheckpointFilePath->assign(path);
		g_SharedMemoryExternal->selectedCheckpointGameVersion->assign(gameVersion);
		*g_SharedMemoryExternal->selectedCheckpointDifficulty = difficulty;
	}
	__declspec(dllexport) extern void updateSelectedFolder(bool nullData, int game, const char* name, const char* path)
	{
		if (!g_SharedMemoryExternal.get())
		{
			PLOG_ERROR << "g_SharedMemoryExternal not initialised!";
			return;
		}
		*g_SharedMemoryExternal->selectedFolderNull = nullData;
		*g_SharedMemoryExternal->selectedFolderGame = game;
		g_SharedMemoryExternal->selectedFolderName->assign(name);
		g_SharedMemoryExternal->selectedFolderPath->assign(path);

	}

	__declspec(dllexport) extern uint16_t initSharedMemory(
		bool CPnullData, bool SFnullData,
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		int SFgame, const char* SFname, const char* SFpath
	)
	{
		PLOG_INFO << "Attempting to init shared memory";
		try
		{
			g_SharedMemoryExternal = std::make_unique<SharedMemoryExternal>(CPnullData, SFnullData, 
				CPgame, CPname, CPpath, CPlevelcode, CPgameVersion, CPdifficulty,
				SFgame, SFname, SFpath);
			PLOG_INFO << "Success!";
			return true;
		}
		catch (std::exception ex)
		{
			PLOG_ERROR << "Failure! " << ex.what();
			return false;
		}
	}


}