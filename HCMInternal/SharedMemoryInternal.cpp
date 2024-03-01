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
	std::string folderAccessName;
	std::string folderAccessPath;

	switch (game)
	{
	case GameState::Value::Halo1:
		folderAccessName = "selectedH1FolderName";
		folderAccessPath = "selectedH1FolderPath";
		break;

	case GameState::Value::Halo2:
		folderAccessName = "selectedH2FolderName";
		folderAccessPath = "selectedH2FolderPath";
		break;

	case GameState::Value::Halo3:
		folderAccessName = "selectedH3FolderName";
		folderAccessPath = "selectedH3FolderPath";
		break;

	case GameState::Value::Halo3ODST:
		folderAccessName = "selectedODFolderName";
		folderAccessPath = "selectedODFolderPath";
		break;

	case GameState::Value::HaloReach:
		folderAccessName = "selectedHRFolderName";
		folderAccessPath = "selectedHRFolderPath";
		break;

	case GameState::Value::Halo4:
		folderAccessName = "selectedH4FolderName";
		folderAccessPath = "selectedH4FolderPath";
		break;

	default:
		throw HCMRuntimeException(std::format("Invalid game passed to getDumpInfo: {}", game.toString()));
	}



	auto* selectedFolderName = segment.find<shm_string>(folderAccessName.c_str()).first;
	auto* selectedFolderPath = segment.find<shm_string>(folderAccessPath.c_str()).first;

	nullCheck(selectedFolderName);
	nullCheck(selectedFolderPath);

	PLOG_VERBOSE << "getDumpInfo returning for game: " << game.toString() << ", \naccessor: " << folderAccessPath << ", \nvalue: " << std::string(*selectedFolderPath);

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
