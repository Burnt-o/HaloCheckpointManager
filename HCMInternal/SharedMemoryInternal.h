#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/string.hpp>
#include "ExternalInfo.h"
#include "ISharedMemory.h"
namespace bip = boost::interprocess;

template <typename T>
using Alloc = bip::allocator<T, bip::managed_shared_memory::segment_manager>;
using shm_string = bip::basic_string<char, std::char_traits<char>, Alloc<char>>;



class SharedMemoryInternal : public ISharedMemory
{
private:
	bip::managed_shared_memory segment;
public:
	SharedMemoryInternal()
	{
		try
		{
			segment = bip::managed_shared_memory(bip::open_only, "hcm_shm");
			auto* pdirPath = segment.find<shm_string>("HCMdirPath").first;
			if (!pdirPath) throw HCMInitException("Could not access HCMDirPath");

			HCMDirPath = *pdirPath;
		}
		catch (bip::interprocess_exception ex)
		{
			throw HCMInitException(std::format("boost::interprocess::interprocess_exception: {}", ex.what())
		}

	}

	virtual SelectedCheckpointData getInjectInfo() override;
	virtual SelectedFolderData getDumpInfo(GameState game) override;
	virtual bool getAndClearInjectQueue() override;
	std::string HCMDirPath;
};

