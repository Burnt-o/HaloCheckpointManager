#include "pch.h"
#include "DumpCheckpoint.h"
#include "MultilevelPointer.h"
#include "InjectRequirements.h"
#include "PointerManager.h"
#include "RPCClientInternal.h"
#include "GameStateHook.h"






class DumpCheckpointImpl : public DumpCheckpointImplBase {
private:
	// data
	std::shared_ptr<InjectRequirements> mInjectRequirements;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation1;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation2;
	std::shared_ptr<MultilevelPointer> mDoubleRevertFlag;
	std::shared_ptr<int64_t> mCheckpointLength;

	GameState mImplGame; // which game is this implementation for

public:

	void onDump() override {
		if (GameStateHook::getCurrentGameState() != mImplGame) return;

		try
		{
			int checkpointLength = *mCheckpointLength.get();
			auto currentSaveFolder = RPCClientInternal::getDumpInfo();
			PLOG_DEBUG << "Attempting checkpoint dump for game: " << mImplGame.toString();;
			if (currentSaveFolder.selectedFolderNull) throw HCMRuntimeException("No savefolder selected, somehow?!");
			if ((GameState)currentSaveFolder.selectedFolderGame != this->mImplGame) throw HCMRuntimeException(std::format("Can't dump - savefolder from wrong game! Expected: {}, Actual: {}", this->mImplGame.toString(), ((GameState)currentSaveFolder.selectedFolderGame).toString()));

			// TODO: force checkpoint if setting se
			// get the folder to dump the checkpoint file to
			// ask the user what they want to call it
			// TODO (need to implement dialogbox stuff, and make sure we're on a seperate thread)
			auto dumpPath = currentSaveFolder.selectedFolderPath + "\\someCheckpointName.bin";
			PLOG_DEBUG << "dump path: " << dumpPath;

			// get pointer to checkpoint in memory
			uintptr_t checkpointLoc = 0;
			if (mInjectRequirements.get()->singleCheckpoint)
			{
				mCheckpointLocation1.get()->resolve(&checkpointLoc);
				PLOG_DEBUG << "using checkpoint location @0x" << std::hex << (uint64_t)checkpointLoc;
			}
			else
			{
				bool firstCheckpoint;
				mDoubleRevertFlag.get()->readData(&firstCheckpoint);
				firstCheckpoint ? mCheckpointLocation1.get()->resolve(&checkpointLoc) : mCheckpointLocation2.get()->resolve(&checkpointLoc);
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
			std::string versionString = PointerManager::getCurrentGameVersion();
			if (versionString.size() != 10) throw HCMRuntimeException(std::format("Version string was the wrong size somehow?! Expected: 10, Actual: {}", versionString.size()));
			std::copy(std::begin(versionString), std::end(versionString), std::end(checkpointData) - 10);


			// setup file stream and write
			std::ofstream dumpFile(dumpPath, std::ios::binary);
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

			PLOG_INFO << std::format("successfully dumped checkpoint from 0x{:X} to path: {}", (uint64_t)checkpointLoc, dumpPath);

		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}
	}

	DumpCheckpointImpl(GameState game) : mImplGame(game)
	{
		mInjectRequirements = PointerManager::getData<std::shared_ptr<InjectRequirements>>("injectRequirements", game);
		mCheckpointLength = PointerManager::getData< std::shared_ptr<int64_t>>("checkpointLength", game);
		mCheckpointLocation1 = PointerManager::getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation1", game);

		if (!mInjectRequirements.get()->singleCheckpoint)
		{
			mCheckpointLocation2 = PointerManager::getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation2", game);
			mDoubleRevertFlag = PointerManager::getData< std::shared_ptr<MultilevelPointer>>("doubleRevertFlag", game);
		}


	}
};



void DumpCheckpoint::initialize()
{
	// construct impls
	PLOG_DEBUG << "dump checkpoint constructing AllImpl for game: " << mGame;
	impl = std::make_unique<DumpCheckpointImpl>(mGame);

	// subscribe to events (this won't happen if impl construction failed cause of missing pointer data or w/e)
	mDumpCallbackHandle = OptionsState::dumpCheckpointEvent.get()->append([this]() {
		impl.get()->onDump();
		});

	//.. oh. if initializing fails, event not unsubscribed? mGame not set properly? hm

}


