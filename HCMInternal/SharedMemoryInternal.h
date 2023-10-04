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



class SharedMemoryInternal
{
private:
	bip::managed_shared_memory segment;
public:
	SharedMemoryInternal()
	{
		// TODO catch exceptions and convert to initExceptions
		segment = bip::managed_shared_memory(bip::open_only, "hcm_shm");
		auto* pdirPath = segment.find<shm_string>("HCMdirPath").first;
		if (!pdirPath) throw HCMInitException("Could not access HCMDirPath");

		HCMDirPath = *pdirPath;
	}

	SelectedCheckpointData getInjectInfo();
	SelectedFolderData getDumpInfo();

	std::string HCMDirPath;
};

