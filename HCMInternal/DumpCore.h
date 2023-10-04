#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SharedMemoryInternal.h"
#include "MCCStateHook.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include "PointerManager.h"
#include "SettingsStateAndEvents.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "RuntimeExceptionHandler.h"

class DumpCore : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mDumpCoreEventCallback;

	// injected services
	gsl::not_null<std::shared_ptr<MCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<MessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;
	gsl::not_null<std::shared_ptr<IGetMCCVersion>> getMCCVer;
	gsl::not_null<std::shared_ptr<SharedMemoryInternal>> sharedMem;


	// primary event callback
	void onDump()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
		try
		{
			std::string coreSaveName = "somecoresavename";
			constexpr int minimumFileLength = 10000;
			auto currentSaveFolder = sharedMem->getDumpInfo();
			PLOG_DEBUG << "Attempting core dump for game: " << mGame;
			if (currentSaveFolder.selectedFolderNull) throw HCMRuntimeException("No savefolder selected, somehow?!");
			if ((GameState)currentSaveFolder.selectedFolderGame != this->mGame) throw HCMRuntimeException(std::format("Can't dump - savefolder from wrong game! Expected: {}, Actual: {}", mGame.toString(), ((GameState)currentSaveFolder.selectedFolderGame).toString()));

			// TODO: force checkpoint if setting se
			// get the folder to dump the checkpoint file to
			// ask the user what they want to call it
			// TODO (need to implement dialogbox stuff, and make sure we're on a seperate thread)

			auto dumpPath = currentSaveFolder.selectedFolderPath + "\\" + coreSaveName + ".bin";
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
			std::string versionString = getMCCVer->getMCCVersionAsString().data();
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

			messagesGUI->addMessage(std::format("Dumped core save: {}.bin to {}", coreSaveName, currentSaveFolder.selectedFolderName));

			PLOG_INFO << std::format("Successfully dumped core save from {} to {}", coreSaveInjectLocation, dumpPath);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}



public:
	DumpCore(GameState game, IDIContainer& dicon) 
		: mGame(game),
		mDumpCoreEventCallback(dicon.Resolve<SettingsStateAndEvents>()->dumpCoreEvent, [this]() { onDump(); }),
		getMCCVer(dicon.Resolve<IGetMCCVersion>()), 
		mccStateHook(dicon.Resolve<MCCStateHook>()),
		sharedMem(dicon.Resolve<SharedMemoryInternal>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUI(dicon.Resolve<MessagesGUI>())

	{
	}

	virtual std::string_view getName() override { return nameof(DumpCore); }
};

