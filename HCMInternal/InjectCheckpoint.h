#pragma once
#include "pch.h"
#include "GameState.h"
#include "SettingsStateAndEvents.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "IMessagesGUI.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "RuntimeExceptionHandler.h"
#include "MultilevelPointer.h"
#include "InjectRequirements.h"
#include "ISharedMemory.h"
#include "IMCCStateHook.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include "openssl\sha.h"



class InjectCheckpoint : public IOptionalCheat {
private:
	// which game is this implementation for
	GameState mImplGame; 

	// event callbacks
	ScopedCallback<ActionEvent> mInjectCheckpointEventCallback;

	// data
	std::shared_ptr<InjectRequirements> mInjectRequirements;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation1;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation2;
	std::shared_ptr<MultilevelPointer> mDoubleRevertFlag;
	std::shared_ptr<PreserveLocations> mPreserveLocations;
	std::shared_ptr<int64_t> mCheckpointLength;
	std::shared_ptr<offsetLengthPair> mSHAdata;
	std::shared_ptr<offsetLengthPair> mBSPdata;
	std::shared_ptr<MultilevelPointer> mLoadedBSP1;
	std::shared_ptr<MultilevelPointer> mLoadedBSP2;


	// injected services
	gsl::not_null<std::shared_ptr<IMCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<IMessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;
	gsl::not_null<std::shared_ptr<ISharedMemory>> sharedMem;


