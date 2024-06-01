#include "pch.h"
#include "TriggerFilter.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "GetTriggerData.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"

class TriggerFilterImpl
{

private:
	GameState mGame;

	// injected services
	std::weak_ptr<GetTriggerData> getTriggerDataWeak;
	std::weak_ptr< IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr< SettingsStateAndEvents> settings;

	// callbacks
	ScopedCallback<eventpp::CallbackList<void(std::string& newValue)>> filterStringChangedCallback;
	ScopedCallback<ToggleEvent> filterToggleChangedCallback;

	void onFilterToggleChanged(const bool& newValue)
	{
		onFilterStringChanged(settings->triggerOverlayFilterString->GetValue());
	}

	void onFilterStringChanged(const std::string& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
				return;

			lockOrThrow(getTriggerDataWeak, getTriggerData);
			auto triggerDataLock = getTriggerData->getTriggerData();

			triggerDataLock->filteredTriggers->clear();

			if (settings->triggerOverlayFilterToggle->GetValue())
			{
				PLOG_DEBUG << "filtering trigger data by string: " << std::endl << newValue;
				std::stringstream stringstream(newValue);

				std::vector<std::string> delimitedTriggerFilter;
				std::string tmp;
				while (std::getline(stringstream, tmp, ',')) // delimit by comma
				{
					// remove whitespace
					tmp.erase(std::remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
					delimitedTriggerFilter.push_back(tmp);
				}




				for (auto& [triggerPointer, triggerData] : *triggerDataLock->allTriggers.get())
				{
					for (auto& filterName : delimitedTriggerFilter)
					{
						if (triggerData.name == filterName)
						{
							triggerDataLock->filteredTriggers->emplace(triggerPointer, triggerData);
							break;
						}

					}
				}
			}
			else
			{
				PLOG_DEBUG << "unfiltering trigger data";
				for (auto& [triggerPointer, triggerData] : *triggerDataLock->allTriggers.get())
				{
					triggerDataLock->filteredTriggers->emplace(triggerPointer, triggerData);
				}
			}
			


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	TriggerFilterImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		filterStringChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterString->valueChangedEvent, [this](const std::string& n) {onFilterStringChanged(n); }),
		filterToggleChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterToggle->valueChangedEvent, [this](bool& n) {onFilterToggleChanged(n); }),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mccStateHookWeak(dicon.Resolve< IMCCStateHook>()),
		settings(dicon.Resolve<SettingsStateAndEvents>())
	{

	}

};

TriggerFilter::TriggerFilter(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<TriggerFilterImpl>(gameImpl, dicon);
}

TriggerFilter::~TriggerFilter() = default;