#pragma once
//#include "SharedMemoryExternal.h"
#include "SharedMemoryExternal.h"

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
		const char* SFnameH1, const char* SFpathH1,
		const char* SFnameH2, const char* SFpathH2,
		const char* SFnameH3, const char* SFpathH3,
		const char* SFnameOD, const char* SFpathOD,
		const char* SFnameHR, const char* SFpathHR,
		const char* SFnameH4, const char* SFpathH4)
	{
		if (!g_SharedMemoryExternal.get())
		{
			PLOG_ERROR << "g_SharedMemoryExternal not initialised!";
			return;
		}

		PLOG_VERBOSE << "assigning selectedH2FolderPath as " << SFpathH2;

		g_SharedMemoryExternal->selectedH1FolderName->assign(SFnameH1);
		g_SharedMemoryExternal->selectedH1FolderPath->assign(SFpathH1);

		g_SharedMemoryExternal->selectedH2FolderName->assign(SFnameH2);
		g_SharedMemoryExternal->selectedH2FolderPath->assign(SFpathH2);

		g_SharedMemoryExternal->selectedH3FolderName->assign(SFnameH3);
		g_SharedMemoryExternal->selectedH3FolderPath->assign(SFpathH3);

		g_SharedMemoryExternal->selectedODFolderName->assign(SFnameOD);
		g_SharedMemoryExternal->selectedODFolderPath->assign(SFpathOD);

		g_SharedMemoryExternal->selectedHRFolderName->assign(SFnameHR);
		g_SharedMemoryExternal->selectedHRFolderPath->assign(SFpathHR);

		g_SharedMemoryExternal->selectedH4FolderName->assign(SFnameH4);
		g_SharedMemoryExternal->selectedH4FolderPath->assign(SFpathH4);
	}

	__declspec(dllexport) extern uint16_t initSharedMemory(
		bool CPnullData,
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		const char* SFnameH1, const char* SFpathH1,
		const char* SFnameH2, const char* SFpathH2,
		const char* SFnameH3, const char* SFpathH3,
		const char* SFnameOD, const char* SFpathOD,
		const char* SFnameHR, const char* SFpathHR,
		const char* SFnameH4, const char* SFpathH4
	)
	{
		PLOG_INFO << "Attempting to init shared memory";
		try
		{
			g_SharedMemoryExternal = std::make_unique<SharedMemoryExternal>(CPnullData, 
				CPgame, CPname, CPpath, CPlevelcode, CPgameVersion, CPdifficulty,
				SFnameH1, SFpathH1,
				SFnameH2, SFpathH2,
				SFnameH3, SFpathH3,
				SFnameOD, SFpathOD,
				SFnameHR, SFpathHR,
				SFnameH4, SFpathH4);
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