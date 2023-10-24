#pragma once
#include "ISharedMemory.h"
class MockSharedMemory : public ISharedMemory
{
public:
	MockSharedMemory(std::string dirPath) : HCMDirPath(dirPath) {}

	virtual SelectedCheckpointData getInjectInfo() override
	{
		return SelectedCheckpointData{}; // null data
	}
	virtual SelectedFolderData getDumpInfo(GameState game) override
	{
		return SelectedFolderData{}; // null data
	}

	std::string HCMDirPath;
};