#include "pch.h"
#include "SharedMemoryInternal.h"

#define nullCheck(x) 	if (!x) throw HCMRuntimeException(std::format("Could not find shm: {}", nameof(x)))

SelectedCheckpointData SharedMemoryInternal::getInjectInfo()
{
	auto* selectedCheckpointNull = segment.find<bool>("selectedCheckpointNull").first;
	auto* selectedCheckpointGame = segment.find<int>("selectedCheckpointGame").first;
	auto* selectedCheckpointName = segment.find<shm_string>("selectedCheckpointName").first;
	auto* selectedCheckpointFilePath = segment.find<shm_string>("selectedCheckpointFilePath").first;
	auto* selectedCheckpointLevelCode = segment.find<shm_string>("selectedCheckpointLevelCode").first;
	auto* selectedCheckpointGameVersion = segment.find<shm_string>("selectedCheckpointGameVersion").first;
	auto* selectedCheckpointDifficulty = segment.find<int>("selectedCheckpointDifficulty").first;

	nullCheck(selectedCheckpointNull);
	nullCheck(selectedCheckpointGame);
	nullCheck(selectedCheckpointName);
	nullCheck(selectedCheckpointFilePath);
	nullCheck(selectedCheckpointLevelCode);
	nullCheck(selectedCheckpointGameVersion);
	nullCheck(selectedCheckpointDifficulty);

	return SelectedCheckpointData
	{
		*selectedCheckpointNull,
		*selectedCheckpointGame,
		std::string(*selectedCheckpointName),
		std::string(*selectedCheckpointFilePath),
		std::string(*selectedCheckpointLevelCode),
		std::string(*selectedCheckpointGameVersion),
		*selectedCheckpointDifficulty,
	};


}

SelectedFolderData SharedMemoryInternal::getDumpInfo(GameState game)
{
	auto* selectedFolderGame = segment.find<int>("selectedFolderGame").first;
	auto* selectedFolderName = segment.find<shm_string>("selectedFolderName").first;
	auto* selectedFolderPath = segment.find<shm_string>("selectedFolderPath").first;

	nullCheck(selectedFolderGame);
	nullCheck(selectedFolderName);
	nullCheck(selectedFolderPath);

	if ((GameState)*selectedFolderGame != game)
		throw HCMRuntimeException(std::format("Cannot dump checkpoint; Wrong game tab selected in external window! HCM Tab was {} but currently running game is {}",((GameState)*selectedFolderGame).toString(), game.toString()));


	return SelectedFolderData
	{
		std::string(*selectedFolderName),
		std::string(*selectedFolderPath)
	};

}

void SharedMemoryInternal::setStatusFlag(HCMInternalStatus in) noexcept
{
	auto* HCMInternalStatusFlag = segment.find<int>("HCMInternalStatusFlag").first;

	if (!HCMInternalStatusFlag)
	{
		PLOG_ERROR << "HCMInternalStatusFlag was null";
		return;
	}

	*HCMInternalStatusFlag = (int)in;
}


bool SharedMemoryInternal::getAndClearInjectQueue()
{

	bool* injectCommandQueued = segment.find<bool>("injectCommandQueued").first;
	nullCheck(injectCommandQueued);

	if (*injectCommandQueued)
	{
		*injectCommandQueued = false;
		return true;
	}
	else
	{
		return false;
	}
}
