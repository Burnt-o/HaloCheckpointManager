#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ModalDialogRenderer.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "IMCCStateHook.h"
#include "boost\algorithm\string\trim.hpp"
#include "GetTriggerData.h"
#include "IMakeOrGetCheat.h"
#include <numeric>

class TriggerFilterModalDialogManager : public IOptionalCheat
{

private:

	class TriggerFilterModalDialogManagerImpl
	{
	private:
		GameState mGame;
		std::optional<std::shared_ptr<std::map<LevelID, std::vector<std::pair<std::string, std::string>>>>> levelMapStringVector;

		std::weak_ptr<ModalDialogRenderer> modalDialogsWeak;
		std::weak_ptr<IMCCStateHook> mccStateHookWeak;
		std::weak_ptr<SettingsStateAndEvents> settingsWeak;
		std::weak_ptr<GetTriggerData> getTriggerDataWeak;
		std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;

		ScopedCallback<ActionEvent> triggerOverlayFilterStringDialogEventCallback;
		ScopedCallback<eventpp::CallbackList<void(std::string& newValue)>> filterStringChangedCallback;
		ScopedCallback<ActionEvent> triggerOverlayFilterStringCopyEventCallback;
		ScopedCallback<ActionEvent> triggerOverlayFilterStringPasteEventCallback;

		// spawn dialog
		void onTriggerOverlayFilterStringDialogEvent()
		{
			try
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(modalDialogsWeak, modalDialogs);
				lockOrThrow(mccStateHookWeak, mccStateHook);

				// one at a time pls
				if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
					return;

				std::stringstream allTriggers;
				{
					lockOrThrow(getTriggerDataWeak, getTriggerData);
					auto allTriggerDataLock = getTriggerData->getAllTriggers();
					PLOG_DEBUG << "allTrigger count: " << allTriggerDataLock->size();

					int count = 0;
					for (auto& [triggerPointer, triggerData] : *allTriggerDataLock.get())
					{
						count++;
						allTriggers << std::format("{} ;", triggerData.name);
						if (count % 4 == 0)
						{
							allTriggers << std::endl;
						}
					}

				}


				auto newFilterString = modalDialogs->showReturningDialog(ModalDialogFactory::makeTriggerFilterStringDialog(
					"Edit Filter String", 
					settings->triggerOverlayFilterString->GetValue(), 
					allTriggers.str(), 
					mccStateHook->getCurrentMCCState().currentLevelID, 
					levelMapStringVector));

				settings->triggerOverlayFilterString->GetValueDisplay() = newFilterString;
				settings->triggerOverlayFilterString->UpdateValueWithInput();

			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
		}

		// update filter count label
		void onFilterStringChanged(const std::string& newValue)
		{
			try
			{
				// count entries
				std::stringstream stringstream(newValue);

				std::vector<std::string> delimitedTriggerFilter;
				std::string tmp;
				while (std::getline(stringstream, tmp, ';')) // delimit by semicolon
				{
					boost::algorithm::trim(tmp); // remove leading and trailing whitespace
					tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end()); // remove newlines
					if (tmp.size() > 0)
						delimitedTriggerFilter.push_back(tmp);
				}

				// update count label
				lockOrThrow(settingsWeak, settings);
				settings->triggerOverlayFilterCountLabel->GetValue() = delimitedTriggerFilter.size();
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}

		}

		void onTriggerOverlayFilterStringCopyEvent()
		{
			try
			{
				lockOrThrow(settingsWeak, settings);
				settings->triggerOverlayFilterString->serialiseToClipboard();
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
		}

		void onTriggerOverlayFilterStringPasteEvent()
		{
			try
			{
				lockOrThrow(settingsWeak, settings);
				settings->triggerOverlayFilterString->deserialiseFromClipboard();
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
		}

	public:
		TriggerFilterModalDialogManagerImpl(GameState game, IDIContainer& dicon)
			:
			mGame(game),
			filterStringChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterString->valueChangedEvent, [this](const std::string& n) { onFilterStringChanged(n); }),
			modalDialogsWeak(dicon.Resolve<ModalDialogRenderer>()),
			settingsWeak(dicon.Resolve< SettingsStateAndEvents>()),
			triggerOverlayFilterStringDialogEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterStringDialogEvent, [this]() { onTriggerOverlayFilterStringDialogEvent(); }),
			mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
			triggerOverlayFilterStringCopyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterStringCopyEvent, [this]() { onTriggerOverlayFilterStringCopyEvent(); }),
			triggerOverlayFilterStringPasteEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterStringPasteEvent, [this]() { onTriggerOverlayFilterStringPasteEvent(); }),
			getTriggerDataWeak(resolveDependentCheat(GetTriggerData))
		
		{
			try
			{
				auto ptr = dicon.Resolve<PointerDataStore>().lock();
				levelMapStringVector = ptr->getData<std::shared_ptr<std::map<LevelID, std::vector<std::pair<std::string, std::string>>>>>("TriggerFilterLevelPresets", game);
			}
			catch (HCMInitException ex)
			{
				PLOG_ERROR << "Could not resolve TriggerFilterLevelPresets, contiuining anyway";
			}
			
		}
	};

	std::unique_ptr< TriggerFilterModalDialogManagerImpl> pimpl;

public:
	TriggerFilterModalDialogManager(GameState gameImpl, IDIContainer& dicon)
	{
			pimpl = std::make_unique< TriggerFilterModalDialogManagerImpl>(gameImpl, dicon);
	}

std::string_view getName() override { return nameof(TriggerFilterModalDialogManager); }
};