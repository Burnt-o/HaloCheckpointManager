#include "pch.h"
#include "SharedMemoryExternal.h"




std::string getOwnProcessDirectory()
{
	CHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(std::string(buffer).substr(0, pos) + "\\");
}


SharedMemoryExternal::SharedMemoryExternal(bool CPnullData, bool SFnullData,
	int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
	int SFgame, const char* SFname, const char* SFpath)
{
	// TODO:: catch exceptions
	bip::shared_memory_object::remove("hcm_shm");
	segment = bip::managed_shared_memory(bip::create_only, "hcm_shm", 65536);

	auto* segmentManager = segment.get_segment_manager();
	shm_string::allocator_type sa(segmentManager);

	auto HCMdirPath = segment.construct<shm_string>("HCMdirPath")(sa);
	HCMdirPath->assign(getOwnProcessDirectory().c_str());


	// selectedCheckpointInfo
	selectedCheckpointNull = segment.construct<bool>("selectedCheckpointNull")(CPnullData);
	selectedCheckpointGame = segment.construct<int>("selectedCheckpointGame")(CPgame);
	selectedCheckpointName = segment.construct<shm_string>("selectedCheckpointName")(sa);
	selectedCheckpointName->assign(CPname);
	selectedCheckpointFilePath = segment.construct<shm_string>("selectedCheckpointFilePath")(sa);
	selectedCheckpointFilePath->assign(CPpath);
	selectedCheckpointLevelCode = segment.construct<shm_string>("selectedCheckpointLevelCode")(sa);
	selectedCheckpointLevelCode->assign(CPpath);
	selectedCheckpointGameVersion = segment.construct<shm_string>("selectedCheckpointGameVersion")(sa);
	selectedCheckpointGameVersion->assign(CPgameVersion);
	selectedCheckpointDifficulty = segment.construct<int>("selectedCheckpointDifficulty")(CPdifficulty);



	// selected folder info
	selectedFolderNull = segment.construct<bool>("selectedFolderNull")(SFnullData);
	selectedFolderGame = segment.construct<int>("selectedFolderGame")(SFgame);
	selectedFolderName = segment.construct<shm_string>("selectedFolderName")(sa);
	selectedFolderName->assign(SFname);
	selectedFolderPath = segment.construct<shm_string>("selectedFolderPath")(sa);
	selectedFolderPath->assign(SFpath);

	}

std::unique_ptr<SharedMemoryExternal> g_SharedMemoryExternal;