#pragma once
#include "pch.h"
#include "GameState.h"
#include "SettingsStateAndEvents.h"
#include "IMessagesGUI.h"
#include "IOptionalCheat.h"
#include "IGetMCCVersion.h"
#include "IMCCStateHook.h"
#include "ISharedMemory.h"
#include "RuntimeExceptionHandler.h"
#include "DIContainer.h"
#include "boost\iostreams\device\mapped_file.hpp"
#include "GetCurrentLevelCode.h"
#include "IModalDialogRenderer.h"
#include "IMakeOrGetCheat.h"
#include "GetCurrentDifficulty.h"


class InjectCore : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mInjectCoreEventCallback;

	// injected services
	gsl::not_null<std::shared_ptr<IMCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<IMessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;
	gsl::not_null<std::shared_ptr<ISharedMemory>> sharedMem;
	gsl::not_null<std::shared_ptr<SettingsStateAndEvents>> settings;
	gsl::not_null<std::shared_ptr<IModalDialogRenderer>> modal;
	gsl::not_null<std::shared_ptr<IGetMCCVersion>> getMCCVer;
	std::optional<std::shared_ptr<GetCurrentLevelCode>> levelCode;
	std::optional<std::shared_ptr<GetCurrentDifficulty>> difficulty;


	void onInject()
	{
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
		try
		{
			constexpr int minimumFileLength = 10000;
			auto currentCheckpoint = sharedMem->getInjectInfo();
			PLOG_DEBUG << "Attempting core inject for game: " << mGame.toString();
			if (currentCheckpoint.selectedCheckpointNull) throw HCMRuntimeException("Can't inject - no core save selected!");
			if ((GameState)currentCheckpoint.selectedCheckpointGame != this->mGame) throw HCMRuntimeException(std::format("Can't inject - core save from wrong game! Expected: {}, Actual: {}", this->mGame.toString(), ((GameState)currentCheckpoint.selectedCheckpointGame).toString()));
			// TODO: add version, difficulty, and levelcode checks (if user wants them)

			// check that the file actually exists
			if (!fileExists(currentCheckpoint.selectedCheckpointFilePath)) throw HCMRuntimeException(std::format("Core Save didn't exist at path: {}", currentCheckpoint.selectedCheckpointFilePath));;


			PLOG_DEBUG << "injectPath: " << currentCheckpoint.selectedCheckpointFilePath;

			//TODO: load correct level if level not aligned

			// check if user wants us to warn them on injecting to wrong level (and if we can do the check)
			if (settings->injectCoreLevelCheck->GetValue() && levelCode.has_value())
			{
				try
				{
					// compare current level code to checkpoint level code (only check first 3 characters of each)
					if (levelCode.value()->getCurrentLevelCode().substr(0, 3) != currentCheckpoint.selectedCheckpointLevelCode.substr(0, 3))
					{
						// no match! warn the user. This is a blocking call until they choose an option.
						auto continueWithInject = modal->showInjectionWarningDialog("Injection: incorrect level warning!", std::format(
							"Warning! The core save you are injecting appears to be from a different level than the one you are currently playing\n{}\nCore save level: {}\nCurrent level: {}",
							"If this is the case, the game will probably crash. Continue anyway?",
							currentCheckpoint.selectedCheckpointLevelCode.substr(0, 3),
							levelCode.value()->getCurrentLevelCode().substr(0, 3)
						));

						if (!continueWithInject) return;
					}
				}
				catch (std::out_of_range ex) // will occur on std::string.substring if string size less than substring length.
				{
					PLOG_ERROR << "out_of_range exception occured testing coresave levelCode strings: " << std::endl
						<< "currentCheckpoint.selectedCheckpointLevelCode.size(): " << currentCheckpoint.selectedCheckpointLevelCode.size() << std::endl
						<< "levelCode.value()->getCurrentLevelCode().size(): " << levelCode.value()->getCurrentLevelCode().size();
				}
			}

			// check if user wants us to warn them on injecting to wrong game version
			if (settings->injectCoreVersionCheck->GetValue())
			{
				// check if wrong game version
				if (getMCCVer->getMCCVersionAsString() != currentCheckpoint.selectedCheckpointGameVersion && currentCheckpoint.selectedCheckpointGameVersion.size() == 10)
				{
					// no match! warn the user. This is a blocking call until they choose an option.
					auto continueWithInject = modal->showInjectionWarningDialog("Injection: incorrect game version warning!", std::format(
						"Warning! The core save you are injecting appears to be from a different version of MCC than the one you are currently playing\n{}\nCheckpoint MCC ver: {}\nCurrent MCC ver: {}",
						"Core saves from different versions sometimes are, and sometimes aren't, compatible with eachother. Continue anyway?",
						currentCheckpoint.selectedCheckpointGameVersion,
						getMCCVer->getMCCVersionAsString()
					));

					if (!continueWithInject) return;
				}
			}

			// check if user wants us to warn them on injecting to wrong difficulty
			if (settings->injectCheckpointDifficultyCheck->GetValue() && difficulty.has_value() && magic_enum::enum_contains<DifficultyEnum>(currentCheckpoint.selectedCheckpointDifficulty))
			{
				// check if wrong game version
				if (difficulty.value()->getCurrentDifficulty() != (DifficultyEnum)currentCheckpoint.selectedCheckpointDifficulty)
				{
					// no match! warn the user. This is a blocking call until they choose an option.
					auto continueWithInject = modal->showInjectionWarningDialog("Injection: mismatched difficulty warning!", std::format(
						"Warning! The core save you are injecting appears to have been played on a different difficulty than the one you are currently playing\n{}\nCheckpoint Difficulty: {}\nCurrent Difficulty: {}",
						"Injecting a core save from a different difficulty can sometimes crash the game, and always messes up your pause menu interface. \nContinue anyway?",
						magic_enum::enum_name((DifficultyEnum)currentCheckpoint.selectedCheckpointDifficulty),
						magic_enum::enum_name(difficulty.value()->getCurrentDifficulty())
					));

					if (!continueWithInject) return;
				}
			}


			// store checkpoint data in a vector buffer
			// uses memory-mapped file + memcpy
			boost::iostreams::mapped_file_source checkpointFile(currentCheckpoint.selectedCheckpointFilePath);
			if (!checkpointFile.is_open()) throw HCMRuntimeException(std::format("Failed to open core save file for reading"));
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
			if (err) throw HCMRuntimeException(std::format("error loading core save data from file! code: {}", err));
			if (checkpointData.size() != checkpointLength) 	throw HCMRuntimeException(std::format("core save data was incorrect length! expected: 0x{:X}, actual: 0x{:X}", checkpointLength, checkpointData.size()));


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

			messagesGUI->addMessage(std::format("Injected checkpoint {}.bin", currentCheckpoint.selectedCheckpointName));

			PLOG_INFO << "Successfully injected coresave from " << currentCheckpoint.selectedCheckpointFilePath << " to " << coreSaveInjectLocation;

			if (settings->injectCoreForcesRevert->GetValue())
			{
				settings->forceCoreLoadEvent->operator()();
			}


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

public:


	InjectCore(GameState game, IDIContainer& dicon) 
		: mGame(game),
		mInjectCoreEventCallback(dicon.Resolve<SettingsStateAndEvents>()->injectCoreEvent, [this]() { onInject(); }),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		sharedMem(dicon.Resolve<ISharedMemory>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		modal(dicon.Resolve<IModalDialogRenderer>()),
		getMCCVer(dicon.Resolve<IGetMCCVersion>())
	{
		try
		{
			levelCode = std::dynamic_pointer_cast<GetCurrentLevelCode>(dicon.Resolve<IMakeOrGetCheat>()->getOrMakeCheat({ game, OptionalCheatEnum::GetCurrentLevelCode }, dicon));
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Inject core save couldn't acquire GetCurrentLevelCode service: " << ex.what();
		}

		try
		{
			difficulty = std::dynamic_pointer_cast<GetCurrentDifficulty>(dicon.Resolve<IMakeOrGetCheat>()->getOrMakeCheat({ game, OptionalCheatEnum::GetCurrentDifficulty }, dicon));
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Inject core save couldn't acquire GetCurrentDifficulty service: " << ex.what();
		}

	}

	~InjectCore()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(DumpCore); }


};