#include "pch.h"
#include "DumpCore.h"
#include "RPCClientInternal.h"
#include "GameStateHook.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include "PointerManager.h"


void DumpCore::onDump()
{
	if (GameStateHook::getCurrentGameState() != mGame) return;
	try
	{
		constexpr int minimumFileLength = 10000;
		auto currentSaveFolder = RPCClientInternal::getDumpInfo();
		PLOG_DEBUG << "Attempting core dump for game: " << mGame;
		if (currentSaveFolder.selectedFolderNull) throw HCMRuntimeException("No savefolder selected, somehow?!");
		if ((GameState)currentSaveFolder.selectedFolderGame != this->mGame) throw HCMRuntimeException(std::format("Can't dump - savefolder from wrong game! Expected: {}, Actual: {}", mGame.toString(), ((GameState)currentSaveFolder.selectedFolderGame).toString()));

		// TODO: force checkpoint if setting se
		// get the folder to dump the checkpoint file to
		// ask the user what they want to call it
		// TODO (need to implement dialogbox stuff, and make sure we're on a seperate thread)

		auto dumpPath = currentSaveFolder.selectedFolderPath + "\\someCheckpointName.bin";
		PLOG_DEBUG << "dump path: " << dumpPath;


		// get filepath to coresave folder
		std::string coreSaveInjectLocation = std::filesystem::current_path().string();
		PLOG_VERBOSE << "MCC working directory: " << coreSaveInjectLocation;
		coreSaveInjectLocation += "\\core\\core.bin";
		PLOG_VERBOSE << "Core save inject location: " << coreSaveInjectLocation;

		if (!fileExists(coreSaveInjectLocation)) throw HCMRuntimeException(std::format("No core save existed at path: {}\nYou may need to force a core save first, or correct the core save folder path in the settings.", coreSaveInjectLocation));

		// store checkpoint data in a vector buffer
		// uses memory-mapped file + memcpy
		boost::iostreams::mapped_file_source checkpointFile(coreSaveInjectLocation);
		if (!checkpointFile.is_open()) throw HCMRuntimeException(std::format("Failed to open checkpoint file for reading"));
		if (checkpointFile.size() < minimumFileLength)
		{
			checkpointFile.close();
			throw HCMRuntimeException(std::format("Core Save file was too small! expected at least: 0x{:X}, actual: 0x{:X}", minimumFileLength, checkpointFile.size()));
		}
		int checkpointLength = checkpointFile.size();
		std::vector<byte> checkpointData;
		checkpointData.resize(checkpointLength);
		auto err = memcpy_s(checkpointData.data(), checkpointLength, checkpointFile.data(), checkpointLength);
		checkpointFile.close();
		if (err) throw HCMRuntimeException(std::format("error loading checkpointdata from file! code: {}", err));
		if (checkpointData.size() != checkpointLength) 	throw HCMRuntimeException(std::format("Checkpoint data was incorrect length! expected: 0x{:X}, actual: 0x{:X}", checkpointLength, checkpointData.size()));

		// add version information to last 10 bytes of file
		std::string versionString = PointerManager::getCurrentGameVersion();
		if (versionString.size() != 10) throw HCMRuntimeException(std::format("Version string was the wrong size somehow?! Expected: 10, Actual: {}", versionString.size()));
		std::copy(std::begin(versionString), std::end(versionString), std::end(checkpointData) - 10);

			// setup file stream and write
		std::ofstream dumpFile(dumpPath, std::ios::binary);
		dumpFile.exceptions(std::ofstream::badbit); // we want exceptions

		if (!dumpFile) throw HCMRuntimeException(std::format("Could not create file at location: {},\nare there naughty characters in your core save name?", dumpPath));

		try
		{
			dumpFile.write((char*)checkpointData.data(), checkpointLength);
		}
		catch (std::ofstream::failure& e)
		{
			throw HCMRuntimeException(std::format("Failed to write core save file to location {}: error: {}", dumpPath, e.what()));
		}

		PLOG_INFO << std::format("Successfully dumped core save from {} to {}", coreSaveInjectLocation, dumpPath);
	}
	catch (HCMRuntimeException ex)
	{
		RuntimeExceptionHandler::handleMessage(ex);
	}
}

