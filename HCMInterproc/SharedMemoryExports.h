#pragma once
#include "SharedMemoryExternal.h"
#include "pch.h"
extern "C"
{




	__declspec(dllexport) extern void queueInjectCommand()
	{
		if (!g_SharedMemoryExternal.get())
		{
			PLOG_ERROR << "g_SharedMemoryExternal not initialised!";
			return;
		}
		*g_SharedMemoryExternal->injectCommandQueued = true;
		PLOG_VERBOSE << "inject command queued";
	}

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
		g_SharedMemoryExternal->selectedCheckpointLevelCode->assign(levelcode);
		g_SharedMemoryExternal->selectedCheckpointGameVersion->assign(gameVersion);
		*g_SharedMemoryExternal->selectedCheckpointDifficulty = difficulty;
	}
	__declspec(dllexport) extern void updateSelectedFolder(
		int SFgame, const char* SFname, const char* SFpath
	)
	{
		if (!g_SharedMemoryExternal.get())
		{
			PLOG_ERROR << "g_SharedMemoryExternal not initialised!";
			return;
		}

		PLOG_VERBOSE << "assigning selectedFolderName as " << SFname;
		PLOG_VERBOSE << "assigning selectedFolderPath as " << SFpath;

		*g_SharedMemoryExternal->selectedFolderGame = SFgame;
		g_SharedMemoryExternal->selectedFolderName->assign(SFname);
		g_SharedMemoryExternal->selectedFolderPath->assign(SFpath);

	}




}