	void onInject() {
		if (!mccStateHook->isGameCurrentlyPlaying(mImplGame)) return;
		try
		{
			auto currentCheckpoint = sharedMem->getInjectInfo();
			PLOG_DEBUG << "Attempting checkpoint inject for game: " << mImplGame.toString();
			if (currentCheckpoint.selectedCheckpointNull) throw HCMRuntimeException("Can't inject - no checkpoint selected!");
			if ((GameState)currentCheckpoint.selectedCheckpointGame != this->mImplGame) throw HCMRuntimeException(std::format("Can't inject - checkpoint from wrong game! Expected: {}, Actual: {}", mImplGame.toString(), ((GameState)currentCheckpoint.selectedCheckpointGame).toString()));
			// TODO: add version, difficulty, and levelcode checks (if user wants them)

			PLOG_DEBUG << "injectPath: " << currentCheckpoint.selectedCheckpointFilePath;

			// get checkpoint length (useful later)
			auto checkpointLength = *mCheckpointLength.get();

			// get the path to the checkpoint file to inject


			// check that the file actually exists
			if (!fileExists(currentCheckpoint.selectedCheckpointFilePath)) throw HCMRuntimeException(std::format("Checkpoint didn't exist at path: {}", currentCheckpoint.selectedCheckpointFilePath));;

			// check that the file is the correct length
			std::ifstream fileTestLength(currentCheckpoint.selectedCheckpointFilePath, std::ios::binary);
			fileTestLength.seekg(0, std::ios::end);
			uint64_t actualLength = fileTestLength.tellg();
			fileTestLength.close();
			if (actualLength != checkpointLength) throw HCMRuntimeException(std::format("Checkpoint was the wrong length, actual: 0x{:X}, expected: 0x{:X}", actualLength, checkpointLength));

			//TODO: load correct level if level not aligned

			//TODO: safety checks if the user enables them (wrong level? wrong difficulty? wrong game?)


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
			if (IsBadWritePtr((void*)checkpointLoc, checkpointLength)) throw HCMRuntimeException(std::format("Bad inject read at 0x{:X}, length 0x{:X}", checkpointLoc, checkpointLength));


			// store preserveLocations of original checkpoint
			if (mInjectRequirements->preserveLocations)
			{
				PLOG_VERBOSE << "storing preserveLocations";
				for (auto& [offset, vec] : mPreserveLocations->locations)
				{
					auto err = memcpy_s(vec.data(), vec.size(), (void*)(checkpointLoc + offset), vec.size());
					if (err) throw HCMRuntimeException(std::format("error storing preserveLocation data! code: {}", err));
				}
			}

			// store checkpoint data in a vector buffer
			// uses memory-mapped file + memcpy
			boost::iostreams::mapped_file_source checkpointFile(currentCheckpoint.selectedCheckpointFilePath);
			if (!checkpointFile.is_open()) throw HCMRuntimeException(std::format("Failed to open checkpoint file for reading"));
			if (checkpointFile.size() != checkpointLength)
			{
				checkpointFile.close();
				throw HCMRuntimeException(std::format("Checkpoint file was incorrect length! expected: 0x{:X}, actual: 0x{:X}", checkpointLength, checkpointFile.size()));
			}
			std::vector<byte> checkpointData;
			checkpointData.resize(checkpointLength);
			auto err = memcpy_s(checkpointData.data(), checkpointLength, checkpointFile.data(), checkpointLength);
			checkpointFile.close();
			if (err) throw HCMRuntimeException(std::format("error loading checkpointdata from file! code: {}", err));
			if (checkpointData.size() != checkpointLength) 	throw HCMRuntimeException(std::format("Checkpoint data was incorrect length! expected: 0x{:X}, actual: 0x{:X}", checkpointLength, checkpointData.size()));

			// load preserveLocations
			if (mInjectRequirements->preserveLocations)
			{
				PLOG_VERBOSE << "loading preserveLocations";
				for (auto& [offset, vec] : mPreserveLocations->locations)
				{
					auto err = memcpy_s((void*)(checkpointData.data() + offset), vec.size(), vec.data(), vec.size());
					if (err) throw HCMRuntimeException(std::format("error loading preserveLocation data! code: {}", err));
				}
			}

			// zero out last 10 bytes
			for (auto it = checkpointData.rbegin(); it != checkpointData.rbegin() + 10; ++it)
			{
				*it = 0;
			}


			// calculate and set SHA
			if (mInjectRequirements->SHA)
			{
				PLOG_VERBOSE << "setting SHA data";
				// zero out SHA bytes in checkpoint Data
				for (auto it = checkpointData.begin() + mSHAdata->offset; it <= checkpointData.begin() + mSHAdata->offset + mSHAdata->length; ++it)
				{
					*it = 0;
				}
				std::vector<byte> SHAout;
				SHAout.reserve(mSHAdata->length);

				SHA1(checkpointData.data(), checkpointLength, SHAout.data());		// compute sha

				// paste it back into checkpoint data
				auto err = memcpy_s(checkpointData.data() + mSHAdata->offset, mSHAdata->length, SHAout.data(), SHAout.size());
				if (err) throw HCMRuntimeException(std::format("error loading checkpointdata from file! code: {}", err));
			}

			// inject checkpoint data into game memory
			err = memcpy_s((void*)checkpointLoc, checkpointLength, checkpointData.data(), checkpointData.size());
			if (err) throw HCMRuntimeException(std::format("error writing checkpointdata to game! code: {}", err));

			PLOG_VERBOSE << "wrote checkpoint data to location: " << std::hex << (uint64_t)checkpointLoc;

			// set BSP data
			if (mInjectRequirements->BSP)
			{
				PLOG_VERBOSE << "setting BSP data";
				std::vector<byte> currentBSPData;
				auto err = memcpy_s(currentBSPData.data(), mBSPdata->length, (void*)(checkpointLoc + mBSPdata->offset), mBSPdata->length);
				if (err) throw HCMRuntimeException(std::format("error copying bsp data! code: {}", err));
				if (mInjectRequirements->singleCheckpoint)
				{
					mLoadedBSP1->writeArrayData(currentBSPData.data(), currentBSPData.size());
				}
				else
				{
					bool firstCheckpoint;
					mDoubleRevertFlag->readData(&firstCheckpoint);
					firstCheckpoint ? mLoadedBSP1->writeArrayData(currentBSPData.data(), currentBSPData.size()) : mLoadedBSP2->writeArrayData(currentBSPData.data(), currentBSPData.size());
				}
			}


			// TODO: force revert if setting set

			messagesGUI->addMessage(std::format("Injected checkpoint {}.bin", currentCheckpoint.selectedCheckpointName));

			PLOG_INFO << "succesfully injected checkpoint from path: " << currentCheckpoint.selectedCheckpointFilePath << " to " << std::hex << (uint64_t)checkpointLoc;

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}
	public:
		InjectCheckpoint(GameState game, IDIContainer& dicon)
			: mImplGame(game),
			mInjectCheckpointEventCallback(dicon.Resolve<SettingsStateAndEvents>()->injectCheckpointEvent, [this]() { onInject(); }),
			mccStateHook(dicon.Resolve<IMCCStateHook>()),
			messagesGUI(dicon.Resolve<IMessagesGUI>()), 
			runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()), 
			sharedMem(dicon.Resolve<ISharedMemory>())
		{
		auto ptr = dicon.Resolve<PointerManager>().get();

		mInjectRequirements = ptr->getData<std::shared_ptr<InjectRequirements>>("injectRequirements", game);
		mCheckpointLength = ptr->getData< std::shared_ptr<int64_t>>("checkpointLength", game);
		mCheckpointLocation1 = ptr->getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation1", game);

		if (!mInjectRequirements->singleCheckpoint)
		{
			mCheckpointLocation2 = ptr->getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation2", game);
			mDoubleRevertFlag = ptr->getData< std::shared_ptr<MultilevelPointer>>("doubleRevertFlag", game);
		}

		if (mInjectRequirements->preserveLocations)
		{
			mPreserveLocations = ptr->getData< std::shared_ptr<PreserveLocations>>("preserveLocations", game);
		}

		if (mInjectRequirements->SHA)
		{
			mSHAdata = ptr->getData< std::shared_ptr<offsetLengthPair>>("SHAdata", game);
		}

		if (mInjectRequirements->BSP)
		{
			mBSPdata = ptr->getData< std::shared_ptr<offsetLengthPair>>("BSPdata", game);
			mLoadedBSP1 = ptr->getData< std::shared_ptr<MultilevelPointer>>("loadedBSP1", game);
			if (!mInjectRequirements->singleCheckpoint) 	mLoadedBSP2 = ptr->getData< std::shared_ptr<MultilevelPointer>>("loadedBSP2", game);
		}

	}

		virtual std::string_view getName() override { return nameof(InjectCheckpoint); }
};

