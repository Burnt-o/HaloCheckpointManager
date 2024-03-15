#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ISharedMemory.h"
#include "IMCCStateHook.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include "PointerDataStore.h"
#include "SettingsStateAndEvents.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "ModalDialogRenderer.h"

class DumpCore : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mDumpCoreEventCallback;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<IGetMCCVersion> getMCCVerWeak;
	std::weak_ptr<ISharedMemory> sharedMemWeak;
	std::weak_ptr<ModalDialogRenderer> modalDialogsWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;

	// primary event callback
	void onDump()
	{

		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);
			lockOrThrow(getMCCVerWeak, getMCCVer);
			lockOrThrow(sharedMemWeak, sharedMem);
			lockOrThrow(modalDialogsWeak, modalDialogs);
			lockOrThrow(settingsWeak, settings);

			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
			PLOG_DEBUG << "onDump called " << mGame.toString();




			// generate a default checkpoint name
			SYSTEMTIME t;
			GetSystemTime(&t);
			std::string coreSaveName = std::format(
				"Checkpoint_{:04}{:02}{:02}_{:02}{:02}{:02}",
				t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

			// ask the user what they want to call it, if they want that
			if (settings->autonameCoresaves->GetValue() == false)
			{
				PLOG_DEBUG << "calling blocking func showSaveDumpNameDialog";
				auto modalReturn = modalDialogs->showReturningDialog((ModalDialogFactory::makeCheckpointDumpNameDialog("Name dumped core save", coreSaveName))); // this is a blocking call
				PLOG_DEBUG << "showSaveDumpNameDialog returned! ";

				if (!std::get<bool>(modalReturn)) { PLOG_DEBUG << "User cancelled dump"; return; } // user cancelled dump
				coreSaveName = std::get<std::string>(modalReturn); // get the name the user set
			}

			// Automatically force coresave beforehand if user wants that
			if (settings->dumpCoreForcesSave->GetValue())
			{
				settings->forceCoreSaveEvent->operator()();
				Sleep(10);
			}
			

			constexpr int minimumFileLength = 10000;
			auto currentSaveFolder = sharedMem->getDumpInfo(mGame);
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
			std::string versionString = getMCCVer->getMCCVersionAsString().data();
			if (versionString.size() != 10) throw HCMRuntimeException(std::format("Version string was the wrong size somehow?! Expected: 10, Actual: {}", versionString.size()));
			std::copy(std::begin(versionString), std::end(versionString), std::end(checkpointData) - 10);

			// setup file stream and write
			std::ofstream dumpFile(str_to_wstr(dumpPath), std::ios::binary); // convert to wstr to handle unicode characters like 'á' etc
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
		mDumpCoreEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->dumpCoreEvent, [this]() { onDump(); }),
		getMCCVerWeak(dicon.Resolve<IGetMCCVersion>()), 
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		sharedMemWeak(dicon.Resolve<ISharedMemory>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		modalDialogsWeak(dicon.Resolve<ModalDialogRenderer>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>())
	{
	}

	~DumpCore()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(DumpCore); }
};

