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

SelectedFolderData SharedMemoryInternal::getDumpInfo()
{
	auto* selectedFolderNull = segment.find<bool>("selectedFolderNull").first;
	auto* selectedFolderGame = segment.find<int>("selectedFolderGame").first;
	auto* selectedFolderName = segment.find<shm_string>("selectedFolderName").first;
	auto* selectedFolderPath = segment.find<shm_string>("selectedFolderPath").first;


	nullCheck(selectedFolderNull);
	nullCheck(selectedFolderGame);
	nullCheck(selectedFolderName);
	nullCheck(selectedFolderPath);


	return SelectedFolderData
	{
		*selectedFolderNull,
		* selectedFolderGame,
		std::string(*selectedFolderName),
		std::string(*selectedFolderPath)
	};
}

