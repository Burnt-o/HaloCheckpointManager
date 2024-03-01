#pragma once
#include "ExternalInfo.h"
#include "GameState.h"

enum HCMInternalStatus
{
	Initialising = 0,
	AllGood = 1,
	Error = 2,
};

class ISharedMemory
{
public:
	virtual SelectedCheckpointData getInjectInfo() = 0;
	virtual SelectedFolderData getDumpInfo(GameState game) = 0;
	virtual bool getAndClearInjectQueue() = 0;


	virtual void setStatusFlag(HCMInternalStatus in) noexcept = 0;

	std::string HCMDirPath;

	~ISharedMemory() = default;
};
