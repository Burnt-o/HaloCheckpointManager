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

class TriggerFilterModalDialogManager : public IOptionalCheat
{

private:

	class TriggerFilterModalDialogManagerImpl
	{
	private:
		std::weak_ptr<ModalDialogRenderer> modalDialogsWeak;
		std::weak_ptr<IMCCStateHook> mccStateHookWeak;
		std::weak_ptr<SettingsStateAndEvents> settingsWeak;
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
				

				auto newFilterString = modalDialogs->showReturningDialog(ModalDialogFactory::makeTriggerFilterStringDialog("Edit Filter String", settings->triggerOverlayFilterString->GetValue(), mccStateHook->getCurrentMCCState()));

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
		TriggerFilterModalDialogManagerImpl(GameState gameImpl, IDIContainer& dicon)
			:
			filterStringChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterString->valueChangedEvent, [this](const std::string& n) { onFilterStringChanged(n); }),
			modalDialogsWeak(dicon.Resolve<ModalDialogRenderer>()),
			settingsWeak(dicon.Resolve< SettingsStateAndEvents>()),
			triggerOverlayFilterStringDialogEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterStringDialogEvent, [this]() { onTriggerOverlayFilterStringDialogEvent(); }),
			mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
			triggerOverlayFilterStringCopyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterStringCopyEvent, [this]() { onTriggerOverlayFilterStringCopyEvent(); }),
			triggerOverlayFilterStringPasteEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterStringPasteEvent, [this]() { onTriggerOverlayFilterStringPasteEvent(); })
		{

		}
	};

	static inline std::unique_ptr< TriggerFilterModalDialogManagerImpl> pimpl;

public:
	TriggerFilterModalDialogManager(GameState gameImpl, IDIContainer& dicon)
	{
		// only need once instance
		if (!pimpl)
			pimpl = std::make_unique< TriggerFilterModalDialogManagerImpl>(gameImpl, dicon);
	}

std::string_view getName() override { return nameof(TriggerFilterModalDialogManager); }
};