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

	bool* selectedFolderNull;
	int* selectedFolderGame;
	shm_string* selectedFolderName;
	shm_string* selectedFolderPath;


	SharedMemoryExternal(bool CPnullData, bool SFnullData, 
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		int SFgame, const char* SFname, const char* SFpath);


};

extern std::unique_ptr<SharedMemoryExternal> g_SharedMemoryExternal; // starts uninit, created by exportedFunction initSharedMemory