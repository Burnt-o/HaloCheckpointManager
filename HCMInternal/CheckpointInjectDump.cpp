#include "pch.h"
#include "CheckpointInjectDump.h"
#include "MultilevelPointer.h"
#include "InjectRequirements.h"
#include "PointerManager.h"

std::string getInjectPath()
{
	throw HCMRuntimeException("Not implemented");
}


std::string getDumpFolder()
{

}






class AllImpl : public CheckpointInjectDumpImpl {
private:
	// data
	std::shared_ptr<InjectRequirements> mInjectRequirements;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation1;
	std::shared_ptr<MultilevelPointer> mCheckpointLocation2;
	std::shared_ptr<PreserveLocations> mPreserveLocations;
	std::shared_ptr<CheckpointInfo> mCheckpointInfo;
	std::shared_ptr<SHAdata> mSHAdata;
	std::shared_ptr<BSPdata> mBSPdata;

	std::shared_ptr<MultilevelPointer> mIntegrityCheckFunction;
	std::shared_ptr<MidhookContextInterpreter> mIntegrityCheckFunctionContext;
	std::shared_ptr<ModuleMidHook> mIntegrityCheckHook;
	safetyhook::MidHookFn mIntegrityCheckHookFunction_Halo1;

public:
	void onInject() override {
		try
		{
			auto injectPath = getInjectPath();

			// check that the file actually exists
			if (!fileExists(injectPath)) throw HCMRuntimeException(std::format("Checkpoint didn't exist at path: {}", injectPath));

		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}

	void onDump() override {

	}

	AllImpl(GameState game)
	{
		mInjectRequirements = PointerManager::getData<std::shared_ptr<InjectRequirements>>("injectionRequirements", game);
		mCheckpointInfo = PointerManager::getData< std::shared_ptr<CheckpointInfo>>("checkpointInfo", game);
		mCheckpointLocation1 = PointerManager::getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation1", game);

		if (!mInjectRequirements.get()->singleCheckpoint)
		{
			mCheckpointLocation2 = PointerManager::getData< std::shared_ptr<MultilevelPointer>>("checkpointLocation2", game);
		}

		if (mInjectRequirements.get()->preserveLocations)
		{
			mPreserveLocations = PointerManager::getData< std::shared_ptr<PreserveLocations>>("preserveLocations", game);
		}

		if (mInjectRequirements.get()->SHA)
		{
			mSHAdata = PointerManager::getData< std::shared_ptr<SHAdata>>("SHAdata", game);
		}

		if (mInjectRequirements.get()->BSP)
		{
			mBSPdata = PointerManager::getData< std::shared_ptr<BSPdata>>("BSPdata", game);
		}

		if (mInjectRequirements.get()->integrityCheck)
		{
			mIntegrityCheckFunction = PointerManager::getData< std::shared_ptr<MultilevelPointer>>("integrityCheckFunction", game);
			mIntegrityCheckFunctionContext = PointerManager::getData< std::shared_ptr<MidhookContextInterpreter>>("integrityCheckFunctionContext", game);
			switch (game)
			{
			case GameState::Halo1:
				mIntegrityCheckHookFunction = [this](SafetyHookContext ctx) { return; };
			default:
				throw HCMInitException("Unsupported game");
			}

			mIntegrityCheckHook = ModuleMidHook::make(str_to_wstr(GameStateToString.at(game)), mIntegrityCheckFunction, mIntegrityCheckHookFunction, true);

			// need to setup static hook

		}
	}
};



void CheckpointInjectDump::initialize()
{
	// construct impls
	impl = std::make_unique<AllImpl>(mGameImpl);

	// subscribe to events
	mInjectCallbackHandle = OptionsState::injectCheckpointEvent.append([this]() { impl->onInject(); });
	mDumpCallbackHandle = OptionsState::dumpCheckpointEvent.append([this]() { impl->onDump(); });

	setUsability(true);
}


CheckpointInjectDump::CheckpointInjectDump(GameState game) : CheatBase(game)
{

}

