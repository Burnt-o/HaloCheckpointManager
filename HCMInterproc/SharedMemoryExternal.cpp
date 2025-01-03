#include "pch.h"
#include "SharedMemoryExternal.h"




std::string getOwnProcessDirectory()
{
	CHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(std::string(buffer).substr(0, pos) + "\\");
}


SharedMemoryExternal::SharedMemoryExternal(bool CPnullData,
	int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
	int SFgame, const char* SFname, const char* SFpath)
{
	try
	{
		// remove shared memory if it already exists
		bip::shared_memory_object::remove("hcm_shm");

		// allow process (hcminternal via mcc) without privledges to access sharedMem even if HCMExternal is privledged
		bip::permissions  unrestricted_permissions;
		unrestricted_permissions.set_unrestricted();

		// create segment with unrestricted permissions
		segment = bip::managed_shared_memory(bip::create_only, "hcm_shm", 65536, 0, unrestricted_permissions);

		auto* segmentManager = segment.get_segment_manager();
		shm_string::allocator_type sa(segmentManager);

		auto HCMdirPath = segment.construct<shm_string>("HCMdirPath")(sa);
		HCMdirPath->assign(getOwnProcessDirectory().c_str());

		injectCommandQueued = segment.construct<bool>("injectCommandQueued")(false);
		HCMInternalStatusFlag = segment.construct<int>("HCMInternalStatusFlag")((int)HCMInternalStatus::Initialising);

		// selectedCheckpointInfo
		selectedCheckpointNull = segment.construct<bool>("selectedCheckpointNull")(CPnullData);
		selectedCheckpointGame = segment.construct<int>("selectedCheckpointGame")(CPgame);
		selectedCheckpointName = segment.construct<shm_string>("selectedCheckpointName")(sa);
		selectedCheckpointName->assign(CPname);
		selectedCheckpointFilePath = segment.construct<shm_string>("selectedCheckpointFilePath")(sa);
		selectedCheckpointFilePath->assign(CPpath);
		selectedCheckpointLevelCode = segment.construct<shm_string>("selectedCheckpointLevelCode")(sa);
		selectedCheckpointLevelCode->assign(CPlevelcode);
		selectedCheckpointGameVersion = segment.construct<shm_string>("selectedCheckpointGameVersion")(sa);
		selectedCheckpointGameVersion->assign(CPgameVersion);
		selectedCheckpointDifficulty = segment.construct<int>("selectedCheckpointDifficulty")(CPdifficulty);

		PLOG_DEBUG << "SharedMemoryExternal::SharedMemoryExternal: SFname: " << SFname;

		// selected folder info
		selectedFolderGame = segment.construct<int>("selectedFolderGame")(SFgame);
		selectedFolderName = segment.construct<shm_string>("selectedFolderName")(sa);
		selectedFolderName->assign(SFname);
		selectedFolderPath = segment.construct<shm_string>("selectedFolderPath")(sa);
		selectedFolderPath->assign(SFpath);

	}
	catch (bip::interprocess_exception ex)
	{
		std::string errorMessage = std::format("HCM Interproc failed to create shared memory, error:\n{}", ex.what());
		PLOG_FATAL << errorMessage;
		int msgboxID = MessageBoxA(
			NULL,
			errorMessage.c_str(),
			"Halo checkpoint manager error",
			MB_OK
		);
		
	}

}


std::unique_ptr<SharedMemoryExternal> g_SharedMemoryExternal;