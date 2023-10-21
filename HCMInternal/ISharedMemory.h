#pragma once
#include "ExternalInfo.h"
#include "GameState.h"
class ISharedMemory
{
public:
	virtual SelectedCheckpointData getInjectInfo() = 0;
	virtual SelectedFolderData getDumpInfo(GameState game) = 0;

	std::string HCMDirPath;

	~ISharedMemory() = default;
};