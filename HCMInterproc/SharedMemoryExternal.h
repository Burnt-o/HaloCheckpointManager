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

	shm_string* selectedH1FolderName;
	shm_string* selectedH1FolderPath;

	shm_string* selectedH2FolderName;
	shm_string* selectedH2FolderPath;

	shm_string* selectedH3FolderName;
	shm_string* selectedH3FolderPath;

	shm_string* selectedODFolderName;
	shm_string* selectedODFolderPath;

	shm_string* selectedHRFolderName;
	shm_string* selectedHRFolderPath;

	shm_string* selectedH4FolderName;
	shm_string* selectedH4FolderPath;

	bool* injectCommandQueued;


	SharedMemoryExternal(bool CPnullData, 
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		const char* SFnameH1, const char* SFpathH1,
		const char* SFnameH2, const char* SFpathH2,
		const char* SFnameH3, const char* SFpathH3,
		const char* SFnameOD, const char* SFpathOD,
		const char* SFnameHR, const char* SFpathHR,
		const char* SFnameH4, const char* SFpathH4);


};

extern std::unique_ptr<SharedMemoryExternal> g_SharedMemoryExternal; // starts uninit, created by exportedFunction initSharedMemory