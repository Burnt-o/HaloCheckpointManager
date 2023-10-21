#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ISharedMemory.h"
#include "IMCCStateHook.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include "PointerManager.h"
#include "SettingsStateAndEvents.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "IModalDialogRenderer.h"

class DumpCore : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mDumpCoreEventCallback;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<IGetMCCVersion> getMCCVer;
	std::weak_ptr<ISharedMemory> sharedMem;
	std::weak_ptr<IModalDialogRenderer> modalDialogs;
	std::weak_ptr<SettingsStateAndEvents> settings;

	// primary event callback
	void onDump()
	{
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame) == false) return;
		try
		{
			// Automatically force coresave beforehand if user wants that
			if (settings.lock()->dumpCoreForcesSave->GetValue())
			{
				settings.lock()->forceCoreSaveEvent->operator()();
				Sleep(10);
			}

			// generate a default checkpoint name
			SYSTEMTIME t;
			GetSystemTime(&t);
			std::string coreSaveName = std::format(
				"Checkpoint_{:04}{:02}{:02}_{:02}{:02}{:02}",
				t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

			// ask the user what they want to call it, if they want that
			if (settings.lock()->autonameCoresaves->GetValue() == false)
			{
				PLOG_DEBUG << "calling blocking func showSaveDumpNameDialog";
				auto modalReturn = modalDialogs.lock()->showSaveDumpNameDialog("Name dumped core save", coreSaveName); // this is a blocking call
				PLOG_DEBUG << "showSaveDumpNameDialog returned! ";

				if (!std::get<bool>(modalReturn)) { PLOG_DEBUG << "User cancelled dump"; return; } // user cancelled dump
				coreSaveName = std::get<std::string>(modalReturn); // get the name the user set
			}
			

			constexpr int minimumFileLength = 10000;
			auto currentSaveFolder = sharedMem.lock()->getDumpInfo(mGame);
			PLOG_DEBUG << "Attempting core dump for game: " << mGame;

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
			std::string versionString = getMCCVer.lock()->getMCCVersionAsString().data();
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

			messagesGUI.lock()->addMessage(std::format("Dumped core save: {}.bin to {}", coreSaveName, currentSaveFolder.selectedFolderName));

			PLOG_INFO << std::format("Successfully dumped core save from {} to {}", coreSaveInjectLocation, dumpPath);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions.lock()->handleMessage(ex);
		}
	}



public:
	DumpCore(GameState game, IDIContainer& dicon) 
		: mGame(game),
		mDumpCoreEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->dumpCoreEvent, [this]() { onDump(); }),
		getMCCVer(dicon.Resolve<IGetMCCVersion>()), 
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		sharedMem(dicon.Resolve<ISharedMemory>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		modalDialogs(dicon.Resolve<IModalDialogRenderer>()),
		settings(dicon.Resolve<SettingsStateAndEvents>())
	{
	}

	~DumpCore()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(DumpCore); }
};

