#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/string.hpp>
#include "ExternalInfo.h"

namespace bip = boost::interprocess;

template <typename T>
using Alloc = bip::allocator<T, bip::managed_shared_memory::segment_manager>;
using shm_string = bip::basic_string<char, std::char_traits<char>, Alloc<char>>;


enum class HCMInternalStatus
{
	Initialising = 0,
	AllGood = 1,
	Error = 2,
	Shutdown = 3
};


class SharedMemoryExternal
{

private:
	bip::managed_shared_memory segment;

public:
	bool* selectedCheckpointNull;
	int* selectedCheckpointGame;
	shm_string* selectedCheckpointName;
	shm_string* selectedCheckpointFilePath;
	shm_string* selectedCheckpointLevelCode;
	shm_string* selectedCheckpointGameVersion;
	int* selectedCheckpointDifficulty;

	int* selectedFolderGame;
	shm_string* selectedFolderName;
	shm_string* selectedFolderPath;


	bool* injectCommandQueued;





	SharedMemoryExternal(bool CPnullData, 
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		int SFgame, const char* SFname, const char* SFpath);




	int* HCMInternalStatusFlag;


};


extern std::unique_ptr<SharedMemoryExternal> g_SharedMemoryExternal; // starts uninitialised, created in inititialiseInterproc

static HCMInternalStatus getHCMInternalStatusFlag()
{
	if (!g_SharedMemoryExternal.get())
	{
		PLOG_ERROR << "g_SharedMemoryExternal not initialised! t.getHCMInternalStatusFlag";
		return HCMInternalStatus::Error;
	}

	if (!g_SharedMemoryExternal->HCMInternalStatusFlag)
	{
		PLOG_ERROR << "g_SharedMemoryExternal->HCMInternalStatusFlag was null! t.getHCMInternalStatusFlag";
		return HCMInternalStatus::Error;
	}

	int readFlag = *g_SharedMemoryExternal->HCMInternalStatusFlag;

	if (!magic_enum::enum_contains<HCMInternalStatus>(readFlag))
	{
		PLOG_ERROR << "invalid HCMInternalStatus by readFlag: " << readFlag;
		return HCMInternalStatus::Error;
	}

	return (HCMInternalStatus)readFlag; 
}