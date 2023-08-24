#include "pch.h"
#include "InjectCore.h"
#include "RPCClientInternal.h"
#include "GameStateHook.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include <filesystem>

void InjectCore::onInject()
{
	if (GameStateHook::getCurrentGameState() != mGame) return;
	try
	{
		constexpr int minimumFileLength = 10000;
		auto currentCheckpoint = RPCClientInternal::getInjectInfo();
		PLOG_DEBUG << "Attempting core inject for game: " << mGame.toString();
		if (currentCheckpoint.selectedCheckpointNull) throw HCMRuntimeException("Can't inject - no checkpoint selected!");
		if ((GameState)currentCheckpoint.selectedCheckpointGame != this->mGame) throw HCMRuntimeException(std::format("Can't inject - checkpoint from wrong game! Expected: {}, Actual: {}", this->mGame.toString(), ((GameState)currentCheckpoint.selectedCheckpointGame).toString()));
		// TODO: add version, difficulty, and levelcode checks (if user wants them)

		// check that the file actually exists
		if (!fileExists(currentCheckpoint.selectedCheckpointFilePath)) throw HCMRuntimeException(std::format("Core Save didn't exist at path: {}", currentCheckpoint.selectedCheckpointFilePath));;


		PLOG_DEBUG << "injectPath: " << currentCheckpoint.selectedCheckpointFilePath;

		//TODO: load correct level if level not aligned

			//TODO: safety checks if the user enables them (wrong level? wrong difficulty? wrong game?)

			// store checkpoint data in a vector buffer
			// uses memory-mapped file + memcpy
		boost::iostreams::mapped_file_source checkpointFile(currentCheckpoint.selectedCheckpointFilePath);
		if (!checkpointFile.is_open()) throw HCMRuntimeException(std::format("Failed to open checkpoint file for reading"));
		if (checkpointFile.size() < minimumFileLength)
		{
			checkpointFile.close();
			throw HCMRuntimeException(std::format("Core Save file was too small! expected at least: 0x{:X}, actual: 0x{:X}", minimumFileLength, checkpointFile.size()));
		}

		PLOG_DEBUG << "reading core save file";
		int checkpointLength = checkpointFile.size();
		std::vector<byte> checkpointData;
		checkpointData.resize(checkpointLength);
		auto err = memcpy_s(checkpointData.data(), checkpointLength, checkpointFile.data(), checkpointLength);
		checkpointFile.close();
		if (err) throw HCMRuntimeException(std::format("error loading checkpointdata from file! code: {}", err));
		if (checkpointData.size() != checkpointLength) 	throw HCMRuntimeException(std::format("Checkpoint data was incorrect length! expected: 0x{:X}, actual: 0x{:X}", checkpointLength, checkpointData.size()));

		
		PLOG_DEBUG << "zeroing last 10 bytes";
		// zero out last 10 bytes
		for (auto it = checkpointData.rbegin(); it != checkpointData.rbegin() + 10; ++it)
		{
			*it = 0;
		}


		// get filepath to coresave folder
		std::string coreSaveInjectLocation = std::filesystem::current_path().string();
		PLOG_VERBOSE << "MCC working directory: " << coreSaveInjectLocation;
		coreSaveInjectLocation += "\\core\\core.bin";
		PLOG_VERBOSE << "Core save inject location: " << coreSaveInjectLocation;

		//if (fileExists(coreSaveInjectLocation))
		//{
		//	PLOG_DEBUG << "deleting existing file";
		//	//delete it
		//	std::filesystem::remove(coreSaveInjectLocation);
		//}

		// write it
		std::ofstream coreDestFile(coreSaveInjectLocation, std::ios::binary);
		coreDestFile.exceptions(std::ofstream::badbit); // we want exceptions

		if (!coreDestFile) throw HCMRuntimeException(std::format("Could not create file at location: {}, you may need to correct the core save folder location in the settings", coreSaveInjectLocation));

		try
		{
			coreDestFile.write((char*)checkpointData.data(), checkpointLength);
			coreDestFile.close();
		}
		catch (std::ofstream::failure& e)
		{
			coreDestFile.close();
			throw HCMRuntimeException(std::format("Failed to write core save file: {}", e.what()));
		}

		PLOG_INFO << "Successfully injected coresave from " << currentCheckpoint.selectedCheckpointFilePath << " to " << coreSaveInjectLocation;
			
	}
	catch (HCMRuntimeException ex)
	{
		RuntimeExceptionHandler::handleMessage(ex);
	}
}

