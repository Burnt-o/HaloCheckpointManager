#include "pch.h"
#include "TriggerFilter.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "GetTriggerData.h"
#include "IMakeOrGetCheat.h"

class TriggerFilterImpl
{

private:
	
	// injected services
	std::weak_ptr<GetTriggerData> getTriggerDataWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr< SettingsStateAndEvents> settings;

	// callbacks
	ScopedCallback<eventpp::CallbackList<void(std::string& newValue)>> filterStringChangedCallback;
	ScopedCallback<ToggleEvent> filterToggleChangedCallback;

	void onFilterToggleChanged(bool& newValue)
	{
		onFilterStringChanged(settings->triggerOverlayFilterString->GetValue());
	}

	void onFilterStringChanged(std::string& newValue)
	{
		try
		{
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
		: filterStringChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterString->valueChangedEvent, [this](std::string& n) {onFilterStringChanged(n); }),
		filterToggleChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterToggle->valueChangedEvent, [this](bool& n) {onFilterToggleChanged(n); }),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settings(dicon.Resolve<SettingsStateAndEvents>())
	{

	}

};

TriggerFilter::TriggerFilter(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<TriggerFilterImpl>(gameImpl, dicon);
}

TriggerFilter::~TriggerFilter() = default;