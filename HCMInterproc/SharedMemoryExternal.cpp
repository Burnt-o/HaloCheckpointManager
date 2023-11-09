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
	const char* SFnameH1, const char* SFpathH1,
	const char* SFnameH2, const char* SFpathH2,
	const char* SFnameH3, const char* SFpathH3,
	const char* SFnameOD, const char* SFpathOD,
	const char* SFnameHR, const char* SFpathHR,
	const char* SFnameH4, const char* SFpathH4)
{
	// TODO:: catch exceptions
	bip::shared_memory_object::remove("hcm_shm");
	segment = bip::managed_shared_memory(bip::create_only, "hcm_shm", 65536);

	auto* segmentManager = segment.get_segment_manager();
	shm_string::allocator_type sa(segmentManager);

	auto HCMdirPath = segment.construct<shm_string>("HCMdirPath")(sa);
	HCMdirPath->assign(getOwnProcessDirectory().c_str());

	injectCommandQueued = segment.construct<bool>("injectCommandQueued")(false);

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

	PLOG_DEBUG << "SharedMemoryExternal::SharedMemoryExternal: SFnameH1: " << SFnameH1;

	// selected folder info
	selectedH1FolderName = segment.construct<shm_string>("selectedH1FolderName")(sa);
	selectedH1FolderName->assign(SFnameH1);
	selectedH1FolderPath = segment.construct<shm_string>("selectedH1FolderPath")(sa);
	selectedH1FolderPath->assign(SFpathH1);

	selectedH2FolderName = segment.construct<shm_string>("selectedH2FolderName")(sa);
	selectedH2FolderName->assign(SFnameH2);
	selectedH2FolderPath = segment.construct<shm_string>("selectedH2FolderPath")(sa);
	selectedH2FolderPath->assign(SFpathH2);

	selectedH3FolderName = segment.construct<shm_string>("selectedH3FolderName")(sa);
	selectedH3FolderName->assign(SFnameH3);
	selectedH3FolderPath = segment.construct<shm_string>("selectedH3FolderPath")(sa);
	selectedH3FolderPath->assign(SFpathH3);

	selectedODFolderName = segment.construct<shm_string>("selectedODFolderName")(sa);
	selectedODFolderName->assign(SFnameOD);
	selectedODFolderPath = segment.construct<shm_string>("selectedODFolderPath")(sa);
	selectedODFolderPath->assign(SFpathOD);

	selectedHRFolderName = segment.construct<shm_string>("selectedHRFolderName")(sa);
	selectedHRFolderName->assign(SFnameHR);
	selectedHRFolderPath = segment.construct<shm_string>("selectedHRFolderPath")(sa);
	selectedHRFolderPath->assign(SFpathHR);

	selectedH4FolderName = segment.construct<shm_string>("selectedH4FolderName")(sa);
	selectedH4FolderName->assign(SFnameH4);
	selectedH4FolderPath = segment.construct<shm_string>("selectedH4FolderPath")(sa);
	selectedH4FolderPath->assign(SFpathH4);
	}

std::unique_ptr<SharedMemoryExternal> g_SharedMemoryExternal;