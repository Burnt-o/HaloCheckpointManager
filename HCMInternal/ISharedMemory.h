#pragma once
#include "ExternalInfo.h"
class ISharedMemory
{
public:
	virtual SelectedCheckpointData getInjectInfo() = 0;
	virtual SelectedFolderData getDumpInfo() = 0;

	std::string HCMDirPath;
};