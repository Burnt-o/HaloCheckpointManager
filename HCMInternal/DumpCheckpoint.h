#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MultilevelPointer.h"
#include "InjectRequirements.h"
#include "PointerManager.h"
#include "ISharedMemory.h"
#include "IMCCStateHook.h"
#include "GetMCCVersion.h"
#include "IMessagesGUI.h"
#include "PointerManager.h"
#include "MultilevelPointer.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "IModalDialogRenderer.h"


class DumpCheckpoint : public IOptionalCheat {
private:
	// which game is this implementation for
	GameState mImplGame; 

	// event callbacks
	ScopedCallback<ActionEvent> mDumpCheckpointEventCallback;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<IGetMCCVersion> getMCCVerWeak;
	std::weak_ptr<ISharedMemory> sharedMemWeak;
	std::weak_ptr<IModalDialogRenderer> modalDialogsWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;


	// data
	std::shared_ptr<InjectRequirements> mInjectRequirements;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation1;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation2;
	std::shared_ptr<MultilevelPointer> mDoubleRevertFlag;
	std::shared_ptr<int64_t> mCheckpointLength;



	// primary event callback
	void onDump() {


		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);
			lockOrThrow(getMCCVerWeak, getMCCVer);
			lockOrThrow(sharedMemWeak, sharedMem);
			lockOrThrow(modalDialogsWeak, modalDialogs);
			lockOrThrow(settingsWeak, settings);


			if (!mccStateHook->isGameCurrentlyPlaying(mImplGame)) return;
			PLOG_DEBUG << "onDump called " << mImplGame.toString();


			// generate a default checkpoint name
			SYSTEMTIME t;
			GetSystemTime(&t);
			std::string checkpointName = std::format(
				"Checkpoint_{:04}{:02}{:02}_{:02}{:02}{:02}",
				t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

			// ask the user what they want to call it, if they want that. Otherwise we use the default checkpoint name
			if (settings->autonameCheckpoints->GetValue() == false)
			{
				PLOG_DEBUG << "calling blocking func showSaveDumpNameDialog";
				auto modalReturn = modalDialogs->showSaveDumpNameDialog("Name dumped checkpoint", checkpointName); // this is a blocking call
				PLOG_DEBUG << "showSaveDumpNameDialog returned! ";

				if (!std::get<bool>(modalReturn)) { PLOG_DEBUG << "User cancelled dump"; return; } // user cancelled dump
				checkpointName = std::get<std::string>(modalReturn); // get the name the user set
			}

			// Automatically force checkpoint beforehand if user wants that
			if (settings->dumpCheckpointForcesSave->GetValue())
			{
				settings->forceCheckpointEvent->operator()();
				Sleep(10);
			}


			int64_t checkpointLength = *mCheckpointLength.get();
			auto currentSaveFolder = sharedMem->getDumpInfo(mImplGame);
			PLOG_DEBUG << "Attempting checkpoint dump for game: " << mImplGame.toString();;

			auto dumpPath = currentSaveFolder.selectedFolderPath + "\\" + checkpointName + ".bin";
			PLOG_DEBUG << "dump path: " << dumpPath;

			// get pointer to checkpoint in memory
			uintptr_t checkpointLoc = 0;
			if (mInjectRequirements->singleCheckpoint)
			{
				mCheckpointLocation1->resolve(&checkpointLoc);
				PLOG_DEBUG << "using checkpoint location @0x" << std::hex << (uint64_t)checkpointLoc;
			}
			else
			{
				bool firstCheckpoint;
				mDoubleRevertFlag->readData(&firstCheckpoint);
				firstCheckpoint ? mCheckpointLocation1->resolve(&checkpointLoc) : mCheckpointLocation2->resolve(&checkpointLoc);
				PLOG_DEBUG << "using checkpoint location " << (firstCheckpoint ? "A" : "B") << " @0x" << std::hex << (uint64_t)checkpointLoc;
			}

			// check that the pointer is good
			if (IsBadReadPtr((void*)checkpointLoc, checkpointLength)) throw HCMRuntimeException(std::format("Bad dump read at 0x{:X}, length 0x{:X}", checkpointLoc, checkpointLength));

			// store checkpoint data in a vector buffer
			std::vector<byte> checkpointData;
			checkpointData.resize(checkpointLength);
			auto err = memcpy_s(checkpointData.data(), checkpointLength, (void*)checkpointLoc, checkpointLength);
			if (err) throw HCMRuntimeException(std::format("error storing checkpointData from memory! code: {}", err));

			// add version information to last 10 bytes of file
			std::string versionString = getMCCVer->getMCCVersionAsString().data();
			if (versionString.size() != 10) throw HCMRuntimeException(std::format("Version string was the wrong size somehow?! Expected: 10, Actual: {}", versionString.size()));
			std::copy(std::begin(versionString), std::end(versionString), std::end(checkpointData) - 10);


			// setup file stream and write
			std::ofstream dumpFile(str_to_wstr(dumpPath), std::ios::binary); // convert to wstr to handle unicode characters like 'á' etc
			dumpFile.exceptions(std::ofstream::badbit); // we want exceptions

			if (!dumpFile) throw HCMRuntimeException(std::format("Could not create file at location: {},\nare there naughty characters in your checkpoint name?", dumpPath));

			try
			{
				dumpFile.write((char*)checkpointData.data(), checkpointLength);
			}
			catch (std::ofstream::failure& e)
			{
				throw HCMRuntimeException(std::format("Failed to write checkpoint file to location {}: error: {}", dumpPath, e.what()));
			}

			messagesGUI->addMessage(std::format("Dumped checkpoint: {}.bin to {}", checkpointName, currentSaveFolder.selectedFolderName));

			PLOG_INFO << std::format("successfully dumped checkpoint from 0x{:X} to path: {}", (uint64_t)checkpointLoc, dumpPath);

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}


public:
	DumpCheckpoint(GameState game, IDIContainer& dicon) 
		: mImplGame(game),
		mDumpCheckpointEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->dumpCheckpointEvent, [this]() { onDump(); }),
		getMCCVerWeak(dicon.Resolve<IGetMCCVersion>()), 
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		sharedMemWeak(dicon.Resolve<ISharedMemory>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		modalDialogsWeak(dicon.Resolve<IModalDialogRenderer>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		mInjectRequirements = ptr->getData<std::shared_ptr<InjectRequirements>>("injectRequirements", game);
		mCheckpointLength = ptr->getData< std::shared_ptr<int64_t>>("checkpointLength", game);
		mCheckpointLocation1 = ptr->getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation1", game);

		if (!mInjectRequirements->singleCheckpoint)
		{
			mCheckpointLocation2 = ptr->getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation2", game);
			mDoubleRevertFlag = ptr->getData< std::shared_ptr<MultilevelPointer>>("doubleRevertFlag", game);
		}


	}

	~DumpCheckpoint()
	{
		PLOG_VERBOSE << "~" << getName();
	}


	std::string_view getName() override { return nameof(DumpCheckpoint); }
};


